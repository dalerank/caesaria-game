// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "watersupply.hpp"

#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "game/road.hpp"
#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "game/city.hpp"
#include "core/foreach.hpp"
#include "core/gettext.hpp"
#include "game/tilemap.hpp"
#include "core/logger.hpp"
#include "constants.hpp"

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

Aqueduct::Aqueduct() : WaterSource( building::aqueduct, Size(1) )
{
  setPicture( ResourceGroup::aqueduct, 133 ); // default picture for aqueduct
  _d->isRoad = false;
  _d->alsoResolved = false;
  // land2a 119 120         - aqueduct over road
  // land2a 121 122         - aqueduct over plain ground
  // land2a 123 124 125 126 - aqueduct corner
  // land2a 127 128         - aqueduct over dirty roads
  // land2a 129 130 131 132 - aqueduct T-shape crossing
  // land2a 133             - aqueduct crossing
  // land2a 134 - 148       - aqueduct without water
}

void Aqueduct::build( CityPtr city, const TilePos& pos )
{
  Tilemap& tilemap = city->getTilemap();
  Tile& terrain = tilemap.at( pos );

  // we can't build if already have aqueduct here
  SmartPtr<Aqueduct> aqueveduct = terrain.getOverlay().as<Aqueduct>();
  if( aqueveduct.isValid() )
  {
    return;
  }

  Construction::build( city, pos );

  CityHelper helper( city );
  AqueductList aqueducts = helper.find<Aqueduct>( building::aqueduct );
  foreach( AqueductPtr aqueduct, aqueducts )
  {
    aqueduct->updatePicture( city );
  }

  updatePicture( city );
}

void Aqueduct::destroy()
{
  Construction::destroy();

  if( _getCity().isValid() )
  {
    TilemapArea area = _getCity()->getTilemap().getArea( getTilePos() - TilePos( 2, 2), Size( 5 ) );
    foreach( Tile* tile, area )
    {
      TileOverlayPtr overlay = tile->getOverlay();
      if( overlay.isValid() && overlay->getType() == building::aqueduct )
      {
        overlay.as<Aqueduct>()->updatePicture( _getCity() );
      }
    }
  }
}

void Aqueduct::initTerrain(Tile &terrain)
{
  bool isRoad   = terrain.getFlag( Tile::tlRoad );
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );

  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlBuilding, true);
  terrain.setFlag( Tile::tlRoad, isRoad );
  terrain.setFlag( Tile::tlMeadow, isMeadow);
  terrain.setFlag( Tile::tlAqueduct, true); // mandatory!
}

bool Aqueduct::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool is_free = Construction::canBuild( city, pos );

  if( is_free )
      return true; // we try to build on free tile

  // we can place on road
  Tilemap& tilemap = city->getTilemap();
  Tile& terrain = tilemap.at( pos );

  // we can't build on plazas
  if( terrain.getOverlay().as<Plaza>().isValid() )
      return false;

  // we can show that won't build over other aqueduct
  if( terrain.getOverlay().as<Aqueduct>().isValid() )
      return false;

  // also we can't build if next tile is road + aqueduct
  if ( terrain.getFlag( Tile::tlRoad ) )
  {
    TilePos tp_from = pos + TilePos (-1, -1);
    TilePos tp_to = pos + TilePos (1, 1);

    if (!tilemap.isInside(tp_from))
      tp_from = pos;

    if (!tilemap.isInside(tp_to))
      tp_to = pos;

    TilemapTiles perimetr = tilemap.getRectangle(tp_from, tp_to, !Tilemap::checkCorners);
    foreach( Tile* tile, perimetr )
    {
      if( tile->getFlag( Tile::tlRoad ) && tile->getFlag( Tile::tlAqueduct ) )
        return false;
    }
  }

  // and we can't build on intersections
  if ( terrain.getFlag( Tile::tlRoad ) )
  {
    int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

    TilePos tile_pos_d[countDirection];
    bool is_border[countDirection];

    tile_pos_d[north] = pos + TilePos(  0,  1);
    tile_pos_d[east]  = pos + TilePos(  1,  0);
    tile_pos_d[south] = pos + TilePos(  0, -1);
    tile_pos_d[west]  = pos + TilePos( -1,  0);

    // all tiles must be in map range
    for (int i = 0; i < countDirection; ++i) {
      is_border[i] = !tilemap.isInside(tile_pos_d[i]);
      if (is_border[i])
        tile_pos_d[i] = pos;
    }

    if (tilemap.at(tile_pos_d[north]).getFlag( Tile::tlRoad )) { directionFlags += 1; } // road to the north
    if (tilemap.at(tile_pos_d[east]).getFlag( Tile::tlRoad )) { directionFlags += 2; } // road to the east
    if (tilemap.at(tile_pos_d[south]).getFlag( Tile::tlRoad )) { directionFlags += 4; } // road to the south
    if (tilemap.at(tile_pos_d[west]).getFlag( Tile::tlRoad )) { directionFlags += 8; } // road to the west

    Logger::warning( "direction flags=%d", directionFlags );

    switch (directionFlags)
    {
    case 0:  // no road!
    case 1:  // North
    case 2:  // East
    case 4:  // South
    case 8:  // West
    case 5:  // North+South
    case 10: // East+West
      return true;
    }
  }
  return false;
}

