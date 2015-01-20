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

#include "fortification.hpp"

#include "core/utils.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "constants.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "events/warningmessage.hpp"
#include "objects/road.hpp"
#include "core/variant_map.hpp"
#include "core/direction.hpp"
#include "core/logger.hpp"
#include "tower.hpp"
#include "core/font.hpp"
#include "gatehouse.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::fortification, Fortification)

class Fortification::Impl
{
public:
  int direction;
  Picture tmpPicture;
  Point offset;
  bool mayPatrol;
  bool isTowerEnter;
  int index;

  bool isFortification( PlayerCityPtr city, TilePos pos, bool tower=false );
};

Fortification::Fortification() : Wall(), _d( new Impl )
{
  setType( objects::fortification );
  setPicture( ResourceGroup::wall, 178 ); // default picture for wall

  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );
}

Fortification::~Fortification() {}

bool Fortification::build( const CityAreaInfo& info )
{
  // we can't build if already have wall here
  WallPtr wall = ptr_cast<Wall>( info.city->getOverlay( info.pos ) );
  if( wall.isValid() )
  {
    return false;
  }

  Pathway way2border = PathwayHelper::create( info.pos, info.city->borderInfo().roadEntry, PathwayHelper::allTerrain );
  if( !way2border.isValid() )
  {
    events::GameEventPtr event = events::WarningMessage::create( "##walls_need_a_gatehouse##" );
    event->dispatch();
  }

  Building::build( info );

  city::Helper helper( info.city );
  FortificationList fortifications = helper.find<Fortification>( objects::fortification );  

  foreach( frt, fortifications ) { (*frt)->updatePicture( info.city ); }

  TowerList towers = helper.find<Tower>( objects::tower );
  foreach( tower, towers ) { (*tower)->resetPatroling(); }

  updatePicture( info.city );

  return true;
}

void Fortification::destroy()
{
  Construction::destroy();

  if( _city().isValid() )
  {
    TilesArray area = _city()->tilemap().getArea( pos() - TilePos( 2, 2), Size( 5 ) );
    foreach( tile, area )
    {
      FortificationPtr f = ptr_cast<Fortification>( (*tile)->overlay() );
      if( f.isValid()  )
      {
        f->updatePicture( _city() );
      }
    }
  }
}

Point Fortification::offset( const Tile& tile, const Point& subpos) const
{
  switch( _d->index )
  {
  case 155:
  case 160:
  case 173: return _d->offset + Point( -8, 0 );

  case 154:
  case 161:
  case 171: return _d->offset + Point( -8, 0 );

  case 152:
  case 172:
  case 159:
  case 175:
  case 176:
  case 177:
  case 181:
  case 182: return _d->offset + Point( -8, 0);
  }

  return _d->offset;
}

