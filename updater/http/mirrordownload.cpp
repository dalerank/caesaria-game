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

#include "mirrordownload.hpp"
#include <stdexcept>
#include <ctime>

#include "core/logger.hpp"

namespace updater
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
