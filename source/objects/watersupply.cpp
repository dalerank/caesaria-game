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

#include "watersupply.hpp"

#include "core/utils.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "objects/road.hpp"
#include "gfx/tile.hpp"
#include "core/variant_map.hpp"
#include "walker/serviceman.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "objects_factory.hpp"
#include "game/gamedate.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::reservoir, Reservoir)

class WaterSource::Impl
{
public:
  int  water;
  bool lastWaterState;
  int  daysWithoutWater;
  bool isRoad;
  std::string errorStr;
};

void Reservoir::_dropWater()
{
  //now remove water flag from near tiles
  Tilemap& tmap = _city()->tilemap();
  TilesArray reachedTiles = tmap.getArea( pos() - TilePos( 10, 10 ), Size( 10 + 10 ) + size() );

  foreach( tile, reachedTiles ) { (*tile)->setParam( Tile::pReservoirWater, 0 ); }
}

void Reservoir::_waterStateChanged()
{

}

void Reservoir::destroy()
{
  _dropWater();

  // update adjacent aqueducts
  Construction::destroy();
}

std::string Reservoir::troubleDesc() const
{
  return haveWater() ? "" : "##trouble_too_far_from_water##";
}

void Reservoir::addWater(const WaterSource& source)
{
  WaterSource::addWater( source );
}

Reservoir::Reservoir()
    : WaterSource( objects::reservoir, Size( 3 ) )
{  
  _isWaterSource = false;
  setPicture( ResourceGroup::utilitya, 34 );
  
  // utilitya 34      - empty reservoir
  // utilitya 35 ~ 42 - full reservoir animation
 
  _animationRef().load( ResourceGroup::utilitya, 35, 8);
  _animationRef().load( ResourceGroup::utilitya, 42, 7, Animation::reverse);
  _animationRef().setDelay( 11 );
  //_animationRef().setOffset( Point( 47, 63 ) );

  _fgPicturesRef().resize(1);
}

Reservoir::~Reservoir(){}

bool Reservoir::build( const CityAreaInfo& info )
{
  Construction::build( info );

  _isWaterSource = _isNearWater( info.city, info.pos );
  _setError( _isWaterSource ? "" : "##need_connect_to_other_reservoir##");

  return true;
}

bool Reservoir::_isNearWater(PlayerCityPtr city, const TilePos& pos ) const
{
  bool near_water = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = city->tilemap();
  TilesArray perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), size() + Size( 2 ), !Tilemap::checkCorners );

  foreach( tile, perimetr) { near_water |= (*tile)->getFlag( Tile::tlWater ); }

  return near_water;
}

void Reservoir::initTerrain(Tile &terrain) {}

void Reservoir::timeStep(const unsigned long time)
{
  WaterSource::timeStep( time );

  if( _isWaterSource )
  {
    _d->water = 100;
  }

  if( !_d->water )
  {
    _fgPicture( 0 ) = Picture::getInvalid();
    _dropWater();
    return;
  }

  //filled area, that reservoir present
  if( game::Date::isWeekChanged() )
  {
    Tilemap& tmap = _city()->tilemap();
    TilesArray reachedTiles = tmap.getArea( pos() - TilePos( 10, 10 ), Size( 10 + 10 ) + size() );

    foreach( tile, reachedTiles )
    {
      int value = (*tile)->param( Tile::pReservoirWater );
      (*tile)->setParam( Tile::pReservoirWater, math::clamp( value+1, 0, 20 ) );
    }
  }

  if( game::Date::isDayChanged() )
  {
    const TilePos offsets[4] = { TilePos( -1, 1), TilePos( 1, 3 ), TilePos( 3, 1), TilePos( 1, -1) };
    _produceWater(offsets, 4);
  }

  _animationRef().update( time );
  
  // takes current animation frame and put it into foreground
  _fgPicture( 0 ) = _animationRef().currentFrame();
}

bool Reservoir::canBuild( const CityAreaInfo& areaInfo ) const
{
  bool ret = Construction::canBuild( areaInfo );

  bool nearWater = _isNearWater( areaInfo.city, areaInfo.pos );
  Reservoir* thisp = const_cast< Reservoir* >( this );
  thisp->_fgPicturesRef().clear();
  if( nearWater )
  {
    thisp->_fgPicturesRef().push_back( Picture::load( ResourceGroup::utilitya, 35 )  );
    thisp->_fgPicturesRef().back().setOffset( 47, -10+picture().offset().y() );
  }
  return ret;
}

bool Reservoir::isNeedRoadAccess() const{  return false; }

WaterSource::WaterSource(const Type type, const Size& size )
  : Construction( type, size ), _d( new Impl )

{
  _d->water = 0;
  _d->lastWaterState = false;

  setState( Building::inflammability, 0 );
  setState( Building::collapsibility, 0 );
}

WaterSource::~WaterSource(){}

void WaterSource::addWater( const WaterSource& source )
{
  if( source.water() > water()+1 )
  {
    _d->water = source.water() - 1;
    _d->daysWithoutWater = 0;
  }
}

bool WaterSource::haveWater() const{  return _d->water > 0;} 

void WaterSource::timeStep( const unsigned long time )
{
  if( game::Date::isDayChanged() )
  {  
    _d->daysWithoutWater++;
    if( _d->daysWithoutWater > 5 )
    {
      _d->water = math::clamp( _d->water-10, 0, 100 );
    }

    if( _d->lastWaterState != (_d->water > 0) )
    {
      _d->lastWaterState = _d->water > 0;
      _waterStateChanged();
    }
  }

  Construction::timeStep( time );
}

void WaterSource::_produceWater(const TilePos* points, const int size)
{
  Tilemap& tilemap = _city()->tilemap();

  for( int index=0; index < size; index++ )
  {
    TilePos p = pos() + points[index];
    if( tilemap.isInside( p ) )
    {
      SmartPtr< WaterSource > ws = ptr_cast<WaterSource>( tilemap.at( p ).overlay() );
    
      if( ws.isValid() )
      {
        if( ws->water() < water() )
            ws->addWater( *this );
      }
    }
  }
}

void WaterSource::_setIsRoad(bool value){  _d->isRoad = value;}
bool WaterSource::_isRoad() const { return _d->isRoad; }
int WaterSource::water() const{ return _d->water; }
std::string WaterSource::errorDesc() const{  return _d->errorStr;}
void WaterSource::_setError(const std::string& error){  _d->errorStr = error;}

void WaterSource::save(VariantMap &stream) const
{
  Construction::save( stream );
  stream[ "water" ] = _d->water;
  stream[ "isRoad" ] = _d->isRoad;
}

void WaterSource::load(const VariantMap &stream)
{
  Construction::load( stream );
  _d->water = stream.get( "water" );
  _d->isRoad = stream.get( "isRoad" );
  _d->daysWithoutWater = 0;
}


TilePos Reservoir::entry(constants::Direction direction)
{
  switch( direction )
  {
  case north: return pos() + TilePos( 1, 2 );
  case east: return pos() + TilePos( 2, 1 );
  case south: return pos() + TilePos( 1, 0 );
  case west: return pos() + TilePos( 0, 1 );
  default: return TilePos( -1, -1 );
  }
}
