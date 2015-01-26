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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "loader.hpp"

#include "loader_map.hpp"
#include "loader_sav.hpp"
#include "loader_oc3save.hpp"
#include "loader_mission.hpp"
#include "loader_omap.hpp"
#include "core/position.hpp"
#include "gfx/tilemap.hpp"
#include "core/utils.hpp"
#include "gfx/helper.hpp"
#include "resourcegroup.hpp"
#include "gfx/animation_bank.hpp"
#include "core/foreach.hpp"
#include "game.hpp"
#include "objects/objects_factory.hpp"
#include "city/city.hpp"
#include "core/logger.hpp"
#include "objects/waymark.hpp"
#include "climatemanager.hpp"

#include <vector>

using namespace gfx;
using namespace constants;

namespace game
{

namespace loader
{
typedef SmartPtr< loader::Base > BasePtr;
}

class Loader::Impl
{
public:
  typedef std::vector< loader::BasePtr > Loaders;
  typedef Loaders::iterator LoaderIterator;
  Loaders loaders;
  std::string restartFile;

  void initLoaders();
  void initEntryExitTile( const TilePos& tlPos, PlayerCityPtr city );
  void initTilesAnimation( Tilemap& tmap );
  void finalize( Game& game );
  bool maySetSign( const Tile& tile )
  {
    return (tile.isWalkable( true ) && !tile.getFlag( Tile::tlRoad)) || tile.getFlag( Tile::tlTree );
  }
};

Loader::Loader() : _d( new Impl )
{
  _d->initLoaders();
}

Loader::~Loader() {}

void Loader::Impl::initEntryExitTile( const TilePos& tlPos, PlayerCityPtr city )
{
  TilePos tlOffset;
  Tilemap& tmap = city->tilemap();
  if( tlPos.i() == 0 || tlPos.i() == tmap.size() - 1 )
  {    
    tlOffset = TilePos( 0, 1 );
  }
  else if( tlPos.j() == 0 || tlPos.j() == tmap.size() - 1 )
  {
    tlOffset = TilePos( 1, 0 );
  }

  Tile& tmpTile = tmap.at( tlPos + tlOffset );
  if( !maySetSign( tmpTile ) )
  {
    tlOffset = -tlOffset;
  }

  Tile& signTile = tmap.at( tlPos + tlOffset );

  Logger::warning( "(%d, %d)", tlPos.i(),    tlPos.j()    );
  Logger::warning( "(%d, %d)", tlOffset.i(), tlOffset.j() );

  if( maySetSign( signTile ) )
  {
    tile::clear( signTile );
    gfx::TileOverlayPtr waymark = TileOverlayFactory::instance().create( constants::objects::waymark );
    CityAreaInfo info = { city, tlPos + tlOffset, TilesArray() };
    waymark->build( info );
    city->addOverlay( waymark );
  }
}

void Loader::Impl::initTilesAnimation( Tilemap& tmap )
{
  TilesArray area = tmap.getArea( TilePos( 0, 0 ), Size( tmap.size() ) );

  foreach( it, area )
  {
    int rId = (*it)->originalImgId() - 364;
    if( rId >= 0 && rId < 8 )
    {
      Animation water = AnimationBank::simple( AnimationBank::animWater );
      water.setIndex( rId );
      (*it)->setAnimation( water );
      (*it)->setFlag( Tile::tlDeepWater, true );
    }

    if( (*it)->getFlag( Tile::tlMeadow ) )
    {
      const Animation& meadow = AnimationBank::simple( AnimationBank::animMeadow );
      if( !(*it)->picture().isValid() )
      {
        Picture pic = MetaDataHolder::randomPicture( objects::terrain, Size(1) );
        (*it)->setPicture( pic );
      }
      (*it)->setAnimation( meadow );
    }
  }
}

void Loader::Impl::finalize( Game& game )
{
  Tilemap& tileMap = game.city()->tilemap();

  // exit and entry can't point to one tile or .... can!
  const BorderInfo& border = game.city()->borderInfo();

  initEntryExitTile( border.roadEntry, game.city() );
  initEntryExitTile( border.roadExit,  game.city() );

  initTilesAnimation( tileMap );
}

void Loader::Impl::initLoaders()
{
  loaders.push_back( new loader::C3Map() );
  loaders.push_back( new loader::C3Sav() );
  loaders.push_back( new loader::OC3() );
  loaders.push_back( new loader::Mission() );
  loaders.push_back( new loader::OMap() );
}

bool Loader::load( vfs::Path filename, Game& game )
{
  // try to load file based on file extension
  Impl::LoaderIterator it = _d->loaders.begin();
  for( ; it != _d->loaders.end(); ++it)
  {
    if( (*it)->isLoadableFileExtension( filename.toString() ) /*||
        (*it)->isLoadableFileFormat(file) */ )
    {
      ClimateType currentClimate = (ClimateType)(*it)->climateType( filename.toString() );
      if( currentClimate >= 0  )
      {
        game::climate::initialize( currentClimate );
      }

      bool loadok = (*it)->load( filename.toString(), game );      
      
      if( loadok )
      {
        _d->restartFile = (*it)->restartFile();
        _d->finalize( game );
      }

      return loadok;
    }
  }
  Logger::warning( "GameLoader: not found loader for " + filename.toString() );

  return false; // failed to load
}

std::string Loader::restartFile() const { return _d->restartFile; }

}//end namespace game
