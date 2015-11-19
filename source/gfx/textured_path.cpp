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

#include "textured_path.hpp"
#include "pathway/pathway.hpp"
#include "walker/walker.hpp"
#include "city/city.hpp"
#include "gfx/tilemap_config.hpp"
#include "gfx/decorator.hpp"
#include "gfx/tilesarray.hpp"
#include "gfx/camera.hpp"
#include "core/color_list.hpp"
#include "gfx/picturesarray.hpp"

namespace gfx
{

namespace {
struct TexturedPathConfig
{
  std::map<NColor,unsigned int> indexes = { {ColorList::red,0},
                                            {ColorList::blue,20},
                                            {ColorList::grey,40},
                                            {ColorList::green,60},
                                            {ColorList::violet,80},
                                            {ColorList::lightSlateBlue,100},
                                            {ColorList::orange,120},
                                            {ColorList::pink,140},
                                            {ColorList::lightSlateGray,160},
                                            {ColorList::yellow,180} };

  std::map<unsigned int,Picture> cache;
  const Picture& getpic( const NColor& color, int direction )
  {
    auto it = indexes.find( color );
    int index = (it != indexes.end() ? it->second : 0) + direction;

    auto itPic = cache.find( index );
    if( itPic == cache.end() )
    {
      cache[ index ] = Picture( "way", index );
      return cache[ index ];
    }
    else
      return itPic->second;
  }
};

}

void TexturedPath::draw(const Pathway& way, const RenderInfo& rinfo, NColor color, const Point& doff )
{
  const TilesArray& tiles = way.allTiles();
  draw( tiles, rinfo, color, doff );
}

void TexturedPath::draw(const TilesArray& tiles, const RenderInfo& rinfo, NColor color, const Point& doff)
{
  static TexturedPathConfig config;
  Point offset;

  const Picture& pic = config.getpic( ColorList::red, 1 );
  if( pic.width()+2 < gfx::tilemap::cellPicSize().width() )
  {
    offset = Point( (gfx::tilemap::cellPicSize().width() - pic.width()) / 2,
                    gfx::tilemap::cellPicSize().height() / 2 - pic.height() * 1.5 );
  }

  if( tiles.size() > 1 )
  {
    for( unsigned int step=0; step < tiles.size()-1; step++ )
    {
      int nextIndex = step+1;
      int prevIndex = step;
      if( step > 1)
      {
        prevIndex = step - 1;
        nextIndex = step + 1;
      }
      Direction dir = tilemap::getDirection( tiles[ prevIndex ]->epos(),
                                             tiles[ nextIndex ]->epos() );

      int index = 0;
      switch( dir )
      {
      case direction::north:
      case direction::south:
        index = 1;
      break;

      case direction::west:
      case direction::east:
        index = 2;
      break;

      case direction::northEast:
      {
        int deltax = tiles[ prevIndex ]->epos().i() - tiles[ step ]->epos().i();
        index = deltax == 0 ? 3 : 4;
      }
      break;

      case direction::southWest: index = 4; break;


      case direction::southEast:
      {
        int deltax = tiles[ prevIndex ]->epos().j() - tiles[ step ]->epos().j();
        index = deltax == 0 ? 6 : 5;
      }
      break;

      case direction::northWest:
      {
        int deltax = tiles[ prevIndex ]->epos().i() - tiles[ step ]->epos().i();
        index = deltax == 0 ? 6 : 5;
      }
      break;

      default: index = 0;
      }

      rinfo.engine.draw( config.getpic( color, index ), tiles[ step ]->mappos() + rinfo.offset + offset + doff );
    }

    int lenght = tiles.size();
    int index = 0;
    Direction dir = tilemap::getDirection( tiles[ lenght-2 ]->epos(),
                                           tiles[ lenght-1 ]->epos() );
    switch( dir )
    {
    case direction::north: index = 7;  break;
    case direction::south: index = 9;  break;
    case direction::west:  index = 10; break;
    case direction::east:  index = 8;  break;
    default: index = 0;
    }
    rinfo.engine.draw( config.getpic( color, index ), tiles.back()->mappos() + rinfo.offset + offset + doff );
  }
}

}//end namespace gfx
