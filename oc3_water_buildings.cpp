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

#include "oc3_water_buildings.hpp"

#include <stdexcept>

#include "oc3_stringhelper.hpp"
#include "oc3_scenario.hpp"
#include "oc3_time.hpp"
#include "oc3_positioni.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_road.hpp"
#include "oc3_tile.hpp"
#include "oc3_walker_service.hpp"

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
};

Aqueduct::Aqueduct() : WaterSource( B_AQUEDUCT, Size(1) ) 
{
  setPicture( Picture::load( ResourceGroup::aqueduct, 133) ); // default picture for aqueduct
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

void Aqueduct::build(const TilePos& pos )
{
  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
  TerrainTile& terrain = tilemap.at( pos ).getTerrain();

  // we can't build if already have aqueduct here
  SmartPtr<Aqueduct> aqueveduct = terrain.getOverlay().as<Aqueduct>();
  if( aqueveduct.isValid() )
  {
    return;
  }

  Construction::build( pos );

  CityHelper helper( Scenario::instance().getCity() );
  std::list< AqueductPtr > aqueducts = helper.getBuildings<Aqueduct>( B_AQUEDUCT );
  for( std::list< AqueductPtr >::iterator it = aqueducts.begin(); it!=aqueducts.end(); ++it )
  {
    (*it)->updatePicture();
  }

  updatePicture();
}

void Aqueduct::destroy()
{
  Construction::destroy();
}

void Aqueduct::setTerrain(TerrainTile &terrain)
{
  bool isRoad   = terrain.isRoad();
  bool isMeadow = terrain.isMeadow();

  terrain.clearFlags();
  terrain.setOverlay(this);
  terrain.setBuilding(true);
  terrain.setRoad(isRoad);
  terrain.setMeadow(isMeadow);
  terrain.setAqueduct(true); // mandatory!
}

bool Aqueduct::canBuild( const TilePos& pos ) const
{
  bool is_free = Construction::canBuild( pos );
  
  if( is_free ) 
      return true; // we try to build on free tile
  
  // we can place on road
  CityPtr city = Scenario::instance().getCity();
  Tilemap& tilemap = city->getTilemap();
  TerrainTile& terrain = tilemap.at( pos ).getTerrain();

  // we can't build on plazas
  if( terrain.getOverlay().as<Plaza>().isValid() )
      return false;

  // we can show that won't build over other aqueduct
  if( terrain.getOverlay().as<Aqueduct>().isValid() )
      return false;

  // also we can't build if next tile is road + aqueduct
  if ( terrain.isRoad() )
  {
    std::list<Tile*> rect = tilemap.getRectangle( pos + TilePos (-1, -1),
                                                  pos + TilePos (1, 1), 
                                                  !Tilemap::checkCorners );
    for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
    {
      Tile* tile = *itTiles;
      if (tile->getTerrain().isRoad() && tile->getTerrain().isAqueduct())
      {
	      return false;
      }
    }    
  }
  
  // and we can't build on intersections
  if ( terrain.isRoad() )
  {
    int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
    if (tilemap.at( pos + TilePos(  0,  1 ) ).getTerrain().isRoad()) { directionFlags += 1; } // road to the north
    if (tilemap.at( pos + TilePos(  0, -1 ) ).getTerrain().isRoad()) { directionFlags += 4; } // road to the south
    if (tilemap.at( pos + TilePos(  1,  0 ) ).getTerrain().isRoad()) { directionFlags += 2; } // road to the east
    if (tilemap.at( pos + TilePos( -1,  0 ) ).getTerrain().isRoad()) { directionFlags += 8; } // road to the west

    StringHelper::debug( 0xff, "direction flags=%d", directionFlags );
   
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



Picture&
Aqueduct::computePicture(const PtrTilesList * tmp, const TilePos pos)
{
  // find correct picture as for roads
  Tilemap& tmap = Scenario::instance().getCity()->getTilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (tmp == NULL) ? getTilePos() : pos;

  LandOverlayPtr northOverlay = tmap.at( tile_pos + TilePos(  0,  1) ).getTerrain().getOverlay();
  LandOverlayPtr eastOverlay  = tmap.at( tile_pos + TilePos(  1,  0) ).getTerrain().getOverlay();
  LandOverlayPtr southOverlay = tmap.at( tile_pos + TilePos(  0, -1) ).getTerrain().getOverlay();
  LandOverlayPtr westOverlay  = tmap.at( tile_pos + TilePos( -1,  0) ).getTerrain().getOverlay();

  bool isNorthBusy = false;
  bool isEastBusy  = false;
  bool isSouthBusy = false;
  bool isWestBusy  = false;

  if (tmp != NULL)
    for (PtrTilesList::const_iterator it = tmp->begin(); it != tmp->end(); ++it)
    {
      int i = (*it)->getI();
      int j = (*it)->getJ();

      if (i == pos.getI() && j == (pos.getJ() + 1))
        isNorthBusy = true;
      else if (i == pos.getI() && j == (pos.getJ() - 1))
        isSouthBusy = true;
      else if (j == pos.getJ() && i == (pos.getI() + 1))
        isEastBusy = true;
      else if (j == pos.getJ() && i == (pos.getI() - 1))
        isWestBusy = true;
    }

  if( northOverlay.is<Aqueduct>() || northOverlay.is<Reservoir>() || isNorthBusy) { directionFlags += 1; }
  if( eastOverlay.is<Aqueduct>()  || eastOverlay.is<Reservoir>()  || isEastBusy) { directionFlags += 2; }
  if( southOverlay.is<Aqueduct>() || southOverlay.is<Reservoir>() || isSouthBusy) { directionFlags += 4; }
  if( westOverlay.is<Aqueduct>()  || westOverlay.is<Reservoir>()  || isWestBusy) { directionFlags += 8; }

  int index;
  switch (directionFlags)
  {
  case 0:  // no neighbours!
    index = 121; break;
  case 1:  // N
  case 4:  // S
  case 5:  // N + S
    index = 121; 
    if( tmap.at( tile_pos ).getTerrain().isRoad() ) 
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
    if( tmap.at( tile_pos ).getTerrain().isRoad() )
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



void Aqueduct::updatePicture()
{
  setPicture(computePicture());
}

bool Aqueduct::isNeedRoadAccess() const
{
  return false;
}

void Aqueduct::_waterStateChanged()
{
  updatePicture();
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
  Tilemap& tmap = Scenario::instance().getCity()->getTilemap();
  PtrTilesArea reachedTiles = tmap.getFilledRectangle( getTilePos() - TilePos( 10, 10 ), Size( 10 + 10 ) + getSize() ); 
  for( PtrTilesArea::iterator it=reachedTiles.begin(); it != reachedTiles.end(); it++ )
  {
    (*it)->getTerrain().decreaseWaterService( WTR_RESERVOIR );
  }


  // update adjacent aqueducts
  Construction::destroy();
}

Reservoir::Reservoir() : WaterSource( B_RESERVOIR, Size( 3 ) )
{
  setPicture( Picture::load( ResourceGroup::waterbuildings, 1 )  );
  
  // utilitya 34      - empty reservoir
  // utilitya 35 ~ 42 - full reservoir animation
 
  _getAnimation().load( ResourceGroup::utilitya, 35, 8);
  _getAnimation().load( ResourceGroup::utilitya, 42, 7, Animation::reverse);
  _getAnimation().setFrameDelay( 11 );
  _getAnimation().setOffset( Point( 47, 63 ) );

  _fgPictures.resize(1);
  //_fgPictures[0]=;
}

Reservoir::~Reservoir()
{
}

void Reservoir::build(const TilePos& pos )
{
  Construction::build( pos );  

  setPicture( Picture::load( ResourceGroup::waterbuildings, 1 ) );
  _isWaterSource = _isNearWater( pos );
  
  //updateAqueducts();
  
  // update adjacent aqueducts
}

bool Reservoir::_isNearWater( const TilePos& pos ) const
{
  bool near_water = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();
  std::list<Tile*> rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), getSize() + Size( 2 ), !Tilemap::checkCorners );
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    near_water |= (*itTiles)->getTerrain().isWater();
  }

  return near_water;
}

void Reservoir::setTerrain(TerrainTile &terrain)
{
  bool isMeadow = terrain.isMeadow();
  terrain.clearFlags();
  terrain.setOverlay(this);
  terrain.setBuilding(true);
  terrain.setMeadow(isMeadow);
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
    _fgPictures[ 0 ] = 0;
    return;
  }

  //filled area, that reservoir present
  if( time % 22 == 1 )
  {
    Tilemap& tmap = Scenario::instance().getCity()->getTilemap();
    PtrTilesArea reachedTiles = tmap.getFilledRectangle( getTilePos() - TilePos( 10, 10 ), Size( 10 + 10 ) + getSize() ); 
    for( PtrTilesArea::iterator it=reachedTiles.begin(); it != reachedTiles.end(); it++ )
    {
      (*it)->getTerrain().fillWaterService( WTR_RESERVOIR );
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
  _fgPictures[ 0 ] = _getAnimation().getCurrentPicture(); 
}

bool Reservoir::canBuild( const TilePos& pos ) const
{
  bool ret = Construction::canBuild( pos );

  bool nearWater = _isNearWater( pos );
  const_cast< Reservoir* >( this )->setPicture( Picture::load( ResourceGroup::waterbuildings, nearWater ? 2 : 1 )  );

  return ret;
}

bool Reservoir::isNeedRoadAccess() const
{
  return false;
}

WaterSource::WaterSource( const BuildingType type, const Size& size )
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

    for( Impl::WaterSourceMap::iterator it=_d->sourcesMap.begin(); it != _d->sourcesMap.end(); it++ )
    {
      (*it).second = math::clamp( (*it).second-1, 0, 4 );
    }
  }

  Construction::timeStep( time );
}

void WaterSource::_produceWater( const TilePos* points, const int size )
{
  Tilemap& tilemap = Scenario::instance().getCity()->getTilemap();

  for( int index=0; index < size; index++ )
  {
    TilePos pos = getTilePos() + points[index];
    SmartPtr< WaterSource > ws = tilemap.at( pos ).getTerrain().getOverlay().as<WaterSource>();
    
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

BuildingFountain::BuildingFountain() : ServiceBuilding(S_FOUNTAIN, B_FOUNTAIN, Size(1))
{  
  std::srand( DateTime::getElapsedTime() );

  //id = std::rand() % 4;

  setPicture( Picture::load( ResourceGroup::utilitya, 10));
  _getAnimation().load( ResourceGroup::utilitya, 11, 7);
  //animLoader.fill_animation_reverse(_animation, "utilitya", 25, 7);
  _getAnimation().setOffset( Point( 12, 24 ) );
  _fgPictures.resize(1);

  //2 10 18 26
  // utilitya 10      - empty 
  // utilitya 11 - 17 - working fontain

  // the first fountain's (10) ofsets ~ 11, 23 
  /*AnimLoader animLoader(PicLoader::instance());
  animLoader.fill_animation(_animation, "utilitya", 11, 7); 
  animLoader.change_offset(_animation, 11, 23);
  _fgPictures.resize(1);*/

  // the second (2)    ~ 8, 42
  // the third (18)    ~ 8, 24
  // the 4rd   (26)    ~14, 26     

  setWorkers( 1 );
}

void BuildingFountain::deliverService()
{
  const TerrainTile& myTile = getTile().getTerrain();

  if( myTile.getWaterService( WTR_RESERVOIR ) > 0 && getWorkers() > 0 )
  {
    _haveReservoirWater = true;
  }
  else
  {
    //remove fontain service from tiles
    Tilemap& tmap = Scenario::instance().getCity()->getTilemap();
    PtrTilesArea reachedTiles = tmap.getFilledRectangle( getTilePos() - TilePos( 4, 4 ), Size( 4 + 4 ) + getSize() ); 
    for( PtrTilesArea::iterator it=reachedTiles.begin(); it != reachedTiles.end(); it++ )
    {
      (*it)->getTerrain().decreaseWaterService( WTR_FONTAIN );
    }
  }

  ServiceWalkerPtr walker = ServiceWalker::create( Scenario::instance().getCity(), getService() );
  walker->setBase( BuildingPtr( this ) );
  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( getTile().getIJ() );
  for( ServiceWalker::ReachedBuildings::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
  {
    (*itBuilding)->applyService( walker );
  } 
}

void BuildingFountain::timeStep(const unsigned long time)
{
  if( !_haveReservoirWater )
  {
    _fgPictures[ 0 ] = 0;
    return;
  }

  //filled area, that fontain present and work
  if( time % 22 == 1 )
  {
    Tilemap& tmap = Scenario::instance().getCity()->getTilemap();
    PtrTilesArea reachedTiles = tmap.getFilledRectangle( getTilePos() - TilePos( 4, 4 ), Size( 4 + 4 ) + getSize() ); 
    for( PtrTilesArea::iterator it=reachedTiles.begin(); it != reachedTiles.end(); it++ )
    {
      (*it)->getTerrain().fillWaterService( WTR_FONTAIN );
    }
  }

  _getAnimation().update( time );

  // takes current animation frame and put it into foreground
  _fgPictures[ 0 ] = _getAnimation().getCurrentPicture(); 
}

bool BuildingFountain::canBuild( const TilePos& pos ) const
{
  bool ret = Construction::canBuild( pos );

  Tilemap& tmap = Scenario::instance().getCity()->getTilemap();
  const TerrainTile& buildTerrain = tmap.at( pos ).getTerrain();
  bool reservoirPresent = buildTerrain.getWaterService( WTR_RESERVOIR ) > 0;
  const_cast< BuildingFountain* >( this )->setPicture( Picture::load( ResourceGroup::waterbuildings, reservoirPresent ? 4 : 3 )  );

  return ret;
}

void BuildingFountain::build( const TilePos& pos )
{
  ServiceBuilding::build( pos );  

  setPicture( Picture::load( ResourceGroup::waterbuildings, 3 ) );
}

bool BuildingFountain::isNeedRoadAccess() const
{
  return false;
}
