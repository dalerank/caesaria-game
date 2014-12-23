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

#include "layerinfo.hpp"
#include "camera.hpp"
#include "city/city.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;

namespace gfx
{

namespace layer
{

class Info::Impl
{
public:
  Picture footColumn;
  Picture bodyColumn;
  Picture headerColumn;

  struct ColumnInfo {
    Point pos;
    int value;
  };

  struct PictureInfo {
    Point pos;
    Picture pic;
  };

  typedef std::vector<ColumnInfo> Columns;
  typedef std::vector<PictureInfo> Pictures;

  Columns columns;
  Pictures pictures;
};

void Info::render(Engine& engine)
{
  Layer::render( engine );
}

void Info::_loadColumnPicture(int picId)
{
  _d->footColumn = Picture::load( ResourceGroup::sprites, picId + 2 );
  _d->bodyColumn = Picture::load( ResourceGroup::sprites, picId + 1 );
  _d->headerColumn = Picture::load( ResourceGroup::sprites, picId );
}

void Info::drawColumn( Engine& engine, const Point& pos, const int percent)
{
  // Column made of tree base parts and contains maximum 10 parts.
  // Header (10)
  // Body (10, max 8 pieces)
  // Foot (10)
  //
  // In original game fire colomn may be in one of 12 (?) states: none, f, f+h, f+b+h, f+2b+h, ... f+8b+h


  int clamped = math::clamp(percent, 0, 100);
  int rounded = (clamped / 10) * 10;
  // [0,  9] -> 0
  // [10,19] -> 10
  // ...
  // [80,89] -> 80
  // [90,99] -> 90
  // [100] -> 100
  // rounded == 0 -> nothing
  // rounded == 10 -> header + footer
  // rounded == 20 -> header + body + footer

  if (percent == 0)
  {
    // Nothing to draw.
    return;
  }

  engine.draw( _d->footColumn, pos + Point( 10, -21 ) );

  if(rounded > 10)
  {
    for( int offsetY=7; offsetY < rounded; offsetY += 10 )
    {
      engine.draw( _d->bodyColumn, pos - Point( -18, 8 + offsetY ) );
    }

    engine.draw(_d->headerColumn, pos - Point(-6, 25 + rounded));
  }
}

Info::~Info() {  }

void Info::beforeRender(Engine& engine)
{
  _d->columns.clear();
  _d->pictures.clear();
}

void Info::afterRender(Engine& engine)
{
  foreach( it, _d->columns )
  {
    drawColumn( engine, it->pos, it->value );
  }

  foreach( it, _d->pictures )
  {
    engine.draw( it->pic, it->pos );
  }

  Layer::afterRender( engine );
}

Info::Info( Camera& camera, PlayerCityPtr city, int columnIndex )
  : Layer( &camera, city ), _d( new Impl )
{
  _loadColumnPicture( columnIndex );
}

void Info::_addColumn(Point pos, int value)
{
  Impl::ColumnInfo info = { pos, value };
  _d->columns.push_back( info );
}

void Info::_addPicture(Point pos, const Picture& pic)
{
  Impl::PictureInfo info = { pos, pic };
  _d->pictures.push_back( info );
}

}//end namespace layer

}//end namespace gfx