Picture& Aqueduct::computePicture(CityPtr city , const TilemapTiles * tmp, const TilePos pos)
{
  // find correct picture as for roads
  Tilemap& tmap = city->getTilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (tmp == NULL) ? getTilePos() : pos;

  if (!tmap.isInside(tile_pos))
    return Picture::load( ResourceGroup::aqueduct, 121 );

  TilePos tile_pos_d[countDirection];
  bool is_border[countDirection];
  bool is_busy[countDirection] = { false };

  tile_pos_d[north] = tile_pos + TilePos(  0,  1);
  tile_pos_d[east]  = tile_pos + TilePos(  1,  0);
  tile_pos_d[south] = tile_pos + TilePos(  0, -1);
  tile_pos_d[west]  = tile_pos + TilePos( -1,  0);

  // all tiles must be in map range
  for (int i = 0; i < countDirection; ++i) {
    is_border[i] = !tmap.isInside(tile_pos_d[i]);
    if (is_border[i])
      tile_pos_d[i] = tile_pos;
  }

  // get overlays for all directions
  TileOverlayPtr overlay_d[countDirection];
  overlay_d[north] = tmap.at( tile_pos_d[north] ).getOverlay();
  overlay_d[east] = tmap.at( tile_pos_d[east]  ).getOverlay();
  overlay_d[south] = tmap.at( tile_pos_d[south] ).getOverlay();
  overlay_d[west] = tmap.at( tile_pos_d[west]  ).getOverlay();

  // if we have a TMP array with aqueducts, calculate them
  if (tmp != NULL)
  {
    for (TilemapTiles::const_iterator it = tmp->begin(); it != tmp->end(); ++it)
    {
      int i = (*it)->getI();
      int j = (*it)->getJ();

      if (i == pos.getI() && j == (pos.getJ() + 1)) is_busy[north] = true;
      else if (i == pos.getI() && j == (pos.getJ() - 1))is_busy[south] = true;
      else if (j == pos.getJ() && i == (pos.getI() + 1))is_busy[east] = true;
      else if (j == pos.getJ() && i == (pos.getI() - 1))is_busy[west] = true;
    }
  }

  // calculate directions
  for (int i = 0; i < countDirection; ++i) {
    if (!is_border[i] && (overlay_d[i].is<Aqueduct>() || overlay_d[i].is<Reservoir>() || is_busy[i]))
      switch (i) {
      case north: directionFlags += 1; break;
      case east:  directionFlags += 2; break;
      case south: directionFlags += 4; break;
      case west:  directionFlags += 8; break;
      default: break;
      }
  }

  int index;
  switch (directionFlags)
  {
  case 0:  // no neighbours!
    index = 121; break;
  case 1:  // N
  case 4:  // S
  case 5:  // N + S
    index = 121; 
    if( tmap.at( tile_pos ).getFlag( Tile::tlRoad ) )
    {
      index = 119; 
      _d->isRoad = true;
    }
    break;
    
  case 3:  // N + E
    index = 123; break;
  case 6:  // E + S
    index = 124; break;
  case 7:  // N + E + S
    index = 129; break;
  case 9:  // N + W
    index = 126; break;
  case 2:  // E
  case 8:  // W
  case 10: // E + W
    index = 122; 
    if( tmap.at( tile_pos ).getFlag( Tile::tlRoad ) )
    {
      index = 120; 
      _d->isRoad = true;
    }
    break;
   
  case 11: // N + E + W
    index = 132; break;
  case 12: // S + W
    index = 125;  break;
  case 13: // N + S + W
    index = 131; break;
  case 14: // E + S + W
    index = 130; break;
  case 15: // N + S + E + W (crossing)
    index = 133; break;
  default:
    index = 121; // it's impossible, but ...
  }

  return Picture::load( ResourceGroup::aqueduct, index + (_d->water == 0 ? 15 : 0) );
}