const Picture& Fortification::picture(const CityAreaInfo& areaInfo) const
{
  // find correct picture as for roads
  Tilemap& tmap = areaInfo.city->tilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (areaInfo.aroundTiles.empty()) ? pos() : areaInfo.pos;

  if (!tmap.isInside(tile_pos))
    return Picture::load( ResourceGroup::aqueduct, 121 );

  TilePos tile_pos_d[countDirection];
  bool is_border[countDirection] = { 0 };
  bool is_busy[countDirection] = { 0 };

  tile_pos_d[north] = tile_pos + TilePos(  0,  1);
  tile_pos_d[east]  = tile_pos + TilePos(  1,  0);
  tile_pos_d[south] = tile_pos + TilePos(  0, -1);
  tile_pos_d[west]  = tile_pos + TilePos( -1,  0);
  tile_pos_d[northEast] = tile_pos + TilePos( 1, 1 );
  tile_pos_d[southEast] = tile_pos + TilePos( 1, -1 );
  tile_pos_d[southWest] = tile_pos + TilePos( -1, -1 );
  tile_pos_d[northWest] = tile_pos + TilePos( -1, 1 );


  // all tiles must be in map range
  for (int i = 0; i < countDirection; ++i)
  {
    is_border[i] = !tmap.isInside(tile_pos_d[i]);
    if (is_border[i])
      tile_pos_d[i] = tile_pos;
  }

  // get overlays for all directions
  TileOverlayPtr overlay_d[countDirection];
  overlay_d[north] = tmap.at( tile_pos_d[north] ).overlay();
  overlay_d[east] = tmap.at( tile_pos_d[east]  ).overlay();
  overlay_d[south] = tmap.at( tile_pos_d[south] ).overlay();
  overlay_d[west] = tmap.at( tile_pos_d[west]  ).overlay();
  overlay_d[northEast] = tmap.at( tile_pos_d[northEast]  ).overlay();
  overlay_d[southEast] = tmap.at( tile_pos_d[southEast]  ).overlay();
  overlay_d[southWest] = tmap.at( tile_pos_d[southWest]  ).overlay();
  overlay_d[northWest] = tmap.at( tile_pos_d[northWest]  ).overlay();

  // if we have a TMP array with wall, calculate them
  const TilePos& p = areaInfo.pos;
  if( !areaInfo.aroundTiles.empty())
  {
    foreach( it, areaInfo.aroundTiles )
    {
      if( (*it)->overlay().isNull()
          || (*it)->overlay()->type() != objects::fortification )
        continue;

      TilePos rpos = (*it)->pos();
      int i = (*it)->i();
      int j = (*it)->j();

      if( (p + TilePos( 0, 1 )) == rpos ) is_busy[north] = true;
      else if(i == p.i() && j == (p.j() - 1)) is_busy[south] = true;
      else if(j == p.j() && i == (p.i() + 1)) is_busy[east] = true;
      else if(j == p.j() && i == (p.i() - 1)) is_busy[west] = true;
      else if((p + TilePos(1, 1)) == rpos ) is_busy[northEast] = true;
      else if((p + TilePos(1, -1)) == rpos ) is_busy[southEast] = true;
      else if((p + TilePos(-1, -1)) == rpos ) is_busy[southWest] = true;
      else if((p + TilePos(-1, 1)) == rpos ) is_busy[northWest] = true;
    }
  }

  // calculate directions
  for (int i = 0; i < countDirection; ++i)
  {
    if (!is_border[i] &&
        ( (overlay_d[i].isValid() && overlay_d[i]->type() == objects::fortification) || is_busy[i]))
    {
      switch (i)
      {
      case north: directionFlags += 0x1; break;
      case east:  directionFlags += 0x2; break;
      case south: directionFlags += 0x4; break;
      case west:  directionFlags += 0x8; break;
      case northEast: directionFlags += 0x10; break;
      case southEast: directionFlags += 0x20; break;
      case southWest: directionFlags += 0x40; break;
      case northWest: directionFlags += 0x80; break;
      default: break;
      }
    }
  }

  const_cast< Fortification* >( this )->_d->direction = directionFlags;
  Fortification& th = *const_cast< Fortification* >( this );
  th._d->offset =  Point( 0, 0 );
  th._fgPicturesRef().clear();
  int index;
  switch( directionFlags )
  {  
  case 0: index = 178; break;  // no neighbours!
  case 1: index = 168; break;  //
  case 2: index = 157; break;  // E
  case 3: index = 157; break;  // N + E
  case 4: index = 162; break;  // S
  case 0x25: index = 156; break;
  case 0x15: index = 156; break;
  case 0x16: index = 174; break;
  case 0x8d: index = 156; break;
  case 0x11: index = 168; break;
  case 0x1f: index = 174; break;
  case 0x91: index = 168; break;
  case 0x48: index = 167; break;
  case 0x4f: index = 174; break;
  case 0xd8: index = 167; break;
  case 0xdd: index = 156; break;
  case 0x41: index = 168; break;
  case 0xde: index = 174; break;
  case 0x36: index = 176; _d->offset = Point( 0, 12 ); break;
  case 0x3f: index = 171; _d->offset = Point( 0, 12 ); break;
  case 0x4e: index = 174; break;
  case 0x23: case 0x83: case 0xa3: index = 179; break;
  case 0xad: index = 156; break;
  case 0xac: index = 180; break;
  case 0xaf: index = 169; _d->offset = Point( 0, 12 ); break;
  case 0xcc: case 0x8c: index = 162; break;
  case 0x7e: index = 173; _d->offset = Point( 0, 12 ); break;
  case 0x7b: index = 153; break;
  case 0xb3: index = 157; break;
  case 0x87: index = 174; break;
  case 0xb7: case 0xbf: index = 171; _d->offset = Point( 0, 12 ); break;
  case 0x5a: case 0x2a: index = 153; break;
  case 0x57: index = 174; break;
  case 0x58: index = 167; break;
  case 0x5e: index = 174; break;
  case 0x55: case 0xa5: index = 156; break;
  case 0x49: index = 164; break;
  case 0xa7: index = 175; _d->offset = Point( 0, 12 ); break;
  case 0x9b: index = 153; break;
  case 0x85: index = 156; break;
  case 0x56: index = 174; break;
  case 0x51: index = 156; break;
  case 0xba: index = 153; break;
  case 0xa1: index = 168; break;
  case 0x9a: case 0x2b: index = 153; break;
  case 0x99: index = 164; break;
  case 0xc9: index = 164; break;
  case 0x9d: index = 156; break;
  case 0xc5: index = 156; break;
  case 0x29: case 0xa9: index = 164; break;
  case 0x22: index = 157; break;
  case 0xdf: index = 174; break;
  case 0xae: case 0xa6: index = 175; _d->offset = Point( 0, 12 ); break;
  case 0xbe: index = 175; _d->offset = Point( 0, 12 ); break;
  case 0x4c: case 0x6c: case 0xec: index = 162; break;  // sw + s + w
  case 0x46: index = 174; break;
  case 0xef: index = 173; _d->offset = Point( 0, 12 ); break;
  case 0x65: index = 156; break;
  case 0x67: index = 175; _d->offset = Point( 0, 12 ); break;
  case 5: index = 156; break;  // N + S
  case 6: index = 174; break;  // E + S
  case 0x12: case 0x73: index = 157; break; //NE + E
  case 0x1e: case 0x47: index = 174; break;
  case 0xc1: index = 168; break;
  case 0xce: index = 174; break;
  case 0x1a: index = 153; break;
  case 0x7d: case 0xb5: index = 156; break;
  case 0x71: index = 168; break;
  case 0xfe: index = 173; _d->offset = Point( 0, 12 ); break;
  case 0xf7: index = 171; _d->offset = Point( 0, 12 ); break;
  case 0xa4: index = 162; break;
  case 0xfd: case 0xfc: index = 156; break;
  case 0xeb: index = 153; break;
  case 0xaa: case 0xfa: index = 153; break;
  case 0x5f: index = 174; break;
  case 0xee: index = 173; _d->offset = Point( 0, 12 ); break;
  case 0xe9: index = 164; break;
  case 0xbd: index = 156; break;
  case 0x86: index = 174; break; //e + s + nw
  case 0x24: index = 162; break;
  case 0x2d: index = 156; break;
  case 0x2c: index = 180; break;
  case 0x44: index = 162; break; //SW + S
  case 0xe3: index = 179; break;
  case 0x26: case 0xe7: case 0xe6: index = 175; _d->offset = Point( 0, 12 ); break; //e + s + se
  case 8: index = 167; break; // W
  case 9: index = 164; break; // N + W
  case 0x59: index = 164; break; // n + w + ne + sw
  case 0x5b: index = 153; break;
  case 0x5d: case 0xc7: index = 156; break;
  case 0x5c: index = 162; break;
  case 0xab: index = 153; break;
  case 10: index = 153; break; // E + W
  case 12: index = 162; break; // S + W
  case 14: index = 174; break; // E + S + W
  case 0x0f: index = 174; break;
  case 0x2e: case 0xb6: index = 175; _d->offset = Point( 0, 12 ); break;
  case 11: index = 153; break; // N + E + W
  case 13: index = 156; break; // W + S + N
  case 7: index = 174; break;// N + E + S
  case 0x81: index = 168; break;
  case 0x82: index = 157; break;
  case 0x6a: index = 153; break;
  case 0x6f: index = 173; _d->offset = Point( 0, 12 ); break;
  case 0xfb: index = 153; break;
  case 0xf5: index = 156; break;
  case 0x8e: index = 174; break;
  case 0x45: index = 156; break;
  case 0x88: index = 167; break;
  case 0x1b: index = 153; break;
  case 0x1c: case 0xdc: index = 162; break;
  case 0x19: index = 164; break;
  case 0x64: index = 162; break;
  case 0x68: index = 167; break;
  case 0x6e: index = 155; _d->offset = Point( 0, 12 ); break;
  case 0xff: index = 152; _d->offset = Point( 0, 12 ); break;
  case 0x66: case 0x76: index = 175; _d->offset = Point( 0, 12 ); break;
  case 0x7f: index = 172; _d->offset = Point( 0, 12 ); break;
  case 0x7a: case 0xea: index = 153; break;
  case 0x77: index = 171; _d->offset = Point( 0, 12 ); break;
  case 0x4d: index = 162; break;
  case 0x52: index = 157; break;
  case 0xcb: case 0xca: index = 153; break;
  case 0xcf: case 0xd7: index = 174; break;
  case 0x17: index = 174; break;
  case 0x8b: index = 153; break;
  case 0x4a: index = 153; break;
  case 0x84: index = 162; break;
  case 0x1d: index = 156; break;
  case 0x32: index = 157; break;
  case 0x34: index = 162; break;
  case 0x3a: index = 153; break;
  case 0x35: index = 156; break;
  case 0x3e: index = 176; _d->offset = Point( 0, 12 ); break;
  case 0x33: case 0xd3: index = 157; break;
  case 0x3b: index = 153; break;
  case 0x31: index = 168; break;
  case 0x37: index = 171; _d->offset = Point( 0, 12 ); break;
  case 0xed: case 0xe5: index = 156;
  case 0xcd: index = 156; break;
  case 0xc8: index = 167; break;
  case 0x27: index = 159; _d->offset = Point( 0, 12 ); break;
  case 0x28: index = 167; break;
  case 0x13: index = 157; break;
  case 0x8a: case 0xdb: index = 153; break;
  case 0xda: index = 153; break;
  case 0x6d: index = 156; break;
  case 0xd5: index = 156; break;
  case 0xd9: case 0x39: index = 164; break;
  case 0x8f: index = 174; break; // N + S + E + W (crossing)
  case 0x2f: index = 175; _d->offset = Point( 0, 12 ); break;
  case 0x53: index = 157; break;
  case 0x54: index = 162; break;
  case 0x89: index = 164; break; //nw + w + n
  case 0x14: index = 162; break; // ne + s
  case 0x40: index = 178; break; // sw
  case 0x9c: index = 164; break;
  case 0x96: index = 174; break;
  case 0x97: index = 174; break;
  case 0x92: index = 157; break;
  case 0x9e: index = 174; break;
  case 0x98: index = 167; break;
  case 0x95: index = 156; break;
  case 0x9f: index = 174; break;
  case 0xbb: index = 153; break;
  case 0xbc: index = 180; break;
  case 0x93: index = 157; break;

  default:
    index = 178; // it's impossible, but ...
    //Logger::warning( "Impossible direction on wall building [%d,%d]", pos.getI(), pos.getJ() );
  }

  _d->isTowerEnter = false;
  bool towerNorth = _d->isFortification( areaInfo.city, p + TilePos( 0, 1 ) );
  bool towerWest = _d->isFortification( areaInfo.city, p + TilePos( -1, 0 ) );
  bool towerEast = _d->isFortification( areaInfo.city, p + TilePos( 1, 0 ) );

  switch( index )
  {
  case 175:
    {
      towerNorth = _d->isFortification( areaInfo.city, p + TilePos( 0, 1 ), true );
      towerWest = _d->isFortification( areaInfo.city, p + TilePos( -1, 0 ), true );
      _d->isTowerEnter = (towerNorth || towerWest);
      if( towerNorth ) { index = 181; }
      else if( towerWest ) { index = 182; }
    }
  break;

  case 157:
    if( towerWest ) { index = 184; }
  break;

  case 162:
    if( towerNorth ) { index = 183; }
  break;

  case 164:
    if( towerEast ) { index = 153; }
  break;
  }

  _d->index = index;
  th._d->mayPatrol = (_d->offset.y() > 0);

  th._d->tmpPicture = Picture::load( ResourceGroup::wall, index );
  th._d->tmpPicture.addOffset( _d->offset );
  return _d->tmpPicture;
}

