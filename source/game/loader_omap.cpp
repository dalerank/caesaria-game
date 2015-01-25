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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "loader_omap.hpp"
#include "gfx/helper.hpp"
#include "city/city.hpp"
#include "core/variant_map.hpp"
#include "game.hpp"
#include "core/exception.hpp"
#include "objects/objects_factory.hpp"
#include "core/utils.hpp"
#include "gfx/tilemap.hpp"
#include "world/empire.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "loaderhelper.hpp"
#include "core/saveadapter.hpp"
#include "vfs/archive_zip.hpp"

using namespace constants;
using namespace gfx;

#include <map>

namespace game
{

namespace loader
{

static const char* rawJson = ".omap";
static const char* zippedJson = ".zmap";

class OMap::Impl
{
public:
  std::string restartFile;
  int climate;
  void loadCity( const VariantMap& vm, PlayerCityPtr oCity );

  void initCameraStartPos(const VariantMap &vm, PlayerCityPtr ioCity);
};

bool OMap::load(const std::string& filename, Game& game)
{
  _d->restartFile = filename;
  VariantMap vm;
  if( vfs::Path( filename ).isMyExtension( ".zmap" ) )
  {
    Logger::warning( "OMap not loading citymap from zip" );
    return false;
  }
  else
  {
    vm = config::load( filename );
  }

  _d->climate = vm.get( "climate" ).toInt();

  _d->loadCity( vm, game.city() );
  _d->initCameraStartPos(vm, game.city() );

  game.city()->setOption( PlayerCity::adviserEnabled, 1 );

  game.empire()->setCitiesAvailable( true );

  return true;
}

int OMap::climateType(const std::string& filename) {  return _d->climate; }
std::string OMap::restartFile() const {  return _d->restartFile; }
OMap::OMap() : _d( new Impl ) {}

bool OMap::isLoadableFileExtension( const std::string& filename )
{
  return vfs::Path( filename ).isMyExtension( rawJson )||
         vfs::Path( filename ).isMyExtension( zippedJson );
}

void OMap::Impl::loadCity( const VariantMap& vm, PlayerCityPtr oCity)
{
  oCity->load( vm );
}

void OMap::Impl::initCameraStartPos(const VariantMap& vm, PlayerCityPtr ioCity)
{
  int mapSize = ioCity->tilemap().size();
  ioCity->setCameraPos( TilePos( mapSize / 2, mapSize / 2 ) );
}

}//end namespace loader

}//end namespace game
