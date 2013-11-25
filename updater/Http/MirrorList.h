/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5534 $ (Revision of last commit) 
 $Date: 2012-08-24 19:29:25 +0400 (Пт, 24 авг 2012) $ (Date of last commit)
 $Author: grayman $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include "IniFile.h"
#include "core/logger.hpp"
#include "core/stringhelper.hpp"

#include <map>
//#include <boost/algorithm/string/predicate.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/algorithm/string/erase.hpp> // grayman #3208

namespace tdm
{

// Information about a TDM mirror
struct Mirror
{
	// Display name, e.g. "KeepOfMetalAndGold"
	std::string displayName;

	// URL prefix, e.g. "http://www.keepofmetalandgold.com/files/tdm/"
	std::string	url;

	// Probability [0..1]
	float weight;

	Mirror(const std::string& displayName_,
				 const std::string& url_,
				 float weight_) :
		displayName(displayName_),
		url(url_),
		weight(weight_)
	{
		// grayman #3208 - remove all spaces in the url
		url = StringHelper::replace( url, " ", "");

		// url should terminate with "/"
		if( *url.rend() != '/' )
		{
			url += "/";
		}
	}
};

/**
 * The list of mirrors available for download.
 * Each mirror has a certain "weight" to accommodate
 * the various download limits imposed on the people
 * who are generously offering to host our files.
 */
class MirrorList : public std::vector<Mirror>, public IniFile::SectionVisitor
{
public:
	MirrorList()
	{}

	// Construct this mirror list from the given ini file
	MirrorList(IniFilePtr iniFile)
	{
		iniFile->ForeachSection(*this);

		NormaliseWeights();
	}

	void VisitSection(const IniFile& iniFile, const std::string& sectionName)
	{
		if( !StringHelper::startsWith( sectionName, "Mirror ") )
		{
			return; // ignore non-Mirror sections
		}

		push_back( Mirror( sectionName.substr(7), // displayname
											 iniFile.GetValue( sectionName, "url" ),
											 StringHelper::toFloat( iniFile.GetValue(sectionName, "weight") ))
						 );
	}

private:
	void NormaliseWeights()
	{
		// Calculate weight sum
		float sum = 0;

		for (const_iterator i = begin(); i != end(); ++i)
		{
			sum += i->weight;
		}

		// Normalise weights
		if (sum > 0)
		{
			for (iterator i = begin(); i != end(); ++i)
			{
				i->weight /= sum;
			}
		}
		else
		{
			Logger::warning( "Invalid weights, total sum <= 0" );
		}
	}
};

} // namespace
