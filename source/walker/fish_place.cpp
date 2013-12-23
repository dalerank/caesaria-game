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

#include "fish_place.hpp"
#include "resourcegroup.hpp"
#include "city.hpp"
#include "gfx/tilemap.hpp"
#include "astarpathfinding.hpp"
#include "pathway.hpp"
#include "walker/walker.hpp"
#include "constants.hpp"

class FishPlace::Impl
{
public:
  Renderer::PassQueue passQueue;
  int fishCount;
  WalkerPtr walker;
  Point basicOffset;
  PicturesArray animations;

  void restoreTilePic( Tile& tile )
  {
    int picId = tile.getOriginalImgId();
    Picture* pic = &Picture::load( TileHelper::convId2PicName( picId ) );
    tile.setPicture( pic );
  }
};

FishPlace::FishPlace() : TileOverlay( constants::place::fishPlace ), _d( new Impl )
{
  setDebugName( "fishPlace" );

  setPicture( Picture::getInvalid() );

  _animationRef().setDelay( 3 );
  _d->animations.resize( 1 );
  _d->passQueue.push_back( Renderer::foreground );
  _d->passQueue.push_back( Renderer::animations );

  _d->fishCount = rand() % 100;

  if( _d->fishCount > 1 )
  {
    _animationRef().load( ResourceGroup::land3a, 19, 24); //big fish place
    _d->basicOffset = Point( -41, 122 );
    _animationRef().setOffset( _d->basicOffset );
  }
  else
  {
    _animationRef().load( ResourceGroup::land3a, 1, 18);
    _d->basicOffset =  Point( 0, 55 );
    _animationRef().setOffset( _d->basicOffset );
  } //small fish place
}

FishPlace::~FishPlace()
{

}

void FishPlace::build(PlayerCityPtr city, const TilePos& pos)
{
  setSize( Size( 0 ) );
  TileOverlay::build( city, pos );

  Tilemap &tilemap = city->getTilemap();
  Tile& tile = tilemap.at( pos );
  tile.setMasterTile( getTile().getMasterTile() );

  if( tile.getOverlay().isValid() && tile.getOverlay() != this )
  {
    tile.getOverlay()->deleteLater();
  }

  tile.setOverlay( this );

  _d->restoreTilePic( getTile() );
}

void FishPlace::initTerrain(Tile& terrain)
{

}

void FishPlace::timeStep(const unsigned long time)
{
  _animationRef().update( time );

  _d->animations[ 0 ] = _animationRef().getFrame();

  if( _d->walker != 0 )
  {
    TilePos lastPos = _d->walker->getIJ();
    _d->walker->timeStep( time );

    if( lastPos != _d->walker->getIJ() )
    {
      getTile().setOverlay( 0 );
      _d->restoreTilePic( getTile() );

      TilePos pos =  _d->walker->getIJ();
      build( _getCity(), pos );

      _animationRef().setDelay( 2 + rand() % 4 );
    }
    else if( lastPos == _d->walker->getPathway().getDestination().getIJ() )
    {
      deleteLater();
    }
  }
  else
  {
    Pathway pathway;
    bool pathFound = Pathfinder::getInstance().getPath( getTilePos(), _getCity()->getBorderInfo().boatExit,
                                                        pathway, Pathfinder::waterOnly );

    if( !pathFound )
    {
      deleteLater();
    }
    else
    {
      _d->walker = WalkerPtr( new Walker( _getCity() ) );
      _d->walker->drop();
      _d->walker->setSpeed( 0.1f );
      _d->walker->setIJ( getTilePos() );
      _d->walker->setPathway( pathway );
      _d->walker->go();
    }
  }
}

void FishPlace::destroy()
{
  getTile().setOverlay( 0 );

  _d->restoreTilePic( getTile() );

  TileOverlay::destroy();
}

void FishPlace::save(VariantMap& stream) const
{
  TileOverlay::save( stream );
  stream[ "fishCount" ] = _d->fishCount;
}

void FishPlace::load(const VariantMap& stream)
{
  TileOverlay::load( stream );
  _d->fishCount = stream.get( "fishCount" );
}

const PicturesArray& FishPlace::getPictures(Renderer::Pass pass) const
{
  switch(pass)
  {
  case Renderer::animations: return _d->animations;
  default: break;
  }

  return TileOverlay::getPictures( pass );
}

Renderer::PassQueue FishPlace::getPassQueue() const
{
  return _d->passQueue;
}
