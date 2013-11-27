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

#include "MirrorDownload.h"
#include <stdexcept>
#include <ctime>

#include "core/logger.hpp"

namespace tdm
{

MirrorDownload::MirrorDownload(const HttpConnectionPtr& conn,
								 const MirrorList& mirrors,
								 const std::string& srcFilename,
								 vfs::Path destFilename) :
	Download(conn, "", destFilename) // empty URL for starters
{
	if (mirrors.empty())
	{
		throw std::runtime_error("No mirrors available, cannot continue.");
	}

	// Pick a random mirror
	float p = (rand() % 100) / 100.f;

	// Generate the list of URLs to use, starting with the "preferred"
	// mirror on top of the list. The "preferred" mirror is the one picked
	// by the random number generator, respecting the weights.
	std::list<std::string> urls;
	std::list<Mirror> orderedMirrors;

	// Re-sort the incoming mirrors into the URL list
	for (MirrorList::const_iterator m = mirrors.begin(); m != mirrors.end(); ++m)
	{
		if (p < m->weight)
		{
			// Preferred mirror, add at front
			p = 1000000.0f; // make sure the rest of the mirrors isn't pushed at the front
			orderedMirrors.push_front(*m);
			urls.push_front(m->url + srcFilename);
			Logger::warning( "Picking mirror %s for file %s", m->displayName.c_str(), srcFilename.c_str() );
		}
		else
		{
			// Non-preferred mirror, add at the end of the list
			p -= m->weight;
			urls.push_back(m->url + srcFilename);
			orderedMirrors.push_back(*m);
		}
	}

	if (urls.empty())
	{
		throw std::runtime_error("No URLs available after mirror sort, cannot continue.");
	}

	// Push the sorted URLs into the protected member
	_urls = std::vector<std::string>(urls.begin(), urls.end());

	// Push the sorted Mirrors into our own _mirrors member
	_mirrors = std::vector<Mirror>(orderedMirrors.begin(), orderedMirrors.end());
}

std::string MirrorDownload::GetCurrentMirrorName()
{
	return (_curUrl < _mirrors.size()) ? _mirrors[_curUrl].displayName : "";
}

void MirrorDownload::InitRandomizer()
{
	srand( std::time(0) );
}

} // namespace
