// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "inifile.hpp"

#include "core/logger.hpp"
#include "core/utils.hpp"
#include <set>
#include <fstream>
#include <sstream>
#include <iterator>

namespace updater
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

IniFilePtr IniFile::ConstructFromFile(vfs::Path filename)
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
  std::string buffer;
  std::ostringstream ors;
  ors << stream.rdbuf();
  buffer = ors.str();
    
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
	Sections::iterator it = _settings.find( name );
	if( it == _settings.end() )
	{
		_settings.insert( make_pair( name, Options() ) );
	}
}

std::string IniFile::GetValue(const std::string& section, const std::string& key) const
{
	Sections::const_iterator i = _settings.find(section);
	
	if (i == _settings.end()) return ""; // section not found
	
	for( Options::const_iterator kv=i->second.begin(); kv!=i->second.end(); kv++ )
	{
		if( kv->key == key )
		{
			return kv->value;
		}
	}

	return "";
}

void IniFile::SetValue(const std::string& section, const std::string& key, const std::string& value)
{
	Sections::iterator i = _settings.find(section);

	if (i == _settings.end() )// section not found
	{
		AddSection( section );
	}

	i = _settings.find(section);
	for( Options::iterator kv=i->second.begin(); kv!=i->second.end(); kv++ )
	{
		if( kv->key == key )
		{
			kv->value = value;
			return;
		}
	}

	Option op = { key, value };
	i->second.push_back( op );
}

bool IniFile::RemoveSection(const std::string& section)
{
	Sections::iterator i = _settings.find(section);

	if (i != _settings.end())
	{
		_settings.erase(i);
		return true;
	}

	return false; // not found
}

bool IniFile::RemoveKey(const std::string& section, const std::string& key)
{
	Sections::iterator i = _settings.find(section);

	if (i == _settings.end())
	{
		return false; // not found
	}

	for( Options::iterator kv=i->second.begin(); kv!=i->second.end(); kv++ )
	{
		if( kv->key == key )
		{
			i->second.erase( kv );
			return true;
		}
	}

	return false; // not found
}

IniFile::Options IniFile::GetAllKeyValues(const std::string& section) const
{
	Sections::const_iterator i = _settings.find(section);

	if (i == _settings.end())
	{
		return Options(); // not found
	}

	return i->second;
}

void IniFile::ForeachSection(SectionVisitor& visitor) const
{
	for( Sections::const_iterator i = _settings.begin();
		 i != _settings.end(); /* in-loop increment */)
	{
		visitor.VisitSection(*this, (*i++).first);
	}
}

void IniFile::ExportToFile(vfs::Path file, const std::string& headerComments) const
{
	std::ofstream stream(file.toString().c_str());

	if (!headerComments.empty())
	{
		// Split the header text into lines and export it as INI comment
		std::vector<std::string> lines = utils::split( headerComments, "\n" );

		for (std::size_t i = 0; i < lines.size(); ++i)
		{
			stream << "# " << lines[i] << std::endl;
		}

		// add some additional line break after the header
		stream << std::endl;
	}

	for (Sections::const_iterator i = _settings.begin(); i != _settings.end(); ++i)
	{
		stream << "[" << i->first << "]" << std::endl;

		for (Options::const_iterator kv = i->second.begin(); kv != i->second.end(); ++kv)
		{
			stream << kv->key << " = " << kv->value << std::endl;
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

		_lastKey = utils::replace( _lastKey, " ", "" );
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
		sRead = utils::trim( sRead );
		sRead = utils::replace( sRead, "\r", "");
		sRead = utils::replace( sRead, "\n", "");
		if( !sRead.empty() )
		{
			RR nType = ( sRead.find_first_of("[") == 0 && ( sRead[sRead.find_last_not_of(" \t\r\n")] == ']' ) ) ? SECTION : OTHER ;
			std::string::size_type equaleSymbolPos = sRead.find_first_of("=");
			nType = ( (nType == OTHER) && ( equaleSymbolPos > 0 && equaleSymbolPos != std::string::npos ) ) ? KEY : nType ;
			nType = ( (nType == OTHER) && ( sRead.find_first_of("#") == 0) ) ? COMMENT : nType ;
			switch( nType )
			{
				case SECTION:
					sectionName = utils::trim( sRead.substr( 1 , sRead.size() - 2 ) );
					AddSection( sectionName );
				break;

				case KEY:
				{
					// Check to ensure valid section... or drop the keys listed
					if( !sectionName.empty() )
					{
						size_t iFind = sRead.find_first_of("=");
						std::string sKey = utils::trim( sRead.substr(0,iFind) );
						std::string sValue = utils::trim( sRead.substr(iFind + 1) );
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
