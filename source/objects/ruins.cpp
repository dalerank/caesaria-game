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

#include "ruins.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "walker/serviceman.hpp"
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/city.hpp"
#include "events/event.hpp"
#include "constants.hpp"
#include "core/foreach.hpp"

using namespace constants;

BurningRuins::BurningRuins() : Ruins( building::burningRuins )
{
  updateState( Construction::fire, 99, false );

  setPicture( ResourceGroup::land2a, 187 );
  _animationRef().load( ResourceGroup::land2a, 188, 8 );
  _animationRef().setOffset( Point( 14, 26 ) );
  _fgPicturesRef().resize(1);
}

void BurningRuins::timeStep(const unsigned long time)
{
  Building::timeStep(time);

  if (time % 16 == 0 )
  {
    if( getState( Construction::fire ) > 0 )
    {
      updateState( Construction::fire, -1 );
      if( getState( Construction::fire ) == 50 )
      {
        setPicture( ResourceGroup::land2a, 214 );
        _animationRef().clear();
        _animationRef().load( ResourceGroup::land2a, 215, 8);
        _animationRef().setOffset( Point( 14, 26 ) );
      }
      else if( getState( Construction::fire ) == 25 )
      {
        setPicture( ResourceGroup::land2a, 223 );
        _animationRef().clear();
        _animationRef().load(ResourceGroup::land2a, 224, 8);
        _animationRef().setOffset( Point( 14, 18 ) );
      }

      Tilemap& tmap = _getCity()->getTilemap();
      for( int range=1; range < 3; range++ )
      {
        TilePos offset( range, range );
        TilesArray tiles = tmap.getRectangle( getTilePos() - offset, getTilePos() + offset );

        foreach( tile, tiles)
        {
          BuildingPtr b = (*tile)->getOverlay().as<Building>();
          if( b.isValid() && b->getClass() != building::disasterGroup )
          {
            b->updateState( Construction::fire, 0.5 );
          }
        }
      }
    }
    else
    {
      deleteLater();
      _animationRef().clear();
      _fgPicturesRef().clear();
    }        
  }
}

void BurningRuins::destroy()
{
  Building::destroy();

  BurnedRuinsPtr p( new BurnedRuins() );
  p->drop();
  p->setInfo( getInfo() );

  events::GameEventPtr event = events::BuildEvent::create( getTilePos(), p.as<TileOverlay>() );
  event->dispatch();
}

int BurningRuins::getMaxWorkers() const
{
  return 0;
}

void BurningRuins::burn()
{

}

void BurningRuins::build(PlayerCityPtr city, const TilePos& pos )
{
  Building::build( city, pos );
  //while burning can't remove it
  getTile().setFlag( Tile::tlTree, false );
  getTile().setFlag( Tile::tlRoad, false );
  getTile().setFlag( Tile::tlRock, true );
}   

bool BurningRuins::isWalkable() const
{
  return (getState( Construction::fire ) == 0);
}

float BurningRuins::evaluateService( ServiceWalkerPtr walker )
{
  if ( Service::prefect == walker->getService() )
  {
    return getState( Construction::fire );
  }

  return 0;
}

void BurningRuins::applyService(ServiceWalkerPtr walker)
{
  if ( Service::prefect == walker->getService() )
  {
    double newValue = math::clamp<float>( getState( Construction::fire ) - walker->getServiceValue() / 10, 0.f, 100.f );
    updateState( Construction::fire, newValue, false );
  }
}

bool BurningRuins::isNeedRoadAccess() const
{
  return false;
}

void BurnedRuins::timeStep( const unsigned long time )
{

}

BurnedRuins::BurnedRuins() : Ruins( building::burnedRuins )
{
  setPicture( ResourceGroup::land2a, 111 + rand() % 8 );
}

void BurnedRuins::build(PlayerCityPtr city, const TilePos& pos )
{
  Building::build( city, pos);

  getTile().setFlag( Tile::tlRock, false );
}

bool BurnedRuins::isWalkable() const
{
  return true;
}