int Fortification::getDirection() const {  return _d->direction;}

void Fortification::updatePicture(PlayerCityPtr city)
{
  CityAreaInfo info = { city, pos(), TilesArray() };
  setPicture( picture( info) );
}

bool Fortification::isTowerEnter() const {  return _d->isTowerEnter;}
bool Fortification::mayPatrol() const{  return _d->mayPatrol;}
bool Fortification::isFlat() const{  return false;}

void Fortification::save(VariantMap& stream) const
{
  Wall::save( stream );

  stream[ "direction" ] = (int)_d->direction;
  stream[ "offset" ] = _d->offset;
  stream[ "mayPatrol" ] = _d->mayPatrol;
  stream[ "isTowerEnter" ] = _d->isTowerEnter;
  stream[ "index" ] = _d->index;
}

void Fortification::load(const VariantMap& stream)
{
  Wall::load( stream );
  _d->direction = (Direction)stream.get( "direction" ).toInt();
  _d->offset = stream.get( "offset" );
  _d->mayPatrol = stream.get( "mayPatrol" );
  _d->isTowerEnter = stream.get( "isTowerEnter" );
  _d->index = stream.get( "index" );
}


bool Fortification::Impl::isFortification( PlayerCityPtr city, TilePos pos, bool tower )
{
  TileOverlayPtr ov = city->getOverlay( pos );
  return tower
            ? is_kind_of<Tower>( ov )
            : is_kind_of<Tower>( ov ) || is_kind_of<Gatehouse>( ov );
}
