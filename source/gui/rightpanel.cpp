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
#include "gfx/decorator.hpp"

using namespace gfx;

namespace gui
{

class MenuRigthPanel::Impl
{
public:
  Batch background;
  Pictures backgroundNb;
};

MenuRigthPanel::MenuRigthPanel( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 100, 100 ) ), _d( new Impl )
{
}

void MenuRigthPanel::_initBackground(const Picture &tilePic)
{
  _d->background.destroy();

  unsigned int y = 0;

  Pictures pics;
  while( y < height() )
  {
    pics.append( tilePic, Point( 0, y ) );
    y += tilePic.height();
  }

  bool batchOk = _d->background.load( pics, lefttop() );
  if( !batchOk )
  {
    _d->background.destroy();
    Decorator::reverseYoffset( pics );
    _d->backgroundNb = pics;
  }
}

void MenuRigthPanel::draw( gfx::Engine& engine )
{
  if( !visible() )
    return;

  if( _d->background.valid() )
    engine.draw( _d->background, &absoluteClippingRectRef() );
  else
    engine.draw( _d->backgroundNb, absoluteRect().lefttop(), &absoluteClippingRectRef() );
}

MenuRigthPanel* MenuRigthPanel::create( Widget* parent, const Rect& rectangle, const Picture& tilePic )
{
  MenuRigthPanel* ret = new MenuRigthPanel( parent );

  if( tilePic.height() == 0 )
    return ret;

  ret->setGeometry( rectangle );
  ret->_initBackground( tilePic );

  return ret;
}

}//end namespace gui
