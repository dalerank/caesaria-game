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
#include "core/gettext.hpp"
#include "core/variant_map.hpp"
#include "gui/film_widget.hpp"
#include "gui/event_messagebox.hpp"
#include "good/helper.hpp"
#include "game/gamedate.hpp"
#include "factory.hpp"

namespace events
{

REGISTER_EVENT_IN_FACTORY(ShowInfobox, "messagebox")

class ShowInfobox::Impl
{
public:
  std::string title, text, tip;
  bool send2scribe;
  std::string video;
  Point position;
  good::Product gtype;
  bool hidden;

  std::map<std::string, Callback> callbacks;
};

GameEventPtr ShowInfobox::create()
{
  GameEventPtr ret( new ShowInfobox() );
  ret->drop();
  return ret;
}

GameEventPtr ShowInfobox::create(const std::string& title, const std::string& text, good::Product type, bool send2scribe)
{
  auto ev = new ShowInfobox();
  ev->_d->title = title;
  ev->_d->text = text;
  ev->_d->gtype = type;
  ev->_d->send2scribe = send2scribe;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

GameEventPtr ShowInfobox::create(const std::string& title, const std::string& text, bool send2scribe, const std::string& video)
{
  auto ev = new ShowInfobox();
  ev->_d->title = title;
  ev->_d->text = text;
  ev->_d->video = video;
  ev->_d->gtype = good::none;
  ev->_d->send2scribe = send2scribe;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

void ShowInfobox::load(const VariantMap& stream)
{
  VARIANT_LOAD_STR_D( _d, title, stream )
  VARIANT_LOAD_STR_D( _d, text, stream )
  VARIANT_LOAD_ANY_D( _d, position, stream )
  VARIANT_LOAD_STR_D( _d, video, stream )
  VARIANT_LOAD_ANY_D( _d, send2scribe, stream)
  VARIANT_LOAD_ANY_D( _d, hidden, stream )
  VARIANT_LOAD_STR_D( _d, tip, stream )

  _d->gtype = good::Helper::getType( stream.get( "good" ).toString() );
}

VariantMap ShowInfobox::save() const
{
  return VariantMap();
}

void ShowInfobox::setDialogVisible( bool visible ) { _d->hidden = visible; }

void ShowInfobox::addCallback(const std::string& text, Callback callback)
{
  _d->callbacks[ text] = callback;
}

bool ShowInfobox::_mayExec(Game& game, unsigned int time) const{  return true;}

ShowInfobox::ShowInfobox() : _d( new Impl )
{
  _d->hidden = false;
}

void ShowInfobox::_exec( Game& game, unsigned int )
{
  if( !_d->hidden )
  {
    if( _d->video.empty() )
    {
      auto& wnd = game.gui()->add<gui::infobox::AboutEvent>( _(_d->title), _(_d->text),
                                                            game::Date::current(), _d->gtype, _d->tip );

      for( auto& callback : _d->callbacks )
        wnd.addCallback( callback.first, callback.second );

      wnd.show();
    }
    else
    {
      auto& wnd = game.gui()->add<gui::FilmWidget>( _d->video );
      wnd.setTitle( _( _d->title ) );
      wnd.setText( _( _d->text ) );
      wnd.show();
    }
  }

  if( _d->send2scribe )
  {
    events::dispatch<ScribeMessage>( _(_d->title), _(_d->text), _d->gtype, _d->position );
  }
}

} //end namespace events
