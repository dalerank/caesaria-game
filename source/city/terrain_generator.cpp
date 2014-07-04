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

#include "terrain_generator.hpp"
#include "gfx/tilemap.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "core/direction.hpp"

using namespace gfx;

MidpointDisplacement::MidpointDisplacement(int n, int wmult, int hmult, float smoothness, float fineness) {
  n_ = n;
  wmult_ = wmult;
  hmult_ = hmult;
  smoothness_ = smoothness;
  fineness_ = fineness;
  grass_threshold_ = 1.25;
  water_threshold_ = 0.55;
  sand_threshold_ = 0.70;
  deep_water_threshold_ = 0.0001;
  hills_threshold_ = 2.60;
  shallow_mountains_threshold_ = 2.92;
  high_mountains_threshold_ = 0;
  random_ = Random();
}

int MidpointDisplacement::CoordinatesToVectorIndex(int x, int y) {
  int i = 0;
  i = x + y * width_;
  return i;
}
std::pair<int, int> MidpointDisplacement::VectorIndexToCoordinates(int i) {
  int x = 0;
  int y = 0;
  y = i / width_;
  x = i % width_;
  return std::make_pair(x, y);
}

std::vector<int> MidpointDisplacement::map() {
  int power = pow(2, n_);
  int width = wmult_ * power + 1;
  int height = hmult_ * power + 1;

  width_ = width;
  height_ = height;

  std::vector<float> map;
  std::vector<int> return_map;

  map.resize(width_ * height_);
  return_map.resize(width_ * height_);

  int step = power / 2;
  float sum;
  int count;

  float h = fineness_;

  for (int i = 0; i < width_; i += 2 * step) {
    for (int j = 0; j < height_; j+= 2 * step) {
      map.at(CoordinatesToVectorIndex(i, j)) = random_.Float(0, 2 * h); //???
    }
  }

  for (int i = 0; i < width_; i++) {
    map.at(CoordinatesToVectorIndex(i, 0)) = random_.Float(-3, -2);
    map.at(CoordinatesToVectorIndex(i, height_ - 1)) = random_.Float(-2, -1);
  }

  for (int i = 0; i < height_; i++) {
    map.at(CoordinatesToVectorIndex(0, i)) = random_.Float(-3, -2);
    map.at(CoordinatesToVectorIndex(width_ - 1, i)) = random_.Float(-3, -2);
  }

  map.at(CoordinatesToVectorIndex((width_ - 1) / 2, (height_ - 1) / 2)) = (2 * h) + random_.Float(1, 5);
  /*map.at(CoordinatesToVectorIndex(0, 0)) = -3;
map.at(CoordinatesToVectorIndex((width_ - 1), 0)) = -3;
map.at(CoordinatesToVectorIndex(0, (height_ - 1))) = -3;
map.at(CoordinatesToVectorIndex((width_ - 1), (height_ - 1))) = -3;
map.at(CoordinatesToVectorIndex((width_ - 1) / 2, (height_ - 1) / 2)) = 3;*/

  while(step > 0) {

    //Diamond
    for (int x = step; x < width_; x += 2 * step) {
      for (int y = step; y < height_; y += 2 * step) {
        sum = map.at(CoordinatesToVectorIndex(x - step, y - step)) + map.at(CoordinatesToVectorIndex(x - step, y + step)) + map.at(CoordinatesToVectorIndex(x + step, y - step)) + map.at(CoordinatesToVectorIndex(x + step, y + step));
        map.at(CoordinatesToVectorIndex(x, y)) = sum / 4 + random_.Float(-h, h); //???
      }
    }

    //Square
    for (int x = 0; x < width_; x += step) {
      for (int y = step * (1 - (x / step) % 2); y < height_; y += 2 * step) {
        sum = 0;
        count = 0;
        if(x - step >= 0) {
          sum += map.at(CoordinatesToVectorIndex(x - step, y));
          count++;
        }
        if(x + step < width) {
          sum += map.at(CoordinatesToVectorIndex(x + step, y));
          count++;
        }
        if(y - step >= 0) {
          sum += map.at(CoordinatesToVectorIndex(x, y - step));
          count++;
        }
        if(y + step < height_) {
          sum += map.at(CoordinatesToVectorIndex(x, y + step));
          count++;
        }
        if(count > 0) {
          map.at(CoordinatesToVectorIndex(x, y)) = sum / count + random_.Float(-h, h); //???
        } else {
          map.at(CoordinatesToVectorIndex(x, y)) = 0;
        }
      }
    }
    h /= smoothness_;
    step /= 2;
  }

  float max = std::numeric_limits<float>::max();
  float min = std::numeric_limits<float>::min();

  for (unsigned int i = 0; i < map.size(); i++) {
    if(map.at(i) > max) {
      map.at(i) = max;
    }
    if(map.at(i) < min) {
      map.at(i) = min;
    }
  }

  for (unsigned int i = 0; i < map.size(); i++) {
    //std::cout << map.at(i);
    //map.at(i) = (map.at(i) - min) / (max - min);
    //std::cout << map.at(i);
    float value = map.at(i);
    int new_value = unknown;
    if(value < deep_water_threshold_) {
      new_value = deepWater;
    } else if (value < water_threshold_) {
      new_value = water;
    } else if (value < sand_threshold_) {
      new_value = coast;
    } else if (value < grass_threshold_) {
      new_value = grass;
    } else if (value < hills_threshold_) {
      new_value = trees;
    } else if (value < shallow_mountains_threshold_) {
      new_value = shallowMountain;
    } else if (value < high_mountains_threshold_) {
      new_value = highMountain;
    } else {
      new_value = highMountain;
    }
   // new_value = (value * 255) + 255;
    return_map.at(i) = new_value;
  }
  return return_map;
}

