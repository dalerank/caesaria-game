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


#include "screen_menu.hpp"

#include <iostream>

#include "gfx_engine.hpp"
#include "exception.hpp"
#include "pic_loader.hpp"

#include "oc3_startmenu.h"
#include "oc3_guienv.h"
#include "oc3_pushbutton.h"

class ScreenMenu::Impl
{
public:
    Picture* bgPicture;
    StartMenu* menu;         // menu to display
    GfxEngine* engine;
    GuiEnv* gui;
    int result;
    bool isStoped;

    void resolveNewGame() { result=startNewGame; isStoped=true; }
    void resolveLoadGame() { result=loadSavedGame; isStoped=true; }
    void resolveQuitGame() { result=closeApplication; isStoped=true; }
};

ScreenMenu::ScreenMenu() : _d( new Impl )
{
    _d->bgPicture = NULL;
    _d->isStoped = false;
}

ScreenMenu::~ScreenMenu() {}

void ScreenMenu::draw()
{
	_d->gui->beforeDraw();

    _d->engine->drawPicture(*_d->bgPicture, 0, 0);
    _d->gui->draw();
}

void ScreenMenu::handleEvent( NEvent& event )
{
    _d->gui->handleEvent( event );
}

void ScreenMenu::initialize( GfxEngine& engine, GuiEnv& gui )
{
    _d->bgPicture = &PicLoader::instance().get_picture("title", 1);

    // center the bgPicture on the screen
    _d->bgPicture->set_offset( (engine.getScreenWidth() - _d->bgPicture->get_width()) / 2,
                               -( engine.getScreenHeight() - _d->bgPicture->get_height() ) / 2 );

    _d->gui = &gui;
    _d->engine = &engine;
    _d->menu = new StartMenu( gui.getRootWidget() );

    PushButton* btn = _d->menu->addButton( "New game", -1 );
    CONNECT( btn, onClicked(), _d.get(), Impl::resolveNewGame );

    btn = _d->menu->addButton( "Load game", -1 );
    CONNECT( btn, onClicked(), _d.get(), Impl::resolveLoadGame );

    btn = _d->menu->addButton( "Quit", -1 );
    CONNECT( btn, onClicked(), _d.get(), Impl::resolveQuitGame );
}

int ScreenMenu::getResult() const
{
	return _d->result;
}

bool ScreenMenu::isStopped() const
{
    return _d->isStoped;
}
/*void ScreenMenu::handleWidgetEvent(const WidgetEvent &event, Widget *widget)
{
   _wevent = event;

   switch (event._eventType)
   {
   case WE_NewGame:
      stop();
      break;
   case WE_LoadGame:
      stop();
      break;
   case WE_QuitGame:
      stop();
      break;
   default:
      THROW("Unexpected widget event: " << event._eventType);
   }
}*/

