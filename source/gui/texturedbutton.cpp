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

#include "texturedbutton.hpp"
#include "game/resourcegroup.hpp"
#include "environment.hpp"
#include "widget_factory.hpp"
#include "dictionary.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(TexturedButton)
REGISTER_CLASS_IN_WIDGETFACTORY(HelpButton)
REGISTER_CLASS_IN_WIDGETFACTORY(ExitButton)

TexturedButton::TexturedButton(Widget* parent, const Point& pos, const TexturedButton::States& states)
  : TexturedButton( parent, pos, Size(defaultSize,defaultSize), -1, states )
{

}

TexturedButton::TexturedButton(Widget *parent, const Point &pos, const Size &size, int id, const States& states)
  : PushButton( parent, Rect(pos, size), "", id, false, noBackground )
{
  changeImageSet( states );
  setTextVisible( false );
}

TexturedButton::TexturedButton(Widget *parent, const Point &pos, const Size &size, int id,
                               const std::string& resourceGroup, const States& states)
  : PushButton( parent, Rect(pos, size), "", id, false, noBackground )
{
  changeImageSet( states, resourceGroup );
  setTextVisible( false );
}

TexturedButton::TexturedButton(Widget *parent) : PushButton( parent )
{
  setTextVisible( false );
}

void TexturedButton::changeImageSet(const std::string& rc, int normal, int hover, int pressed, int disabled)
{
  changeImageSet(States(normal, hover, pressed, disabled), rc);
}

void TexturedButton::changeImageSet( const States& states, const std::string& rc )
{
  const std::string r = rc.empty() ? gui::rc.panel : rc;
  setPicture( r, states.normal, stNormal );
  setPicture( r, (states.hover == -1) ? states.normal+1 : states.hover, stHovered );
  setPicture( r, (states.pressed == -1) ? states.normal+2 : states.pressed, stPressed );
  setPicture( r, (states.disabled == -1) ? states.normal+3 : states.disabled, stDisabled );
}

HelpButton::HelpButton(Widget* parent)
  : HelpButton( parent, Point(), "", -1 )
{

}

HelpButton::HelpButton(Widget* parent, const Point& pos, const std::string& helpId, int id)
  : TexturedButton( parent, pos, Size(defaultSize,defaultSize), id, States( gui::button.help ) )
{
  _helpid = helpId;
}

void HelpButton::setupUI(const VariantMap& ui)
{
  TexturedButton::setupUI( ui );
  _helpid = ui.get( "uri" ).toString();
}

void HelpButton::setupUI(const vfs::Path & ui)
{
  TexturedButton::setupUI(ui);
}

void HelpButton::setHelpId(const std::string& id)
{
  _helpid = id;
}

void HelpButton::_btnClicked()
{
  if( !_helpid.empty() )
    ui()->add<DictionaryWindow>( _helpid  );
}

ExitButton::ExitButton(Widget* parent)
  : ExitButton( parent, Point(), -1 )
{

}

ExitButton::ExitButton(Widget* parent, const Point& pos, int id)
  : TexturedButton( parent, pos, Size(defaultSize,defaultSize), id, States( gui::button.exit ) )
{

}

void ExitButton::_btnClicked()
{
  if( parent() )
    parent()->deleteLater();
}

}//end namespace gui