static void __removeCorners(Game& game )
{
  PlayerCityPtr oCity = game.city();
  Tilemap& oTilemap = oCity->tilemap();
  unsigned int mapSize = oTilemap.size();

  TilePos psAr[8] = { TilePos(0, 1), TilePos(-1, 1), TilePos(-1, 0), TilePos(-1, -1),
                      TilePos(0, -1), TilePos(1, -1), TilePos(1, 0), TilePos(1, 1)};
  //check one water tiles
  for( unsigned int i=0; i < mapSize; i++ )
    for( unsigned int j=0; j < mapSize; j++ )
    {
      TilePos tpos = TilePos( i, j );
      Tile& wtile = oTilemap.at( tpos );
      if( wtile.originalImgId() == 0 ||
          wtile.getFlag( Tile::tlWater ) || wtile.getFlag( Tile::tlDeepWater ) )
        continue;

      for( int k=0; k < 8; k++ )
      {
        Tile& t = oTilemap.at( tpos + psAr[k] );
        bool isWater = ( t.getFlag( Tile::tlWater ) || t.getFlag( Tile::tlDeepWater ) );
        if( isWater )
        {
          wtile.setFlag( Tile::clearAll, true );
          wtile.setOriginalImgId( 0 );
          wtile.setPicture( Picture() );
          break;
        }
      }
    }
}

