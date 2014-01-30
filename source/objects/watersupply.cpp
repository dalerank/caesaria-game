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

#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "objects/road.hpp"
#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"

using namespace constants;

class WaterSource::Impl
{
public:
  typedef std::map< int, int > WaterSourceMap;
  WaterSourceMap sourcesMap;
  WaterSourceMap consumersMap;

  int  water;
  bool lastWaterState;
  bool isRoad;
  bool alsoResolved;
  std::string errorStr;
};

void Reservoir::destroy()
{
  //now remove water flag from near tiles
  Tilemap& tmap = _getCity()->getTilemap();
  TilesArray reachedTiles = tmap.getArea( getTilePos() - TilePos( 10, 10 ), Size( 10 + 10 ) + getSize() );

  foreach( tile, reachedTiles ) { (*tile)->decreaseWaterService( WTR_RESERVOIR ); }

  // update adjacent aqueducts
  Construction::destroy();
}

Reservoir::Reservoir() : WaterSource( building::reservoir, Size( 3 ) )
{
  setPicture( ResourceGroup::waterbuildings, 1 );
  
  // utilitya 34      - empty reservoir
  // utilitya 35 ~ 42 - full reservoir animation
 
  _animationRef().load( ResourceGroup::utilitya, 35, 8);
  _animationRef().load( ResourceGroup::utilitya, 42, 7, Animation::reverse);
  _animationRef().setDelay( 11 );
  _animationRef().setOffset( Point( 47, 63 ) );

  _fgPicturesRef().resize(1);
  //_fgPictures[0]=;
}

Reservoir::~Reservoir()
{
}

void Reservoir::build(PlayerCityPtr city, const TilePos& pos )
{
  Construction::build( city, pos );

  setPicture( ResourceGroup::waterbuildings, 1 );
  _isWaterSource = _isNearWater( city, pos );
  
  //updateAqueducts();
  
  // update adjacent aqueducts
}

bool Reservoir::_isNearWater(PlayerCityPtr city, const TilePos& pos ) const
{
  bool near_water = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = city->getTilemap();
  TilesArray perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), !Tilemap::checkCorners );

  foreach( tile, perimetr) { near_water |= (*tile)->getFlag( Tile::tlWater ); }

  return near_water;
}

void Reservoir::initTerrain(Tile &terrain)
{
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlMeadow, isMeadow);
}

void Reservoir::timeStep(const unsigned long time)
{
  WaterSource::timeStep( time );

  if( _isWaterSource )
  {
    _d->water = 16;
  }

  if( !_d->water )
  {
    _fgPicturesRef()[ 0 ] = Picture::getInvalid();
    return;
  }

  //filled area, that reservoir present
  if( time % 22 == 1 )
  {
    Tilemap& tmap = _getCity()->getTilemap();
    TilesArray reachedTiles = tmap.getArea( getTilePos() - TilePos( 10, 10 ), Size( 10 + 10 ) + getSize() );

    foreach( tile, reachedTiles ) { (*tile)->fillWaterService( WTR_RESERVOIR ); }
  }

  //add water to all consumer
  if( time % 11 == 1 )
  {
    const TilePos offsets[4] = { TilePos( -1, 1), TilePos( 1, 3 ), TilePos( 3, 1), TilePos( 1, -1) };  
    _produceWater(offsets, 4);
  }

  _animationRef().update( time );
  
  // takes current animation frame and put it into foreground
  _fgPicturesRef()[ 0 ] = _animationRef().getFrame();
}

bool Reservoir::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  bool ret = Construction::canBuild( city, pos, aroundTiles );

  bool nearWater = _isNearWater( city, pos );
  const_cast< Reservoir* >( this )->setPicture( ResourceGroup::waterbuildings, nearWater ? 2 : 1  );

  const_cast< Reservoir* >( this )->_setError( nearWater ? "" : _("##need_connect_to_other_reservoir##"));

  return ret;
}

bool Reservoir::isNeedRoadAccess() const
{
  return false;
}

WaterSource::WaterSource(const Type type, const Size& size )
  : Construction( type, size ), _d( new Impl )

{
  _d->water = 0;
  _d->lastWaterState = false;
}

WaterSource::~WaterSource()
{
}

void WaterSource::addWater( const WaterSource& source )
{
  _d->water = math::clamp( _d->water+1, 0, 16 );
  int sourceId = source.getId();
  _d->sourcesMap[ sourceId ] = math::clamp( _d->sourcesMap[ sourceId ]+1, 0, 4 );
}

bool WaterSource::haveWater() const
{
  return _d->water > 0;
} 

void WaterSource::timeStep( const unsigned long time )
{
  if( time % 22 == 1)
  {
    _d->water = math::clamp( _d->water-1, 0, 16 );
    if( _d->lastWaterState != (_d->water > 0) )
    {
      _d->lastWaterState = _d->water > 0;
      _waterStateChanged();
    }

    foreach( item, _d->sourcesMap ) { item->second = math::clamp( item->second-1, 0, 4 ); }
  }

  Construction::timeStep( time );
}

