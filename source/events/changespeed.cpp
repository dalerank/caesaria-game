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

#include "changespeed.hpp"
#include "dispatcher.hpp"
#include "core/utils.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "warningmessage.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "gui/environment.hpp"
#include "gui/label.hpp"
#include "gfx/helper.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

static const int windowGamePausedId = utils::hash( "gamepause" );

GameEventPtr Pause::create( Mode mode )
{
  Pause* e = new Pause();
  e->_mode = mode;

  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

bool Pause::_mayExec(Game& game, unsigned int) const{  return true;}

Pause::Pause() : _mode( unknown ) {}

void Pause::_exec(Game& game, unsigned int)
{
  gui::Widget* rootWidget = game.gui()->rootWidget();
  gui::Label* wdg = safety_cast< gui::Label* >( rootWidget->findChild( windowGamePausedId ) );

  switch( _mode )
  {
  case toggle:
  case pause:
  case play:
    game.setPaused( _mode == toggle ? !game.isPaused() : (_mode == pause) );

    if( game.isPaused()  )
    {
      if( !wdg )
      {
        Size scrSize = rootWidget->size();
        wdg = new gui::Label( rootWidget, Rect( Point( (scrSize.width() - 450)/2, 40 ), Size( 450, 50 ) ),
                              _("##game_is_paused##"), false, gui::Label::bgWhiteFrame, windowGamePausedId );
        wdg->setTextAlignment( align::center, align::center );
      }
    }
    else
    {
      if( wdg )
      {
        wdg->deleteLater();
      }
    }
  break;

  case hidepause:
  case hideplay:
    game.setPaused( _mode == hidepause );
  break;

  default: break;
  }
}


GameEventPtr Step::create(unsigned int count)
{
  Step* e = new Step(count);
  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

void Step::_exec(Game &game, unsigned int)
{
  game.step(_count);
}

bool Step::_mayExec(Game &game, unsigned int) const
{
  return game.isPaused();
}

Step::Step(unsigned int count):_count(count)
{
}

GameEventPtr ChangeSpeed::create(int value)
{
  ChangeSpeed* ev = new ChangeSpeed();
  ev->_value = value;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool ChangeSpeed::_mayExec(Game& game, unsigned int) const{  return true;}

ChangeSpeed::ChangeSpeed()
{
  _value = 0;
}

void ChangeSpeed::_exec(Game& game, unsigned int)
{
  game.changeTimeMultiplier( _value );  

  GameEventPtr e = WarningMessage::create( _("##current_game_speed_is##") + utils::i2str( game.timeMultiplier() ) + "%" );
  e->dispatch();
}

} //end namespace events
