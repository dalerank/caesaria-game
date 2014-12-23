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

using namespace gfx;

namespace gui
{

class MenuRigthPanel::Impl
{
public:
  Pictures background;
};

MenuRigthPanel::MenuRigthPanel( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 100, 100 ) ), _d( new Impl )
{
}

void MenuRigthPanel::draw( gfx::Engine& engine )
{
  if( !visible() )
    return;

  engine.draw( _d->background, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
}

MenuRigthPanel* MenuRigthPanel::create( Widget* parent, const Rect& rectangle, Picture& tilePic )
{
  MenuRigthPanel* ret = new MenuRigthPanel( parent );

  ret->setGeometry( rectangle );
  ret->_d->background.clear();

  unsigned int y = 0;
  if( tilePic.height() == 0 )
    return ret;

  while( y < ret->height() )
  {
    ret->_d->background.append( tilePic, Point( 0, -y ) );
    y += tilePic.height();
  }

  return ret;
}

}//end namespace gui