bool BurnedRuins::isFlat() const
{
  return true;
}

bool BurnedRuins::isNeedRoadAccess() const
{
  return false;
}

void BurnedRuins::destroy()
{
  Building::destroy();
}

CollapsedRuins::CollapsedRuins() : Ruins(building::collapsedRuins)
{
  updateState( Construction::damage, 1, false );

  _animationRef().load( ResourceGroup::sprites, 1, 8 );
  _animationRef().setOffset( Point( 14, 26 ) );
  _animationRef().setDelay( 4 );
  _animationRef().setLoop( false );
  _fgPicturesRef().resize(1);
}

void CollapsedRuins::burn()
{

}

void CollapsedRuins::build(PlayerCityPtr city, const TilePos& pos )
{
  Building::build( city, pos );
  //while burning can't remove it
  getTile().setFlag( Tile::tlTree, false );
  getTile().setFlag( Tile::tlRoad, false );
  setPicture( ResourceGroup::land2a, 111 + rand() % 8  );
}

bool CollapsedRuins::isWalkable() const
{
  return true;
}

bool CollapsedRuins::isNeedRoadAccess() const
{
  return false;
}


PlagueRuins::PlagueRuins() : Ruins( building::plagueRuins )
{
  updateState( Construction::fire, 99, false );

  setPicture( ResourceGroup::land2a, 187 );
  _animationRef().load( ResourceGroup::land2a, 188, 8 );
  _animationRef().setOffset( Point( 14, 26 ) );
  _fgPicturesRef().resize(2);
  _fgPicturesRef().at( 1 ) = Picture::load( ResourceGroup::sprites, 218 );
  _fgPicturesRef().at( 1 ).setOffset( Point( 16, 32 ) );
}

void PlagueRuins::timeStep(const unsigned long time)
{
  _animationRef().update( time );
  _fgPicturesRef().at( 0 ) = _animationRef().getFrame();

  if (time % 16 == 0 )
  {
    if( getState( Construction::fire ) > 0 )
    {
      updateState( Construction::fire, -1 );
      if( getState( Construction::fire ) == 50 )
      {
        setPicture( ResourceGroup::land2a, 214 );
        _animationRef().clear();
        _animationRef().load( ResourceGroup::land2a, 215, 8);
        _animationRef().setOffset( Point( 14, 26 ) );
      }
      else if( getState( Construction::fire ) == 25 )
      {
        setPicture( ResourceGroup::land2a, 223 );
        _animationRef().clear();
        _animationRef().load(ResourceGroup::land2a, 224, 8);
        _animationRef().setOffset( Point( 14, 18 ) );
      }
    }
    else
    {
      deleteLater();
      _animationRef().clear();
      _fgPicturesRef().clear();
    }
  }
}

void PlagueRuins::destroy()
{
  Building::destroy();

  BurnedRuinsPtr p( new BurnedRuins() );
  p->drop();
  p->setInfo( getInfo() );

  events::GameEventPtr event = events::BuildEvent::create( getTilePos(), p.as<TileOverlay>() );
  event->dispatch();
}

void PlagueRuins::applyService(ServiceWalkerPtr walker)
{

}

void PlagueRuins::burn()
{

}

void PlagueRuins::build(PlayerCityPtr city, const TilePos& pos )
{
  Building::build( city, pos );
  //while burning can't remove it
  getTile().setFlag( Tile::tlTree, false );
  getTile().setFlag( Tile::tlRoad, false );
  getTile().setFlag( Tile::tlRock, true );
}

bool PlagueRuins::isWalkable() const
{
  return (getState( Construction::fire ) == 0);
}

bool PlagueRuins::isNeedRoadAccess() const
{
  return false;
}


Ruins::Ruins(building::Type type)
  : Building( type, Size(1) )
{

}

void Ruins::save(VariantMap& stream) const
{
  Building::save( stream );

  stream[ "text" ] = Variant( _parent );
}

void Ruins::load(const VariantMap& stream)
{
  Building::load( stream );

  _parent = stream.get( "text" ).toString();
}
