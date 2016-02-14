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

#include "rightpanel.hpp"
#include "gfx/picture.hpp"
#include "gfx/engine.hpp"
#include "gfx/drawstate.hpp"
#include "gfx/decorator.hpp"

using namespace gfx;

namespace gui
{

class MenuRigthPanel::Impl
{
public:
  struct {
    Batch body;
    Pictures fallback;
  } batch;
  Picture tile;
};

MenuRigthPanel::MenuRigthPanel( Widget* parent )
  : Widget( parent, -1, Rect( 0, 0, 100, 100 ) ), _d( new Impl )
{
}

void MenuRigthPanel::_initBackground(const Picture &tilePic)
{
  _d->batch.body.destroy();
  _d->tile = tilePic;

  unsigned int y = 0;

  Pictures pics;
  while( y < height() )
  {
    pics.append( tilePic, Point( 0, y ) );
    y += tilePic.height();
  }

  bool batchOk = _d->batch.body.load( pics, lefttop() );
  if( !batchOk )
  {
    _d->batch.body.destroy();
    Decorator::reverseYoffset( pics );
    _d->batch.fallback = pics;
  }
}

void MenuRigthPanel::draw( gfx::Engine& engine )
{
  if( !visible() )
    return;

  DrawState pipe( engine, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  pipe.draw( _d->batch.body )
      .fallback( _d->batch.fallback );
}

void MenuRigthPanel::setSide(MenuRigthPanel::Side side)
{
  switch( side )
  {
  case leftSide: setPosition( {0, top() } ); break;
  case rightSide: setPosition( { static_cast<int>(parent()->width() - width()), top() } ); break;
  }

  _initBackground( _d->tile );
}

MenuRigthPanel* MenuRigthPanel::create( Widget* parent, const Picture& tilePic, int top )
{
  MenuRigthPanel& ret = parent->add<MenuRigthPanel>();
  if( tilePic.height() == 0 )
    return &ret;

  Rect rect( parent->width() - tilePic.width(), top,
             parent->width(), parent->height() );

  ret.setGeometry( rect );
  ret._initBackground( tilePic );

  return &ret;
}

}//end namespace gui
