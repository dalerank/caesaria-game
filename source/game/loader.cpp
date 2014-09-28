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
#include "core/position.hpp"
#include "gfx/tilemap.hpp"
#include "core/stringhelper.hpp"
#include "gfx/tile.hpp"
#include "resourcegroup.hpp"
#include "core/foreach.hpp"
#include "game.hpp"
#include "city/city.hpp"
#include "core/logger.hpp"
#include "climatemanager.hpp"

#include <vector>
#include <core/saveadapter.hpp>

using namespace gfx;

typedef SmartPtr< GameAbstractLoader > GameAbstractLoaderPtr;

class GameLoader::Impl
{
public:
  typedef std::vector< GameAbstractLoaderPtr > Loaders;
  typedef Loaders::iterator LoaderIterator;
  Loaders loaders;
  std::string restartFile;

  void initLoaders();
  void initEntryExitTile( const TilePos& tlPos, Tilemap& tileMap, const unsigned int picIdStart, bool exit );
  void initWaterTileAnimation( Tilemap& tmap );
  void finalize( Game& game );
  bool maySetSign( const Tile& tile )
  {
    return (tile.isWalkable( true ) && !tile.getFlag( Tile::tlRoad)) || tile.getFlag( Tile::tlTree );
  }
};

GameLoader::GameLoader() : _d( new Impl )
{
  _d->initLoaders();
}

GameLoader::~GameLoader() {}

void GameLoader::Impl::initEntryExitTile( const TilePos& tlPos, Tilemap& tileMap, const unsigned int picIdStart, bool exit )
{
  unsigned int idOffset = 0;
  TilePos tlOffset;
  if( tlPos.i() == 0 || tlPos.i() == tileMap.size() - 1 )
  {    
    tlOffset = TilePos( 0, 1 );
    idOffset = (tlPos.i() == 0 ? 1 : 3 );
  }

  else if( tlPos.j() == 0 || tlPos.j() == tileMap.size() - 1 )
  {
    tlOffset = TilePos( 1, 0 );
    idOffset = (tlPos.j() == 0 ? 2 : 0 );
  }

  Tile& tmpTile = tileMap.at( tlPos + tlOffset );
  if( !maySetSign( tmpTile ) )
  {
    tlOffset = -tlOffset;
  }

  Tile& signTile = tileMap.at( tlPos + tlOffset );  

  Logger::warning( "(%d, %d)", tlPos.i(),    tlPos.j()    );
  Logger::warning( "(%d, %d)", tlOffset.i(), tlOffset.j() );

  if( maySetSign( signTile ) )
  {
    signTile.setPicture( ResourceGroup::land3a, picIdStart + idOffset );
    signTile.setFlag( Tile::clearAll, true );
    signTile.setFlag( Tile::tlRock, true );

    if( signTile.overlay().isValid() )
    {
      signTile.overlay()->deleteLater();
      signTile.setOverlay( 0 );
    }
  }
}

void GameLoader::Impl::initWaterTileAnimation( Tilemap& tmap )
{
  TilesArray area = tmap.getArea( TilePos( 0, 0 ), Size( tmap.size() ) );

  Animation water;
  water.setDelay( 12 );
  water.load( ResourceGroup::land1a, 121, 7 );
  water.load( ResourceGroup::land1a, 127, 7, true );
  foreach( tile, area )
  {
    int rId = (*tile)->originalImgId() - 364;
    if( rId >= 0 && rId < 8 )
    {
      water.setIndex( rId );
      (*tile)->setAnimation( water );
      (*tile)->setFlag( Tile::tlDeepWater, true );
    }
  }
}

void GameLoader::Impl::finalize( Game& game )
{
  Tilemap& tileMap = game.city()->tilemap();

  // exit and entry can't point to one tile or .... can!
  const BorderInfo& border = game.city()->borderInfo();

  initEntryExitTile( border.roadEntry, tileMap, 89, false );
  initEntryExitTile( border.roadExit,  tileMap, 85, true  );

  initWaterTileAnimation( tileMap );
}

void GameLoader::Impl::initLoaders()
{
  loaders.push_back( GameAbstractLoaderPtr( new GameLoaderC3Map() ) );
  loaders.push_back( GameAbstractLoaderPtr( new GameLoaderC3Sav() ) );
  loaders.push_back( GameAbstractLoaderPtr( new GameLoaderOc3() ) );
  loaders.push_back( GameAbstractLoaderPtr( new GameLoaderMission() ) );
}

bool GameLoader::load( vfs::Path filename, Game& game )
{
  // try to load file based on file extension
  Impl::LoaderIterator it = _d->loaders.begin();
  for( ; it != _d->loaders.end(); ++it)
  {
    if( (*it)->isLoadableFileExtension( filename.toString() ) /*||
        (*it)->isLoadableFileFormat(file) */ )
    {
			VariantMap vm;
			ClimateType currentClimate;
			if (typeid(*(*it).object()) != typeid(GameLoaderOc3))
			{
				currentClimate = (ClimateType)(*it)->climateType(filename.toString());
			}
			else
			{
				vm = SaveAdapter::load(filename);
				currentClimate = (ClimateType)((GameLoaderOc3*)(*it).object())->climateType(vm);
			}
      if( currentClimate >= 0  )
      {
        ClimateManager::initialize( currentClimate );
      }

			bool loadok;
			if (typeid(*(*it).object()) != typeid(GameLoaderOc3))
			{
				 loadok = (*it)->load(filename.toString(), game);
			}
			else
			{
				loadok = ((GameLoaderOc3*)(*it).object())->load(vm, game);
			}
      
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

std::string GameLoader::restartFile() const { return _d->restartFile; }
