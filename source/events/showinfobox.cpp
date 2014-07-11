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
#include "gui/event_messagebox.hpp"
#include "good/goodhelper.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace {
CAESARIA_LITERALCONST(title)
CAESARIA_LITERALCONST(text)
CAESARIA_LITERALCONST(video)
CAESARIA_LITERALCONST(good)
CAESARIA_LITERALCONST(position)
}

namespace events
{

class ShowInfobox::Impl
{
public:
  std::string title, text;
  bool send2scribe;
  vfs::Path video;
  Point position;
  Good::Type gtype;
};

GameEventPtr ShowInfobox::create()
{
  GameEventPtr ret( new ShowInfobox() );
  ret->drop();
  return ret;
}

GameEventPtr ShowInfobox::create(const std::string& title, const std::string& text, Good::Type type, bool send2scribe)
{
  ShowInfobox* ev = new ShowInfobox();
  ev->_d->title = title;
  ev->_d->text = text;
  ev->_d->gtype = type;
  ev->_d->send2scribe = send2scribe;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

GameEventPtr ShowInfobox::create(const std::string& title, const std::string& text, bool send2scribe, const vfs::Path &video)
{
  ShowInfobox* ev = new ShowInfobox();
  ev->_d->title = title;
  ev->_d->text = text;
  ev->_d->video = video;
  ev->_d->gtype = Good::none;
  ev->_d->send2scribe = send2scribe;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

void ShowInfobox::load(const VariantMap& stream)
{
  _d->title = stream.get( lc_title ).toString();
  _d->text = stream.get( lc_text ).toString();
  _d->gtype = GoodHelper::getType( stream.get( lc_good ).toString() );
  _d->position = stream.get( lc_position ).toPoint();
  _d->video = stream.get( lc_video ).toString();
  VARIANT_LOAD_ANY_D(_d,send2scribe,stream);
}

VariantMap ShowInfobox::save() const
{
  return VariantMap();
}

bool ShowInfobox::_mayExec(Game& game, unsigned int time) const{  return true;}

ShowInfobox::ShowInfobox() : _d( new Impl )
{

}

void ShowInfobox::_exec( Game& game, unsigned int )
{
  if( _d->video.toString().empty() )
  {
    gui::EventMessageBox* msgWnd = new gui::EventMessageBox( game.gui()->rootWidget(), _d->title, _d->text,
                                                             GameDate::current(), _d->gtype );
    msgWnd->show();
  }
  else
  {
    gui::FilmWidget* wnd = new gui::FilmWidget( game.gui()->rootWidget(), _d->video );
    wnd->setTitle( _d->title );
    wnd->setText( _d->text );
    wnd->show();
  }

  if( _d->send2scribe )
  {
    GameEventPtr e = ScribeMessage::create( _d->title, _d->text, _d->gtype, _d->position );
    e->dispatch();
  }
}

} //end namespace events
