/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5598 $ (Revision of last commit) 
 $Date: 2012-10-19 19:46:11 +0400 (Пт, 19 окт 2012) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#include "IniFile.h"

#include "core/logger.hpp"
#include "core/stringhelper.hpp"
#include <set>
#include <fstream>
#include <sstream>

namespace tdm
{

IniFile::IniFile()
{}

IniFile::IniFile(const std::string& str)
{
	ParseFromString(str);
}

IniFilePtr IniFile::Create()
{
	return IniFilePtr(new IniFile);
}

IniFilePtr IniFile::ConstructFromFile(const io::FilePath& filename)
{
	// Start parsing
	std::ifstream iniFile(filename.toString().c_str());

	if (!iniFile)
	{
		Logger::warning( "[IniFile]: Cannot open file " + filename.toString() );
		return IniFilePtr();
	}

	return ConstructFromStream(iniFile);
}

IniFilePtr IniFile::ConstructFromStream(std::istream& stream)
{
	// Read the whole stream into a string
	std::string buffer(std::istreambuf_iterator<char>(stream), (std::istreambuf_iterator<char>()));

	return ConstructFromString(buffer);
}

IniFilePtr IniFile::ConstructFromString(const std::string& str)
{
	return IniFilePtr(new IniFile(str));
}

bool IniFile::IsEmpty() const
{
	return _settings.empty();
}

void IniFile::AddSection(const std::string& name)
{
	_settings.insert(SettingMap::value_type(name, KeyValues()));
}

std::string IniFile::GetValue(const std::string& section, const std::string& key) const
{
	SettingMap::const_iterator i = _settings.find(section);
	
	if (i == _settings.end()) return ""; // section not found
	
	KeyValues::const_iterator kv = i->second.find(KeyValuePair(key, ""));

	return (kv != i->second.end()) ? kv->second : "";
}

void IniFile::SetValue(const std::string& section, const std::string& key, const std::string& value)
{
	// Find the section, and create it if necessary
	SettingMap::iterator i = _settings.find(section);

	if (i == _settings.end())
	{
		AddSection(section);
	}

	// Section exists past this point

	KeyValues::iterator kv = _settings[section].find(KeyValuePair(key, ""));

	// Remove existing key value first
	if (kv != _settings[section].end())
	{
		_settings[section].erase(kv);
	}

	// Insert afresh
	_settings[section].insert(KeyValuePair(key, value));
}

bool IniFile::RemoveSection(const std::string& section)
{
	SettingMap::iterator i = _settings.find(section);

	if (i != _settings.end())
	{
		_settings.erase(i);
		return true;
	}

	return false; // not found
}

bool IniFile::RemoveKey(const std::string& section, const std::string& key)
{
	SettingMap::iterator i = _settings.find(section);

	if (i == _settings.end())
	{
		return false; // not found
	}

	KeyValues::iterator kv = i->second.find(KeyValuePair(key, ""));

	if (kv != i->second.end())
	{
		i->second.erase(kv);
		return true;
	}

	return false; // not found
}

IniFile::KeyValuePairList IniFile::GetAllKeyValues(const std::string& section) const
{
	SettingMap::const_iterator i = _settings.find(section);

	if (i == _settings.end())
	{
		return KeyValuePairList(); // not found
	}

	return KeyValuePairList(i->second.begin(), i->second.end());
}

void IniFile::ForeachSection(SectionVisitor& visitor) const
{
	for (SettingMap::const_iterator i = _settings.begin(); 
		 i != _settings.end(); /* in-loop increment */)
	{
		visitor.VisitSection(*this, (*i++).first);
	}
}

void IniFile::ExportToFile(const io::FilePath& file, const std::string& headerComments) const
{
	std::ofstream stream(file.toString().c_str());

	if (!headerComments.empty())
	{
		// Split the header text into lines and export it as INI comment
		std::vector<std::string> lines = StringHelper::split( headerComments, "\n" );

		for (std::size_t i = 0; i < lines.size(); ++i)
		{
			stream << "# " << lines[i] << std::endl;
		}

		// add some additional line break after the header
		stream << std::endl;
	}

	for (SettingMap::const_iterator i = _settings.begin(); i != _settings.end(); ++i)
	{
		stream << "[" << i->first << "]" << std::endl;

		for (KeyValues::const_iterator kv = i->second.begin(); kv != i->second.end(); ++kv)
		{
			stream << kv->first << " = " << kv->second << std::endl;
		}
		
		stream << std::endl;
	}
}

// Functor class adding INI sections and keyvalues
// Keeps track of the most recently added section and key
// as the Add* methods are called in the order of parsing without context.
class IniParser
{
private:
	IniFile& _self;
	
	// Most recently added section and key
	std::string _lastSection;
	std::string _lastKey;

public:
	IniParser(IniFile& self) :
		_self(self)
	{}

	void AddSection(char const* beg, char const* end)
	{
		// Remember this section name
		_lastSection = std::string(beg, end);

		_self.AddSection(_lastSection);
	}

	void AddKey(char const* beg, char const* end)
	{
		assert(!_lastSection.empty()); // need to have parsed a section beforehand

		// Just remember the key name, an AddValue() call is imminent
		_lastKey = std::string(beg, end);

		_lastKey = StringHelper::replace( _lastKey, " ", "" );
	}

	void AddValue(char const* beg, char const* end)
	{
		assert(!_lastSection.empty());
		assert(!_lastKey.empty());

		_self.SetValue(_lastSection, _lastKey, std::string(beg, end));
	}
};

void IniFile::ParseFromString(const std::string& str)
{
	_settings.clear();

	std::string sectionName;
	std::string sRead;

	typedef enum { KEY , SECTION , COMMENT , OTHER } RR;
	std::istringstream iss(str);
	while( std::getline( iss, sRead ) )
	{
		sRead = StringHelper::replace( sRead, " ", "");
		sRead = StringHelper::replace( sRead, "\r", "");
		sRead = StringHelper::replace( sRead, "\n", "");
		if( !sRead.empty() )
		{
			RR nType = ( sRead.find_first_of("[") == 0 && ( sRead[sRead.find_last_not_of(" \t\r\n")] == ']' ) ) ? SECTION : OTHER ;
			nType = ( (nType == OTHER) && ( sRead.find_first_of("=") ) > 0 ) ? KEY : nType ;
			nType = ( (nType == OTHER) && ( sRead.find_first_of("#") == 0) ) ? COMMENT : nType ;
			switch( nType )
			{
				case SECTION:
					sectionName = sRead.substr( 1 , sRead.size() - 2 );
					AddSection( sectionName );
				break;

				case KEY:
				{
					// Check to ensure valid section... or drop the keys listed
					if( !sectionName.empty() )
					{
						size_t iFind = sRead.find_first_of("=");
						std::string sKey = sRead.substr(0,iFind);
						std::string sValue = sRead.substr(iFind + 1);
						SetValue( sectionName, sKey, sValue );
					}
				}
				break;

				case COMMENT:
				break;

				case OTHER:
				break;
			}
		}
	}
}

} // namespace
