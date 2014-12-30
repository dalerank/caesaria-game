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

#ifndef __CAESARIA_MIRRORLIST_H_INLCUDE__
#define __CAESARIA_MIRRORLIST_H_INLCUDE__

#include "../inifile.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"

#include <map>
//#include <boost/algorithm/string/predicate.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/algorithm/string/erase.hpp> // grayman #3208

namespace updater
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
		url = utils::replace( url, " ", "");

		// url should terminate with "/"
		if( *url.rbegin() != '/' )
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
	
	virtual ~MirrorList() {}

	// Construct this mirror list from the given ini file
	MirrorList(IniFilePtr iniFile)
	{
		iniFile->ForeachSection(*this);

		NormaliseWeights();
	}

	void VisitSection(const IniFile& iniFile, const std::string& sectionName)
	{
		if( !utils::startsWith( sectionName, "Mirror ") )
		{
			return; // ignore non-Mirror sections
		}

		push_back( Mirror( sectionName.substr(7), // displayname
											 iniFile.GetValue( sectionName, "url" ),
											 utils::toFloat( iniFile.GetValue(sectionName, "weight") ))
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

#endif //__CAESARIA_MIRRORLIST_H_INLCUDE__