void WaterSource::_produceWater( const TilePos* points, const int size )
{
  Tilemap& tilemap = _getCity()->getTilemap();

  for( int index=0; index < size; index++ )
  {
    TilePos pos = getTilePos() + points[index];
    if( !tilemap.isInside( pos ) )
    {
      continue;
    }

    SmartPtr< WaterSource > ws = ptr_cast<WaterSource>( tilemap.at( pos ).getOverlay() );
    
    if( ws.isValid() )
    {     
      if( _d->sourcesMap[ ws->getId() ] == 0 )
      {
        ws->addWater( *this );
      }
    }
  }
}

void WaterSource::_setIsRoad(bool value){  _d->isRoad = value;}
void WaterSource::_setResolved(bool value){  _d->alsoResolved = value;}
bool WaterSource::_isResolved() const { return _d->alsoResolved; }
int WaterSource::_getWater() const{  return _d->water;}
bool WaterSource::_isRoad() const { return _d->isRoad; }
int WaterSource::getId() const{  return getTilePos().j() * 10000 + getTilePos().i();}
std::string WaterSource::getError() const{  return _d->errorStr;}
void WaterSource::_setError(const std::string& error){  _d->errorStr = error;}

typedef enum { fontainEmpty = 3, fontainFull = 4, fontainStartAnim = 11, fontainSizeAnim = 7 } FontainConstant;

Fountain::Fountain() : ServiceBuilding(Service::fontain, building::fountain, Size(1))
{  
  //std::srand( DateTime::getElapsedTime() );

  //setPicture( ResourceGroup::utilitya, 10 );

  _waterIncreaseInterval = GameDate::getTickInMonth() / 3;
  _initAnimation();
  _fgPicturesRef().resize(1);

  _damageIncrement = 0;
  _fireIncrement = 0;

  setWorkers( 1 );
}

void Fountain::deliverService()
{
  ServiceWalkerPtr walker = ServiceWalker::create( _getCity(), getService() );
  walker->setBase( BuildingPtr( this ) );
  walker->setReachDistance( 4 );
  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( getTile().getIJ() );

  foreach( b, reachedBuildings ) { (*b)->applyService( walker ); }
}

void Fountain::timeStep(const unsigned long time)
{
  //filled area, that fontain present and work
  if( time % _waterIncreaseInterval == 1 )
  {
    if( getTile().getWaterService( WTR_RESERVOIR ) > 0 /*&& getWorkersCount() > 0*/ )
    {
      _haveReservoirWater = true;
      _animationRef().start();
    }
    else
    {
      //remove fontain service from tiles
      _haveReservoirWater = false;
      _animationRef().stop();
    }

    if( !isActive() )
    {
      _fgPicturesRef().front() = Picture::getInvalid();
      return;
    }

    Tilemap& tmap = _getCity()->getTilemap();
    TilesArray reachedTiles = tmap.getArea( getTilePos() - TilePos( 4, 4 ), Size( 4 + 4 ) + getSize() );

    foreach( tile, reachedTiles ) { (*tile)->fillWaterService( WTR_FONTAIN ); }
  }

  ServiceBuilding::timeStep( time );
}

bool Fountain::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  bool ret = Construction::canBuild( city, pos, aroundTiles );

  Tilemap& tmap = city->getTilemap();
  const Tile& tile = tmap.at( pos );
  int picid = (tile.getWaterService( WTR_RESERVOIR ) > 0 ? fontainFull : fontainEmpty );
  const_cast< Fountain* >( this )->setPicture( ResourceGroup::waterbuildings, picid );

  return ret;
}

void Fountain::build(PlayerCityPtr city, const TilePos& pos )
{
  ServiceBuilding::build( city, pos );

  setPicture( ResourceGroup::waterbuildings, fontainEmpty );
}

bool Fountain::isNeedRoadAccess() const {  return false; }
bool Fountain::isActive() const {  return ServiceBuilding::isActive() && _haveReservoirWater; }

bool Fountain::haveReservoirAccess() const
{
  TilesArray reachedTiles = _getCity()->getTilemap().getArea( getTilePos() - TilePos( 10, 10 ), Size( 10, 10 ) + getSize() );
  foreach( tile, reachedTiles )
  {
    TileOverlayPtr overlay = (*tile)->getOverlay();
    if( overlay != 0 && (building::reservoir == overlay->getType()) )
    {
      return true;
    }
  }

  return false;
}

void Fountain::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );

  //check animation
  timeStep( 1 );
}

void Fountain::_initAnimation()
{
  _animationRef().load( ResourceGroup::utilitya, fontainStartAnim, fontainSizeAnim );
  //animLoader.fill_animation_reverse(_animation, "utilitya", 25, 7);
  _animationRef().setOffset( Point( 12, 24 ) );
  _animationRef().setDelay( 2 );
  _animationRef().stop();
}