static void __finalizeMap(Game& game, int pass )
{
  PlayerCityPtr oCity = game.city();
  Tilemap& oTilemap = oCity->tilemap();
  unsigned int mapSize = oTilemap.size();

  TilePos psAr[8] = { TilePos(0, 1), TilePos(-1, 1), TilePos(-1, 0), TilePos(-1, -1),
                      TilePos(0, -1), TilePos(1, -1), TilePos(1, 0), TilePos(1, 1)};
  int directions[8] = { 0x1, 0x80, 0x8, 0x40, 0x4, 0x20, 0x2, 0x10 };
  //check one water tiles
  for( unsigned int i=0; i < mapSize; i++ )
    for( unsigned int j=0; j < mapSize; j++ )
    {
      TilePos tpos = TilePos( i, j );
      Tile& wtile = oTilemap.at( tpos );
      if( wtile.originalImgId() > 0 )
        continue;

      int direction = 0;
      for( int k=0; k < 8; k++ )
      {
        Tile& t = oTilemap.at( tpos + psAr[k] );
        bool isWater = !t.getFlag( Tile::tlCoast ) && ( t.getFlag( Tile::tlWater ) || t.getFlag( Tile::tlDeepWater ) );
        if( isWater )
        {
          direction += directions[k];
        }
      }


      int start=0, rnd=4;
      switch( pass ) {
      case 1:
        switch( direction )
        {
        case 0x1: case 0x81: case 0x91: case 0x11: case 0x90: start=128; break;
        }
      break;

      case 2:
        switch( direction )
        {
        case 0x2: case 0x12: case 0x30: case 0x32: case 0x22: start=132; break;
        }
      break;

      case 3:
        switch( direction )
        {
        case 0x4: case 0x44: case 0x24: case 0x64: case 0x60: start=136; break;
        }
      break;

      case 4:
        switch( direction )
        {
        case 0x8: case 0xc8: case 0x48: case 0x88: case 0xc0: start=140; break;
        }
      break;

      case 5:
        switch( direction )
        {
        case 0x26: case 0x66: case 0x36: case 0x76: case 0x72: case 0x52: case 0x74:
        case 0x34: case 0x16: case 0x42: case 0x46: case 0x14: case 0x54: case 0x56:
        case 0x06: start = 171; rnd=0; break;

        case 0xcc: case 0x4e: case 0x4c: case 0x62: case 0x6c: case 0xec:
        case 0xc4: case 0x68: case 0xe4: case 0xe8: case 0xa8: case 0x84:
        case 0x28: case 0xac: case 0xa4: case 0x0c: case 0x8c: case 0x2c:
          start = 172; rnd=0; break;

        case 0xc9: case 0x89: case 0x99: case 0x98: case 0xd9: case 0x58: case 0x19:
        case 0xd8: case 0xd1: case 0x49: case 0x18: case 0xc1: case 0x41:
        case 0x51: case 0x09:  start=173; rnd=0; break;

        case 0x13: case 0xb1: case 0x23: case 0x93: case 0x33: case 0xb3: case 0x83:
        case 0x31: case 0xa3: case 0x21: case 0x82: case 0xa1: case 0xa2: case 0x92:
        case 0xb2: case 0x03: case 0xd0: start=170; rnd=0; break;

        case 0xa0: start=168; rnd=0; break;
        case 0x50: start=169; rnd=0; break;
        }
      break;

      case 6:
        switch( direction )
        {
        case 0x10: start=144; break;
        case 0x20: start=148; break;
        case 0x40: start=152; break;
        case 0x80: start=156; break;
        }
      break;

      case 7:
        switch( direction )
        {
        case 0x6e: case 0xed: case 0x9b: case 0xcd: case 0xff: case 0xee:
        case 0xdf: case 0x05: case 0x37: case 0x77: case 0xfb: case 0x9a:
        case 0xeb: case 0xa5: case 0xf9: case 0xb7: case 0xbf: case 0x1b:
        case 0x35: case 0xe5: case 0x95: case 0x17: case 0x7d: case 0xe7:
        case 0xad: case 0x71: case 0xdc: case 0xdb: case 0xa6: case 0xb9:
        case 0xa7: case 0x27: case 0xdd: case 0xf3: case 0x7e: case 0x75:
        case 0xd3: case 0xc5: case 0x55: case 0xfe: case 0xbb: case 0x7c:
        case 0x5a: case 0x53: case 0x45: case 0xce: case 0xb6: case 0xef:
        case 0x7f: case 0x8d: case 0x1d: case 0xf5: case 0xf7: case 0xf1:
        case 0x97: case 0x2e: case 0xc3: case 0x5c: case 0x0a: case 0x69:
        case 0xb5: case 0x25: case 0x8a: case 0x3b: case 0xe6: case 0x6a:
        case 0xcb: case 0x3f: case 0xaa: case 0x57: case 0xfd: case 0xd5:
        case 0x7a: case 0x85: case 0xfc: case 0xca: case 0x8b:  start=120; rnd=0; break;
        }
      break;

      case 8:
        switch( direction )
        {
        case 0:
        {
          Picture pic = Picture::load( ResourceGroup::land1a, 62 + math::random( 57 ) );
          wtile.setPicture( pic );
          //wtile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
          wtile.setOriginalImgId( direction );
        }
        break;
        }
      break;

      case 0xff:
        wtile.setOriginalImgId( direction );
      break;
      }

      if( start > 0 )
      {
        if( rnd > 0 )
          rnd = math::random( 4 );

        wtile.setFlag( Tile::tlWater, true );
        wtile.setFlag( Tile::tlCoast, true );
        Picture pic = Picture::load( ResourceGroup::land1a, start + rnd );
        wtile.setPicture( pic );
        wtile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
    }
}

void TerrainGenerator::create(Game& game)
{
  MidpointDisplacement diamond_square = MidpointDisplacement(5, 8, 8, 3, 1.2);
  std::vector<int> map = diamond_square.map();

  PlayerCityPtr oCity = game.city();
  Tilemap& oTilemap = oCity->tilemap();
  unsigned int mapSize = diamond_square.width();
  oTilemap.resize( mapSize );

  Logger::warning( "W:%d H:%d", diamond_square.width(), diamond_square.height() );

  for( unsigned int index = 0; index < map.size(); index++)
  {
    int j = index / (diamond_square.width());
    int i = index % (diamond_square.width());

    Tile& tile = oTilemap.at(i, j);
    //tile.setPicture( TileHelper::convId2PicName( pGraphicGrid.data()[index] ) );
    //tile.setOriginalImgId( pGraphicGrid.data()[index] );

    //TileHelper::decode( tile, pTerrainGrid.data()[index] );
    NColor color;
    tile.setFlag( Tile::clearAll, true );

    switch( map.at(index) )
    {
      case MidpointDisplacement::unknown:
      {
        color = NColor( 255, 255, 0, 255);
        tile.setFlag( Tile::tlDeepWater, true );
        Picture pic = Picture::load( ResourceGroup::land1a, 120 );
        tile.setPicture( pic );
        tile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
      break;

      case MidpointDisplacement::deepWater:
      {
        color = NColor( 255, 7, 93, 192);
        tile.setFlag( Tile::tlDeepWater, true );
        Picture pic = Picture::load( ResourceGroup::land1a, 120 );
        tile.setPicture( pic );
        tile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );

      }
      break;

      case MidpointDisplacement::water:
      {
        color = NColor( 255, 74, 157, 251);
        tile.setFlag( Tile::tlWater, true );
        Picture pic = Picture::load( ResourceGroup::land1a, 120 );
        tile.setPicture( pic );
        tile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
      break;

      case MidpointDisplacement::coast: {
        color = NColor( 255, 255, 238, 178);
      }
      break;

      case MidpointDisplacement::grass:
      {
        color = NColor( 255, 7, 192, 53);
        Picture pic;
        /*int rnd3 = math::random( 1 );
        switch( rnd3 )
        {
        case 0: pic = ;
        case 1:
        }*/
        pic = Picture::load( ResourceGroup::land1a, 62 + math::random( 57 ) );
        tile.setPicture( pic );
        tile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
      break;

      case MidpointDisplacement::trees: {
        color = NColor( 255, 32, 139, 58);
        Picture pic = Picture::load( ResourceGroup::land1a, 30 + math::random( 31 ) );
        tile.setFlag( Tile::tlTree, true );
        tile.setPicture( pic );
        tile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
      break;

      case MidpointDisplacement::shallowMountain: {
        color = NColor( 255, 147, 188, 157 );
        Picture pic = Picture::load( ResourceGroup::land1a, 290 + math::random( 7 ) );
        tile.setFlag( Tile::tlRock, true );
        tile.setPicture( pic );
        tile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
      break;

      case MidpointDisplacement::highMountain: {
        color = NColor( 255, 129, 141, 132);
        Picture pic = Picture::load( ResourceGroup::land1a, 230 + math::random( 59 ) );
        //tile.setFlag( Tile::tlRock, true );
        tile.setPicture( pic );
        tile.setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
      break;

      case 8: {
        color = DefaultColors::white;
      }
      break;
    }
  }

  __removeCorners( game );

  __finalizeMap( game, 1 );
  __finalizeMap( game, 2 );
  __finalizeMap( game, 3 );
  __finalizeMap( game, 4 );
  __finalizeMap( game, 5 );
  __finalizeMap( game, 6 );
  __finalizeMap( game, 7 );
  __finalizeMap( game, 8 );
  __finalizeMap( game, 0xff );
}
