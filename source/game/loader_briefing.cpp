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

#include "loader_briefing.hpp"
#include "core/saveadapter.hpp"

class GameLoaderBriefing::Impl
{
public:
  static const int currentVesion = 1;
};

GameLoaderBriefing::GameLoaderBriefing()
{
}

bool GameLoaderBriefing::load( const std::string& filename, Game& game )
{
  VariantMap vm = SaveAdapter::load( filename );
  
  if( Impl::currentVesion == vm[ "version" ].toInt() )
  {
    std::string mapToLoad = vm[ "map" ].toString();

    return true;
  }
 
  return false;
}

bool GameLoaderBriefing::isLoadableFileExtension( const std::string& filename )
{
  vfs::Path path( filename );
  return path.isExtension( ".briefing" );
}
