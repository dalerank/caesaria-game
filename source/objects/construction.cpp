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

#include "construction.hpp"

#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "game/city.hpp"
#include "events/disaster.hpp"
#include "core/logger.hpp"

class Construction::Impl
{
public:
  typedef std::map<Construction::Param, double> Params;
  TilesArray accessRoads;
  Params params;
};

Construction::Construction(const Type type, const Size& size)
  : TileOverlay( type, size ), _d( new Impl )
{
  _d->params[ fire ] = 0;
  _d->params[ damage ] = 0;
}

Construction::~Construction()
{

}

bool Construction::canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& ) const
{
  Tilemap& tilemap = city->getTilemap();

  bool is_constructible = true;

  //return area for available tiles
  TilesArray area = tilemap.getArea( pos, getSize() );

  //on over map size
  if( (int)area.size() != getSize().getArea() )
    return false;

  foreach( Tile* tile, area )
  {
     is_constructible &= tile->getFlag( Tile::isConstructible );
  }

  return is_constructible;
}

std::string Construction::getError() const { return ""; }

void Construction::build(PlayerCityPtr city, const TilePos& pos )
{
  TileOverlay::build( city, pos );

  computeAccessRoads();
}

TilesArray Construction::getAccessRoads() const
{
   return _d->accessRoads;
}

// here the problem lays: if we remove road, it is left in _accessRoads array
// also we need to recompute _accessRoads if we place new road tile
// on next to this road tile buildings
void Construction::computeAccessRoads()
{
  _d->accessRoads.clear();
  if( !_getMasterTile() )
      return;

  Tilemap& tilemap = _getCity()->getTilemap();

  int s = getSize().getWidth();
  for( int dst=1; dst <= getRoadAccessDistance(); dst++ )
  {
    TilesArray rect = tilemap.getRectangle( getTilePos() + TilePos( -dst, -dst ),
                                            getTilePos() + TilePos( s+dst-1, s+dst-1 ),
                                            !Tilemap::checkCorners );
    foreach( Tile* tile, rect )
    {
      if( tile->getFlag( Tile::tlRoad ) )
      {
        _d->accessRoads.push_back( tile );
      }
    }
  }
}

int Construction::getRoadAccessDistance() const
{
  return 1;
  // it is default value
  // for houses - 2
}

void Construction::burn()
{
  deleteLater();

  events::GameEventPtr event = events::DisasterEvent::create( getTile().getIJ(), events::DisasterEvent::fire );
  event->dispatch();
}

void Construction::collapse()
{
  deleteLater();
  events::GameEventPtr event = events::DisasterEvent::create( getTile().getIJ(), events::DisasterEvent::collapse );
  event->dispatch();
}

Desirability Construction::getDesirability() const
{
  return MetaDataHolder::instance().getData( getType() ).getDesirbility();
}

void Construction::destroy()
{
  TileOverlay::destroy();
}

void Construction::updateState(Construction::Param param, double value, bool relative)
{
  if( relative ) _d->params[ param ] += value;
  else           _d->params[ param ] = value;

  _d->params[ param ] = math::clamp<double>( _d->params[ param ], 0.f, 100.f );
}

void Construction::save( VariantMap& stream) const
{
    TileOverlay::save( stream );
    stream[ Serializable::fireLevel ] = getState( fire );
    stream[ Serializable::damageLevel ] = getState( damage );

//    stream.write_int(_traineeMap.size(), 1, 0, WTT_MAX);
//    for (std::map<WalkerTraineeType, int>::iterator itLevel = _traineeMap.begin(); itLevel != _traineeMap.end(); ++itLevel)
//    {
//       WalkerTraineeType traineeType = itLevel->first;
//       int traineeLevel = itLevel->second;
//       stream.write_int((int)traineeType, 1, 0, WTT_MAX);
//       stream.write_int(traineeLevel, 1, 0, 200);
//    }
//
//    stream.write_int(_reservedTrainees.size(), 1, 0, WTT_MAX);
//    for (std::set<WalkerTraineeType>::iterator itReservation = _reservedTrainees.begin(); itReservation != _reservedTrainees.end(); ++itReservation)
//    {
//       WalkerTraineeType traineeType = *itReservation;
//       stream.write_int((int)traineeType, 1, 0, WTT_MAX);
//    }
}

void Construction::load( const VariantMap& stream )
{
  TileOverlay::load( stream );
  _d->params[ fire ] = (float)stream.get( Serializable::fireLevel, 0.f );
  _d->params[ damage ] = (float)stream.get( Serializable::damageLevel, 0.f );
//    Construction::unserialize(stream);
//    _damageLevel = (float)stream.read_int(1, 0, 100);
//    _fireLevel = (float)stream.read_int(1, 0, 100);
//
//    int size = stream.read_int(1, 0, WTT_MAX);
//    for (int i=0; i<size; ++i)
//    {
//       WalkerTraineeType traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
//       int traineeLevel = stream.read_int(1, 0, 200);
//       _traineeMap[traineeType] = traineeLevel;
//    }
//
//    size = stream.read_int(1, 0, WTT_MAX);
//    for (int i=0; i<size; ++i)
//    {
//       WalkerTraineeType traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
//       _reservedTrainees.insert(traineeType);
//    }
}

double Construction::getState(Construction::Param param) const
{
  return _d->params[ param ];
}

TilesArray Construction::getEnterArea() const
{
  TilesArray tiles;

  int s = getSize().getWidth();
  const TilesArray& near = _getCity()->getTilemap().getRectangle( getTilePos() - TilePos(1, 1),
                                                                  getTilePos() + TilePos(s, s),
                                                                  !Tilemap::checkCorners );

  for( TilesArray::const_iterator it=near.begin(); it != near.end(); it++)
  {
    if( (*it)->isWalkable( true ) )
    {
      tiles.push_back( *it );
    }
  }

  return tiles;
}

void Construction::timeStep(const unsigned long time)
{
  if( getState( Construction::damage ) >= 100 )
  {
    Logger::warning( "Building destroyed!" );
    collapse();
  }
  else if( getState( Construction::fire ) >= 100 )
  {
    Logger::warning( "Building catch fire!" );
    burn();
  }

  TileOverlay::timeStep( time );
}

const Picture& Construction::getPicture() const
{
  return TileOverlay::getPicture();
}

const Picture& Construction::getPicture(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  return getPicture();
}

bool Construction::isNeedRoadAccess() const
{
  return true;
}
