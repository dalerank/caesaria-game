/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5122 $ (Revision of last commit) 
 $Date: 2011-12-11 23:47:31 +0400 (Вс, 11 дек 2011) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include "Http/HttpConnection.h"
#include "core/logger.hpp"
#include "core/variant.hpp"
#include <sstream>

namespace tdm
{

class ProgramOptions
{
protected:
	std::map< std::string, std::string > _desc;
	VariantMap _vm;

	// The command line arguments for reference
	StringArray _cmdLineArgs;

public:
	virtual ~ProgramOptions()
	{}

	void ParseFromCommandLine(int argc, char* argv[])
	{
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

	std::string Get(const std::string& key) const
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