void Aqueduct::updatePicture( CityPtr city )
{
  setPicture( computePicture( city ) );
}

bool Aqueduct::isNeedRoadAccess() const
{
  return false;
}

void Aqueduct::_waterStateChanged()
{
  updatePicture( _getCity() );
}

void Aqueduct::addWater( const WaterSource& source )
{
  if( !_d->alsoResolved )
  {
    _d->alsoResolved = true;
    WaterSource::addWater( source );

    const TilePos offsets[4] = { TilePos( -1, 0 ), TilePos( 0, 1), TilePos( 1, 0), TilePos( 0, -1) };
    _produceWater( offsets, 4 );
    _d->alsoResolved = false;
  }
}

bool Aqueduct::isWalkable() const
{
  return _d->isRoad;
}

bool Aqueduct::isRoad() const
{
  return _d->isRoad;
}

void Reservoir::destroy()
{
  //now remove water flag from near tiles
  Tilemap& tmap = _getCity()->getTilemap();
  TilemapArea reachedTiles = tmap.getArea( getTilePos() - TilePos( 10, 10 ), Size( 10 + 10 ) + getSize() );
  foreach( Tile* tile, reachedTiles )
  {
    tile->decreaseWaterService( WTR_RESERVOIR );
  }


  // update adjacent aqueducts
  Construction::destroy();
}

Reservoir::Reservoir() : WaterSource( building::reservoir, Size( 3 ) )
{
  setPicture( ResourceGroup::waterbuildings, 1 );
  
  // utilitya 34      - empty reservoir
  // utilitya 35 ~ 42 - full reservoir animation
 
  _getAnimation().load( ResourceGroup::utilitya, 35, 8);
  _getAnimation().load( ResourceGroup::utilitya, 42, 7, Animation::reverse);
  _getAnimation().setDelay( 11 );
  _getAnimation().setOffset( Point( 47, 63 ) );

  _getFgPictures().resize(1);
  //_fgPictures[0]=;
}

Reservoir::~Reservoir()
{
}

void Reservoir::build( CityPtr city, const TilePos& pos )
{
  Construction::build( city, pos );

  setPicture( ResourceGroup::waterbuildings, 1 );
  _isWaterSource = _isNearWater( city, pos );
  
  //updateAqueducts();
  
  // update adjacent aqueducts
}

bool Reservoir::_isNearWater(CityPtr city, const TilePos& pos ) const
{
  bool near_water = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = city->getTilemap();
  TilemapTiles perimetr = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), !Tilemap::checkCorners );
  foreach( Tile* tile, perimetr)
  {
    near_water |= tile->getFlag( Tile::tlWater );
  }

  return near_water;
}

void Reservoir::initTerrain(Tile &terrain)
{
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlBuilding, true);
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
    _getFgPictures().at( 0 ) = Picture::getInvalid();
    return;
  }

  //filled area, that reservoir present
  if( time % 22 == 1 )
  {
    Tilemap& tmap = _getCity()->getTilemap();
    TilemapArea reachedTiles = tmap.getArea( getTilePos() - TilePos( 10, 10 ), Size( 10 + 10 ) + getSize() );
    foreach( Tile* tile, reachedTiles )
    {
      tile->fillWaterService( WTR_RESERVOIR );
    }   
  }

  //add water to all consumer
  if( time % 11 == 1 )
  {
    const TilePos offsets[4] = { TilePos( -1, 1), TilePos( 1, 3 ), TilePos( 3, 1), TilePos( 1, -1) };  
    _produceWater(offsets, 4);
  }

  _getAnimation().update( time );
  
  // takes current animation frame and put it into foreground
  _getFgPictures().at( 0 ) = _getAnimation().getFrame();
}

