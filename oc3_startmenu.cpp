// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#include "oc3_startmenu.h"

#include <iostream>
#include <cmath>
#include <iterator>

#include "exception.hpp"
#include "pic_loader.hpp"
#include "sdl_facade.hpp"
#include "gui_paneling.hpp"
#include "gettext.hpp"
#include "building.hpp"
#include "building_data.hpp"
#include "city.hpp"
#include "scenario.hpp"
#include "oc3_positioni.h"
#include "oc3_resourcegroup.h"
#include "oc3_pushbutton.h"
#include "oc3_label.h"

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

    Point offsetBtn( ( getWidth() - btnSize.getWidth() ) / 2, ( getHeight() - offsetY * 3 ) / 2 );
    List< PushButton* > buttons = findChildren< PushButton* >(); 
    for( List< PushButton* >::iterator it=buttons.begin(); it != buttons.end(); it++ )
    {
        (*it)->setPosition( offsetBtn );
        offsetBtn += Point( 0, offsetY );
    }

    return btn;
}
// InGameMenu::InGameMenu()
// {
//    _isDeleted = false;
//    _hoverButton = NULL;
// }
// 
// void InGameMenu::init()
// {
//    _optionsButton.setPosition(0, _height);
//    _optionsButton.setText(_("Options"));
//    _optionsButton.setEvent(WidgetEvent::OptionsMenuEvent());
//    add_widget(_optionsButton);
//    _height += 30;
// 
//    _saveGameButton.setPosition(0, _height);
//    _saveGameButton.setText(_("Save Game"));
//    _saveGameButton.setEvent(WidgetEvent::SaveGameEvent());
//    add_widget(_saveGameButton);
//    _height += 30;
// 
//    _quitGameButton.setPosition(0, _height);
//    _quitGameButton.setText(_("Quit Game"));
//    _quitGameButton.setEvent(WidgetEvent::QuitGameEvent());
//    add_widget(_quitGameButton);
//    _height += 30;
// 
//    // compute the necessary width
//    SdlFacade &sdlFacade = SdlFacade::instance();
//    int max_text_width = 0;
//    int text_width;
//    int text_height;
//    std::stringstream cost_ss;
//    Font &font = FontCollection::instance().getFont(FONT_2);
//    for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
//    {
//       Widget *widget = *itWidget;
//       BuildButton *button = (BuildButton*) widget;
//       sdlFacade.getTextSize(font, button->getText(), text_width, text_height);
//       max_text_width = std::max(max_text_width, text_width);
//    }
// 
//    _width = std::max(200, max_text_width + 20);
// 
//    // set the same size for all buttons
//    for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
//    {
//       Widget *widget = *itWidget;
//       TextButton *button = (TextButton*) widget;
//       button->setSize(_width, 25);
//       button->init_pictures();
//    }
// 
// }
// 
// 
// InGameMenu::~InGameMenu()
// {
// }
// 
// 
// void InGameMenu::draw(const int dx, const int dy)
// {
//    drawChildren(dx, dy);
// }
// 
// bool InGameMenu::isDeleted() const
// {
//    return _isDeleted;
// }
// 
// void InGameMenu::setDeleted()
// {
//    _isDeleted = true;
// }
// 
// void InGameMenu::handleEvent(SDL_Event &event)
// {
//    if (event.type == SDL_MOUSEBUTTONDOWN)
//    {
//       // mouse click
// 
//       if (event.button.button == SDL_BUTTON_RIGHT)
//       {
//          // right click
//          _isDeleted = true;
//          return;
//       }
//    }
//    else if (event.type == SDL_KEYDOWN)
//    {
//       if (event.key.keysym.sym == SDLK_ESCAPE)
//       {
//          _isDeleted = true;
//          return;
//       }
//    }
// 
//    WidgetGroup::handleEvent(event);
// }

