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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "coast.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "core/saveadapter.hpp"
#include "gfx/imgid.hpp"
#include "core/variant_map.hpp"
#include "core/foreach.hpp"
#include "objects_factory.hpp"
#include "terrain.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::coast, Coast)

namespace {
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Coast::Coast() : Overlay( object::terrain, Size(1) )
{
  setPicture( ResourceGroup::land1a, 199 );
}

bool Coast::build( const city::AreaInfo& info )
{
  Overlay::build( info );
  updatePicture();
  //deleteLater();
  //tile().setOverlay( nullptr );
  tile().setAnimation( Animation() );

  CoastList coasts = neighbors();
  for( auto nb : coasts )
    nb->updatePicture();

  return true;
}

void Coast::initTerrain(Tile& tile)
{
  tile.terrain().clear();
  tile.terrain().water = true;
  tile.terrain().coast = true;
}

Picture Coast::computePicture()
{
  return calcPicture();
}

bool Coast::isWalkable() const{ return false;}
bool Coast::isFlat() const { return true;}

void Coast::destroy()
{
  tile().terrain().coast = false;
  CoastList coasts = neighbors();
  for( auto nb : coasts )
    nb->updatePicture();
}

bool Coast::isDestructible() const { return false;}
Renderer::PassQueue Coast::passQueue() const { return riftPassQueue; }

static int __isWater( Tile& tile )
{
  bool isWater = tile.getFlag( Tile::tlWater ) || tile.getFlag( Tile::tlDeepWater );
  bool isCoast = tile.getFlag( Tile::tlCoast );
  return (isWater && !isCoast) ? 1 : 0;
}

Picture Coast::calcPicture()
{
  Tilemap& tmap = _map();
  TilePos mpos = tile().epos();

  int waterN = __isWater( tmap.at( mpos.nb().north() ) );
  int waterNE = __isWater( tmap.at( mpos.nb().northeast() ) );
  int waterE = __isWater( tmap.at( mpos.nb().east() ) );
  int waterSE = __isWater( tmap.at( mpos.nb().southeast() ) );
  int waterS = __isWater( tmap.at( mpos.nb().south() ) );
  int waterSW = __isWater( tmap.at( mpos.nb().southwest() ) );
  int waterW = __isWater( tmap.at( mpos.nb().west() ) );
  int waterNW = __isWater( tmap.at( mpos.nb().northwest() ) );

  int coastN = tmap.at( mpos.nb().north() ).getFlag( Tile::tlCoast );
  int coastNE = tmap.at( mpos.nb().northeast() ).getFlag( Tile::tlCoast );
  int coastNW = tmap.at( mpos.nb().northwest() ).getFlag( Tile::tlCoast );
  int coastE = tmap.at( mpos.nb().east() ).getFlag( Tile::tlCoast );
  int coastS = tmap.at( mpos.nb().south() ).getFlag( Tile::tlCoast );
  int coastSE = tmap.at( mpos.nb().southeast() ).getFlag( Tile::tlCoast );
  int coastW = tmap.at( mpos.nb().west() ).getFlag( Tile::tlCoast );
  int coastSW = tmap.at( mpos.nb().southwest() ).getFlag( Tile::tlCoast );

  _rindex = (waterN) + (waterNE << 1) + (waterE<<2) + (waterSE<<3)
              + (waterS << 4) + (waterSW<<5) + (waterW<<6) + (waterNW<<7)
              + (coastN<<8) + (coastNE << 9) + (coastE<<10) + (coastSE<<11)
              + (coastS << 12) + (coastSW<<13) + (coastW<<14) + (coastNW<<15);

  int start = 0;
  int size = 1;

  switch( _rindex )
  {
  case 0xcc03: case 0xce01: case 0xcf00:
  case 0xc001: case 0xc03: case 0x6681: case 0x4681:
  case 0x6483: case 0xe403: case 0xc601: case 0x6e81:
  case 0xc403: case 0x4483: case 0x6780: case 0xd403:
  case 0x4780: case 0x4f80: case 0x403: case 0x601:
  case 0xc700: case 0x6c83: case 0x4c83: case 0x7483:
  case 0xe601: case 0xec03: case 0xe502: case 0xee01:
  case 0x4e81: case 0xdc03:
    start = 128; size = 4; break;

  case 0xcc30: case 0xc10: case 0xec10: case 0xfc00:
  case 0x6c10: case 0x4030: case 0x6418:
  case 0xc438: case 0x4c30: case 0x4438: case 0xe418:
  case 0x7c00: case 0x5c20: case 0x4638: case 0x6718:
  case 0x6618: case 0xc638: case 0x418: case 0x4738:
  case 0x618:  case 0x6e10: case 0x4e30:
  case 0xde20: case 0xee10:
    start = 136; size = 4; break;

  case 0x5f8: case 0x7588: case 0xdf0: case 0x8d70:
  case 0x6d90: case 0x2dd0: case 0xad50: case 0x7d80:
  case 0xed10: case 0xfd00: case 0x3dc0: case 0xf508:
  case 0x5e18: case 0xb548: case 0x8578:
  case 0xe518: case 0x518: case 0xd10: case 0xa558:
  case 0x25d8: case 0x8778: case 0x6598: case 0x7f8:
  case 0x6798: case 0xc538: case 0x35c8: case 0xff0:
  case 0x8f70: case 0xa758:
    start = 172; size = 1; break;

  case 0x1508: case 0x1c00: case 0x1408: case 0x3408:
  case 0x1608: case 0x3c00: case 0x1e00: case 0x3608:
  case 0x9408: case 0x1708: case 0x7708: case 0xb708:
  case 0x9608:
    start = 148; size = 4; break;

  case 0x1be0: case 0x19e0: case 0x13e0: case 0x11e0:
  case 0x3c0: case 0x9560: case 0x15e0: case 0xb940:
  case 0x9960: case 0x9160: case 0x3bc0: case 0x31c0:
  case 0x7980: case 0xb140: case 0xf900: case 0xf100:
  case 0x3840: case 0x39c0: case 0x8140:
  case 0x1900: case 0x1100: case 0xfb00: case 0x1c0:
  case 0x33c0: case 0xf300: case 0x7380: case 0x9b60:
  case 0xb340: case 0x9760: case 0xbf40: case 0xbb40:
  case 0xdb20: case 0x9360: case 0x7b80:
    start = 140; size = 4; break;

  case 0xda05: case 0xd00f: case 0xd807: case 0x5a85:
  case 0x528d: case 0xde01: case 0x5804: case 0xd708:
  case 0xdf00: case 0xd30c: case 0xdb04: case 0x5788:
  case 0x5c00: case 0xd204: case 0x5689: case 0x5f80:
  case 0x508f: case 0x4087: case 0x5887: case 0xd202:
  case 0xf00f: case 0x100d: case 0xd609: case 0x2d0d:
  case 0x7a85: case 0x5b84: case 0x538c: case 0x7c83:
  case 0xf609: case 0xfe01: case 0x738c: case 0xd906:
    start = 170; size = 1; break;

  case 0x7608: case 0x4936: case 0x6b14: case 0x7906:
  case 0x4b34: case 0x413e: case 0x6916: case 0x433c:
  case 0x611e: case 0xc13e: case 0x11e: case 0x631c:
  case 0x4d32: case 0xd432: case 0x5b24: case 0x6d12:
  case 0xf906: case 0xcf30: case 0x6f10:
  case 0xcb34: case 0xc738: case 0xe11e: case 0x5f20:
  case 0xdb24: case 0xef10: case 0xe718:
    start = 171; size = 1; break;

  case 0x16e1: case 0x17e0: case 0x1403: case 0xf403:
  case 0x34c3: case 0x14e3: case 0x36c1: case 0xf601:
  case 0x7681: case 0x37c0: case 0x7780: case 0xb740:
  case 0xb641: case 0x9661: case 0xb443: case 0x9463:
  case 0x1ce3: case 0xd621: case 0xd423: case 0x9562:
  case 0xb542: case 0xd320: case 0xf700: case 0x9e61:
    start = 173; size = 1; break;

  case 0xcd00: case 0x4d80: case 0x6d80:
    start = 184; size = 1; break;

  case 0x5980:
    start = 192; size = 1; break;

  case 0xdd22:
    start = 183; size = 1; break;

  case 0x9508:
    start = 186; size = 1; break;

  case 0xd408:
    start = 190; size = 1; break;

  case 0x6180: case 0xe100: case 0x4380:
  case 0xc300: case 0xc100: case 0x100: case 0x6380:
  case 0x4980: case 0x6b80:
    start = 156; size = 4; break;

  case 0x5820: case 0xd820: case 0xd020: case 0xf800:
  case 0xf000: case 0x7800: case 0x7000: case 0x5320:
  case 0x5620: case 0x522: case 0x5e20: case 0xd620:
  case 0x5020: case 0x1800: case 0x1000: case 0x5a20:
  case 0xdc20:
    start = 152; size = 4; break;

  case 0x1906: case 0x110e: case 0x910e: case 0x9b04:
  case 0x9f00: case 0x9906: case 0x1804: case 0x9708:
  case 0x1b04: case 0x930c: case 0x1f00: case 0x304:
  case 0x130c: case 0x330c: case 0x3906:
  case 0x310e: case 0xb10e: case 0x3b04:
  case 0x7b04: case 0xb906: case 0xb30c: case 0xf10e:
  case 0x730c: case 0xbb04:
    start = 132; size = 4; break;

  case 0xd02: case 0x8d02: case 0x8502: case 0x502:
  case 0x8f00: case 0xc00: case 0xf00: case 0x1502:
  case 0x700: case 0x8700: case 0x2502: case 0x6502:
  case 0xf502: case 0xed02: case 0x3d02: case 0xb502:
  case 0x2d02: case 0x1d02:
    start = 144; size = 4; break;

  case 0x5588: case 0xd508: case 0x5d80: case 0xdd00:
    start = 168; size = 1; break;

  case 0xd522: case 0x5522:
    start = 169; size = 1; break;

  case 0xd10e:
    start = 174; size = 1; break;

  case 0x600: case 0xe00: case 0xe000: case 0x300:
  case 0xc000: case 0x1b00: case 0x8100: case 0x8300:
  case 0x3800: case 0xb900:
    start = -1;
  break;

  case 0x837c: case 0x7887: case 0x6996: case 0xc34b:
  case 0x46b9: case 0xea15: case 0xba45: case 0x1ae5:
  case 0x14eb: case 0x6897: case 0x6c90: case 0x3cc3:
  case 0xb847: case 0xb748: case 0xeb14: case 0x6c93:
  case 0xe817: case 0x8e71: case 0xb34c: case 0xee11:
  case 0xe01f: case 0x39c6: case 0x3fc:
    start = -2;
  break;
  }

  if( start == 0 )
  {
    std::map<int, std::set<int> > maptiles;
    VariantMap vm = config::load( ":/coast.model" );
    for( auto& item : vm )
    {
      std::set<int> tiles;
      VariantList tilesVm = item.second.toList();
      for( auto& t : tilesVm )
        tiles.insert( utils::toInt( t.toString(), 16 ) );

      maptiles[ utils::toInt( item.first ) ] = tiles;
    }

    for( auto& item : maptiles )
    {
      if( item.second.count( _rindex ) > 0 )
      {
        start = item.first;
        size = 1;
        break;
      }
    }
  }

  if( start == -1 )
  {
    tile().setOverlay( nullptr );
    tile().terrain().clear();
    destroy();
    deleteLater();
    return Terrain::randomPicture();
  }

  if( start == -2 )
  {
    tile().setOverlay( nullptr );
    tile().terrain().clear();
    tile().terrain().water = true;
    destroy();
    deleteLater();
    return Picture( ResourceGroup::land1a, 120 );
  }

  return Picture( ResourceGroup::land1a, start + math::random(size-1) );
}

void Coast::updatePicture()
{
  setPicture( computePicture() );
  tile().setPicture( picture() );
  tile().setImgId( imgid::fromResource( picture().name() ) );
}

CoastList Coast::neighbors() const
{
  return _map().getNeighbors(pos(), Tilemap::AllNeighbors)
               .overlays<Coast>();
}
