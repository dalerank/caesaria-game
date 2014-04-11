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

#include "showinfobox.hpp"
#include "gui/info_box.hpp"
#include "game/game.hpp"
#include "gui/environment.hpp"
#include "scribemessage.hpp"
#include "gui/film_widget.hpp"

using namespace constants;

namespace events
{

GameEventPtr ShowInfobox::create()
{
  GameEventPtr ret( new ShowInfobox() );
  ret->drop();
  return ret;
}

GameEventPtr ShowInfobox::create(const std::string& title, const std::string& text, bool send2scribe , const vfs::Path &video)
{
  ShowInfobox* ev = new ShowInfobox();
  ev->_title = title;
  ev->_text = text;
  ev->_video = video;
  ev->_send2scribe = send2scribe;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

void ShowInfobox::load(const VariantMap& stream)
{
  _title = stream.get( "title" ).toString();
  _text = stream.get( "text" ).toString();
  _send2scribe = stream.get( "send2scribe" );
  _video = stream.get( "video" ).toString();
}

bool ShowInfobox::_mayExec(Game& game, unsigned int time) const{  return true;}

void ShowInfobox::_exec( Game& game, unsigned int )
{
  if( _video.toString().empty() )
  {
    gui::InfoboxText* msgWnd = new gui::InfoboxText( game.gui()->rootWidget(), _title, _text );
    msgWnd->show();
  }
  else
  {
    gui::FilmWidget* wnd = new gui::FilmWidget( game.gui()->rootWidget(), _video );
    wnd->setTitle( _title );
    wnd->setText( _text );
    wnd->show();
  }

  if( _send2scribe )
  {
    GameEventPtr e = ScribeMessage::create( _title, _text );
    e->dispatch();
  }
}

} //end namespace events
