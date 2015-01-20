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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "startmenu.hpp"

#include <iostream>
#include <cmath>
#include <iterator>

#include "game/resourcegroup.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "core/variant_map.hpp"
#include "core/foreach.hpp"
#include "core/saveadapter.hpp"

namespace gui
{

class StartMenu::Impl
{
public:
  VariantMap options;
};

StartMenu::StartMenu( Widget* parent ) 
  : Widget( parent, -1, parent->relativeRect() ), _d( new Impl )
{
  std::string path2options;
#ifdef CAESARIA_PLATFORM_ANDROID
  path2options = ":/gui/startmenu_android.gui";
#else
  path2options = ":/gui/startmenu.gui";
#endif

  _d->options = config::load( path2options );
}

StartMenu::~StartMenu() {}

void StartMenu::draw(gfx::Engine &painter)
{
  Widget::draw( painter );
}

PushButton* StartMenu::addButton( const std::string& caption, int id )
{
  Size buttonSize = _d->options.get( "buttonSize", Size( 200, 25 ) ).toSize();
  Font btnFont = Font::create( _d->options.get( "buttonFont", Variant( "FONT_2" ) ).toString() );
  std::string style = _d->options.get( "buttonStyle" ).toString();
  int offsetY = _d->options.get( "buttonOffset", 40 );

  PushButton* newButton = new PushButton( this, Rect( Point( 0, 0 ), buttonSize ), caption, id, false );
  newButton->setBackgroundStyle( style );
  newButton->setFont( btnFont );

  List< PushButton* > buttons = findChildren< PushButton* >();
  Point offsetBtn( ( width() - buttonSize.width() ) / 2, ( height() - offsetY * buttons.size() ) / 2 );

  foreach( btn, buttons )
  {
    (*btn)->setPosition( offsetBtn );
    offsetBtn += Point( 0, offsetY );
  }

  return newButton;
}

void StartMenu::clear()
{
  Widgets rchildren = children();
  foreach( it, rchildren )
    (*it)->remove();
}

}//end namespace gui