bool Reservoir::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool ret = Construction::canBuild( city, pos );

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

    foreach( Impl::WaterSourceMap::value_type& item, _d->sourcesMap )
    {
      item.second = math::clamp( item.second-1, 0, 4 );
    }
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

    SmartPtr< WaterSource > ws = tilemap.at( pos ).getOverlay().as<WaterSource>();
    
    if( ws.isValid() )
    {     
      if( _d->sourcesMap[ ws->getId() ] == 0 )
      {
        ws->addWater( *this );
      }
    }
  }
}

int WaterSource::getId() const
{
  return getTilePos().getJ() * 10000 + getTilePos().getI();
}

std::string WaterSource::getError() const
{
  return _d->errorStr;
}

void WaterSource::_setError(const std::string& error)
{
  _d->errorStr = error;
}

typedef enum { fontainEmpty = 3, fontainFull = 4, fontainStartAnim = 11, fontainSizeAnim = 7 } FontainConstant;

Fountain::Fountain() : ServiceBuilding(Service::fontain, building::fountain, Size(1))
{  
  //std::srand( DateTime::getElapsedTime() );

  //setPicture( ResourceGroup::utilitya, 10 );

  _initAnimation();
  _getFgPictures().resize(1);

  _damageIncrement = 0;
  _fireIncrement = 0;

  setWorkers( 1 );
}

void Fountain::deliverService()
{
  ServiceWalkerPtr walker = ServiceWalker::create( _getCity(), getService() );
  walker->setBase( BuildingPtr( this ) );
  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( getTile().getIJ() );

  foreach( BuildingPtr building, reachedBuildings )
  {
    building->applyService( walker );
  } 
}

void Fountain::timeStep(const unsigned long time)
{
  //filled area, that fontain present and work
  if( time % 22 == 1 )
  {
    if( getTile().getWaterService( WTR_RESERVOIR ) > 0 && getWorkers() > 0 )
    {
      _haveReservoirWater = true;
      _getAnimation().start();
    }
    else
    {
      //remove fontain service from tiles
      Tilemap& tmap = _getCity()->getTilemap();
      TilemapArea reachedTiles = tmap.getArea( getTilePos() - TilePos( 4, 4 ), Size( 4 + 4 ) + getSize() );
      foreach( Tile* tile, reachedTiles )
      {
        tile->decreaseWaterService( WTR_FONTAIN );
      }

      _getAnimation().stop();
    }

    if( !isActive() )
    {
      _getFgPictures().at( 0 ) = Picture::getInvalid();
      return;
    }

    Tilemap& tmap = _getCity()->getTilemap();
    TilemapArea reachedTiles = tmap.getArea( getTilePos() - TilePos( 4, 4 ), Size( 4 + 4 ) + getSize() );
    foreach( Tile* tile, reachedTiles )
    {
      tile->fillWaterService( WTR_FONTAIN );
    }
  }

  ServiceBuilding::timeStep( time );
}

bool Fountain::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool ret = Construction::canBuild( city, pos );

  Tilemap& tmap = city->getTilemap();
  const Tile& tile = tmap.at( pos );
  int picid = (tile.getWaterService( WTR_RESERVOIR ) > 0 ? fontainFull : fontainEmpty );
  const_cast< Fountain* >( this )->setPicture( ResourceGroup::waterbuildings, picid );

  return ret;
}

void Fountain::build( CityPtr city, const TilePos& pos )
{
  ServiceBuilding::build( city, pos );

  setPicture( ResourceGroup::waterbuildings, fontainEmpty );
}

bool Fountain::isNeedRoadAccess() const
{
  return false;
}

bool Fountain::isActive() const
{
  return ServiceBuilding::isActive() && _haveReservoirWater;
}

bool Fountain::haveReservoirAccess() const
{
  TilemapArea reachedTiles = _getCity()->getTilemap().getArea( getTilePos() - TilePos( 10, 10 ), Size( 10, 10 ) + getSize() );
  foreach( Tile* tile, reachedTiles )
  {
    TileOverlayPtr overlay = tile->getOverlay();
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
  _getAnimation().load( ResourceGroup::utilitya, fontainStartAnim, fontainSizeAnim );
  //animLoader.fill_animation_reverse(_animation, "utilitya", 25, 7);
  _getAnimation().setOffset( Point( 12, 24 ) );
  _getAnimation().setDelay( 2 );
  _getAnimation().stop();
}
