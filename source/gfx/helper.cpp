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

#include "helper.hpp"
#include "core/exception.hpp"
#include "objects/building.hpp"
#include "tileoverlay.hpp"
#include "animation_bank.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "picture_bank.hpp"
#include "core/logger.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace gfx
{

namespace tilemap
{

static int x_tileBase = 60;
static int y_tileBase = x_tileBase / 2;
static Size tilePicSize( x_tileBase * 2 - 2, x_tileBase );
static Size tileCellSize( x_tileBase, y_tileBase );
static Point centerOffset( y_tileBase / 2, y_tileBase / 2 );

void initTileBase(int width)
{
  x_tileBase = width;
  y_tileBase = x_tileBase / 2;
  tilePicSize = Size( x_tileBase * 2 - 2, x_tileBase );
  tileCellSize = Size( x_tileBase, y_tileBase );
  centerOffset = Point( y_tileBase / 2, y_tileBase / 2 );
}

const Point& cellCenter() { return centerOffset;}
const Size& cellPicSize() { return tilePicSize; }
const Size& cellSize() { return tileCellSize; }

Direction getDirection(const TilePos& b, const TilePos& e)
{
  float t = (e - b).getAngleICW();
  int angle = (int)ceil( t / 45.f);

  Direction directions[] = { east, southEast, south, southWest,
                             west, northWest, north, northEast, northEast };

  return directions[ angle ];
}

}

namespace imgid
{

std::string toResource( const unsigned int imgId )
{
  // example: for land1a_00004, pfx=land1a and id=4
  std::string res_pfx;  // resource name prefix
  int res_id = imgId;   // id of resource

  if( imgId < 245 )
  {
    res_pfx = ResourceGroup::plateau;
    res_id = imgId - 200;
  }
  else if( imgId < 548 )
  {
    res_pfx = ResourceGroup::land1a;
    res_id = imgId - 244;
  }
  else if( imgId < 779 )
  {
    res_pfx = ResourceGroup::land2a;
    res_id = imgId - 547;
  }
  else if( imgId < 871)
  {
    res_pfx = ResourceGroup::land3a;
    res_id = imgId - 778;
  }
  else
  {
    res_pfx = ResourceGroup::land1a;
    res_id = 0;

    if (imgId == 0xb10 || imgId == 0xb0d)
    {
      res_pfx = ResourceGroup::housing;
      res_id = 51;
    } // TERRIBLE HACK!

    Logger::warning( "TileHelper: unknown image Id=%d ", imgId );
  }

  std::string ret_str = utils::format( 0xff, "%s_%05d", res_pfx.c_str(), res_id );
  return ret_str;
}

int fromResource( const std::string &pic_name )
{
  // example: for land1a_00004, return 244+4=248
  std::string res_pfx;  // resource name prefix = land1a
  int res_id = 0;   // idx of resource = 4

  // extract the name and idx from name (ex: [land1a, 4])
  int pos = pic_name.find("_");
  res_pfx = pic_name.substr(0, pos);
  std::stringstream ss(pic_name.substr(pos+1));
  ss >> res_id;

  if (res_pfx == ResourceGroup::plateau ){  res_id += 200; }
  else if (res_pfx == ResourceGroup::land1a) { res_id += 244; }
  else if (res_pfx == ResourceGroup::land2a) { res_id += 547; }
  else if (res_pfx == ResourceGroup::land3a) { res_id += 778; }
  else
  {
    Logger::warning( "TileHelper: unknown image " + pic_name );
  }

  return res_id;
}

Picture& toPicture(const unsigned int imgId)
{
  std::string picname = toResource( imgId );
  return Picture::load( picname );
}

}

namespace tile
{

int encode(const Tile& tt)
{
  int res = tt.getFlag( Tile::tlTree )   ? 0x00011 : 0;
  res += tt.getFlag( Tile::tlRock )      ? 0x00002 : 0;
  res += tt.getFlag( Tile::tlWater )     ? 0x00004 : 0;
  res += tt.getFlag( Tile::tlOverlay )   ? 0x00008 : 0;
  res += tt.getFlag( Tile::tlRoad )      ? 0x00040 : 0;
  res += tt.getFlag( Tile::tlCoast )     ? 0x00100 : 0;
  res += tt.getFlag( Tile::tlElevation ) ? 0x00200 : 0;
  res += tt.getFlag( Tile::tlMeadow )    ? 0x00800 : 0;
  res += tt.getFlag( Tile::tlRubble )    ? 0x01000 : 0;
  res += tt.getFlag( Tile::tlWall )      ? 0x04000 : 0;
  res += tt.getFlag( Tile::tlDeepWater ) ? 0x08000 : 0;
  res += tt.getFlag( Tile::tlRift )      ? 0x10000 : 0;
  return res;
}

static int __turnBySet( int imgid, int start, int length, int frameCount, int angle90time )
{
  imgid -= start;
  imgid += (frameCount * angle90time);
  imgid %= (frameCount * 4);
  imgid += start;

  return imgid;
}

int turnCoastTile(int imgid, constants::Direction newDirection )
{
  int koeff[] = { 0, 0, 0, 1, 1, 2, 2, 3, 3, -1};
  imgid -= 372;
  if( koeff[ newDirection ] >= 0 )
  {
    if( (imgid >= 0 && imgid <= 15) )
    {
      imgid = __turnBySet( imgid, 0, 16, 4, koeff[ newDirection ] );
    }
    else if( imgid >= 16 && imgid <= 31 )
    {
      imgid = __turnBySet( imgid, 16, 16, 4, koeff[ newDirection ] );
    }
    else if( imgid >= 36 && imgid <= 39 )
    {
      imgid = __turnBySet( imgid, 36, 4, 1, koeff[ newDirection ] );
    }
    else if( imgid >= 42 && imgid <= 45 )
    {
      imgid = __turnBySet( imgid, 42, 4, 1, koeff[ newDirection ] );
    }
    else if( imgid >= 46 && imgid <= 49 )
    {
      imgid = __turnBySet( imgid, 46, 4, 1, koeff[ newDirection ] );
    }
    else
    {
      return -1;
    }
  }

  return imgid + 372;
}

unsigned int hash(const TilePos& pos)
{
  return (pos.i() << 16) + pos.j();
}

Point tilepos2screen(const TilePos& pos)
{
  return Point( tilemap::x_tileBase * (pos.j()+pos.i()),
                tilemap::y_tileBase * (pos.j()-pos.i()) );
}

TilePos screen2tilepos( const Point& point, int mapsize )
{
  return TilePos( 0, 0 );
}

void decode(Tile& tile, const int bitset)
{
  tile.setFlag( Tile::clearAll, true );

  if(bitset & 0x00001) { tile.setFlag( Tile::tlTree, true);      }
  if(bitset & 0x00002) { tile.setFlag( Tile::tlRock, true);      }
  if(bitset & 0x00004) { tile.setFlag( Tile::tlWater, true);     }
  //if(bitset & 0x8)   { tile.setFlag( Tile::tlOverlay, true);  }
  if(bitset & 0x00010) { tile.setFlag( Tile::tlTree, true);      }
  if(bitset & 0x00020) { tile.setFlag( Tile::tlGarden, true);    }
  if(bitset & 0x00040) { tile.setFlag( Tile::tlRoad, true);      }
  if(bitset & 0x00100) { tile.setFlag( Tile::tlCoast, true);     }
  if(bitset & 0x00200) { tile.setFlag( Tile::tlElevation, true); }
  if(bitset & 0x00400) { tile.setFlag( Tile::tlRock, true );     }
  if(bitset & 0x00800) { tile.setFlag( Tile::tlMeadow, true);    }
  if(bitset & 0x01000) { tile.setFlag( Tile::tlRubble, true);    }
  if(bitset & 0x04000) { tile.setFlag( Tile::tlWall, true);      }
  if(bitset & 0x08000) { tile.setFlag( Tile::tlDeepWater, true); }
  if(bitset & 0x10000) { tile.setFlag( Tile::tlRift, true);      }
}

Tile& getInvalid()
{
  static Tile invalidTile( TilePos( -1, -1) );
  return invalidTile;
}

void clear(Tile& tile)
{
  int startOffset  = ( (math::random( 10 ) > 6) ? 62 : 232 );
  int imgId = math::random( 58 );

  Picture pic = Picture::load( ResourceGroup::land1a, startOffset + imgId );
  tile.setPicture( ResourceGroup::land1a, startOffset + imgId );
  tile.setOriginalImgId( imgid::fromResource( pic.name() ) );
}

void fixPlateauFlags(Tile& tile)
{
  if( tile.originalImgId() > 200 && tile.originalImgId() < 245 )
  {
    tile.setFlag( Tile::clearAll, true );
    Picture pic = imgid::toPicture( tile.originalImgId() );
    int size = (pic.width() + 2) / 60;
    bool flat = pic.height() <= 30 * size;
    tile.setFlag( Tile::tlRock, !flat );
  }
}

}//end namespace util

}//end namespace gfx
