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

#ifndef __CAESARIA_UPDATER_PROGRAMM_OPTIONS_H_INLCUDE__
#define __CAESARIA_UPDATER_PROGRAMM_OPTIONS_H_INLCUDE__

#include "http/httpconnection.hpp"
#include "core/logger.hpp"
#include "core/variant.hpp"
#include "core/variant_map.hpp"
#include <sstream>

namespace updater
{

class ProgramOptions
{
protected:
	std::map< std::string, std::string > _desc;
	VariantMap _vm;

	// The command line arguments for reference
	StringArray _cmdLineArgs;
	std::string _runPath;

public:
	virtual ~ProgramOptions()	{}

	void ParseFromCommandLine(int argc, char* argv[])
	{
		_runPath = argv[0];

		for (int i = 0; i < argc; i++)
		{
			std::string optName = (*argv[i] == '-' ? argv[i] : "");
			std::string optValue = (i+1 != argc
																? (*argv[i+1] == '-' ? "" : argv[i+1] )
																: "" );
			if( !optName.empty() )
			{
				if( optName[0] == '-' ) optName = optName.substr( 1 );
				if( optName[0] == '-' ) optName = optName.substr( 1 );

				if( optValue.empty() )
				{
					_vm[ optName ] = true;
				}
				else
				{
					_vm[ optName ] = Variant( optValue );
					i++;
				}
			}
		}
	}

	std::string runPath() const 	{		return _runPath;	}

	void Set(const std::string& key)
	{
		_vm[ key ] = true;
		_cmdLineArgs.push_back("--" + key);
	}

	void Set(const std::string& key, const std::string& value)
	{
		_vm[ key ] = Variant( value );
		_cmdLineArgs.push_back("--" + key + " " + value);
	}

	void Reset(const std::string& key)
	{
		_vm.erase( key );

		for( StringArray::iterator i = _cmdLineArgs.begin();
			 i != _cmdLineArgs.end(); ++i)
		{
			if( *i == ("--" + key) )
			{
				_cmdLineArgs.erase(i);
				break;
			}
		}
	}

	bool empty() const
	{
		return _vm.empty();
	}

	bool isSet(const std::string& key) const
	{
		return _vm.count(key) > 0;
	}

	std::string get(const std::string& key) const
	{
		return _vm.count(key) > 0 ? _vm.get( key ).toString() : "";
	}

	const StringArray& GetRawCmdLineArgs() const
	{
		return _cmdLineArgs;
	}

	virtual void PrintHelp()
	{
		for( std::map< std::string, std::string >::iterator i=_desc.begin(); i!=_desc.end(); i++ )
		{
			Logger::warning( "%s\t%s", i->first.c_str(), i->second.c_str() );
		}
	}

protected:
	/**
	 * Subclasses should implement this method to populate the available options
	 * and call it in their constructors.
	 */
	virtual void SetupDescription() = 0;
};

}

#endif //__CAESARIA_UPDATER_PROGRAMM_OPTIONS_H_INLCUDE__
