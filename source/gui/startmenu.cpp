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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "startmenu.hpp"

#include <iostream>
#include <cmath>
#include <iterator>

#include "core/exception.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "objects/building.hpp"
#include "objects/metadata.hpp"
#include "city/city.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "core/foreach.hpp"

namespace gui
{

StartMenu::StartMenu( Widget* parent ) 
	: Widget( parent, -1, parent->getRelativeRect() )
{
}

StartMenu::~StartMenu()
{
}

PushButton* StartMenu::addButton( const std::string& caption, int id )
{
    Size btnSize( 200, 25 );
    int offsetY = 40;

    PushButton* btn = new PushButton( this, Rect( Point( 0, 0 ), btnSize ), caption, id ); 

    Point offsetBtn( ( width() - btnSize.width() ) / 2, ( height() - offsetY * 5 ) / 2 );
    List< PushButton* > buttons = findChildren< PushButton* >(); 
    foreach( btn, buttons )
    {
      (*btn)->setPosition( offsetBtn );
      offsetBtn += Point( 0, offsetY );
    }

    return btn;
}

}//end namespace gui
