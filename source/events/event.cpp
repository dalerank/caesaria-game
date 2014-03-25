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

#include "event.hpp"
#include "objects/objects_factory.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "city/funds.hpp"
#include "gui/environment.hpp"
#include "gui/info_box.hpp"
#include "gfx/tilemap.hpp"
#include "game/game.hpp"
#include "dispatcher.hpp"
#include "core/stringhelper.hpp"
#include "gui/label.hpp"
#include "world/empire.hpp"
#include "game/resourcegroup.hpp"
#include "gui/advisors_window.hpp"
#include "city/trade_options.hpp"
#include "gui/message_stack_widget.hpp"
#include "game/settings.hpp"
#include "objects/constants.hpp"
#include "gfx/tilesarray.hpp"
#include "playsound.hpp"
#include "city/helper.hpp"
#include "city/statistic.hpp"

using namespace constants;

namespace events
{

bool GameEvent::tryExec(Game& game, unsigned int time)
{
  if( _mayExec( game, time ) )
  {
    _exec( game, time );
    return true;
  }

  return false;
}

bool GameEvent::isDeleted() const { return true; }
void events::GameEvent::dispatch() { Dispatcher::instance().append( this );}
VariantMap GameEvent::save() const { return VariantMap(); }
void GameEvent::load(const VariantMap&) {}

static const int windowGamePausedId = StringHelper::hash( "gamepause" );

GameEventPtr ClearLandEvent::create(const TilePos& pos)
{
  ClearLandEvent* ev = new ClearLandEvent();
  ev->_pos = pos;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

bool ClearLandEvent::_mayExec(Game& game, unsigned int time) const{  return true;}

void ClearLandEvent::_exec( Game& game, unsigned int )
{
  Tilemap& tmap = game.city()->getTilemap();

  Tile& cursorTile = tmap.at( _pos );

  if( cursorTile.getFlag( Tile::isDestructible ) )
  {
    Size size( 1 );
    TilePos rPos = _pos;

    TileOverlayPtr overlay = cursorTile.overlay();

    bool deleteRoad = cursorTile.getFlag( Tile::tlRoad );

    ConstructionPtr constr = ptr_cast<Construction>(overlay);
    if( constr.isValid() && !constr->canDestroy() )
    {
      GameEventPtr e = WarningMessageEvent::create( _( constr->getError().c_str() ) );
      e->dispatch();
      return;
    }

    if( overlay.isValid() )
    {
      size = overlay->size();
      rPos = overlay->pos();
      overlay->deleteLater();
    }

    TilesArray clearedTiles = tmap.getArea( rPos, size );
    foreach( it, clearedTiles )
    {
      Tile* tile = *it;
      tile->setMasterTile( NULL );
      tile->setFlag( Tile::tlTree, false);
      tile->setFlag( Tile::tlRoad, false);
      tile->setFlag( Tile::tlGarden, false);
      tile->setOverlay( NULL );

      deleteRoad |= tile->getFlag( Tile::tlRoad );

      if( tile->getFlag( Tile::tlMeadow ) || tile->getFlag( Tile::tlWater ) )
      {
        tile->setPicture( TileHelper::convId2PicName( tile->originalImgId() ) );
      }
      else
      {
        // choose a random landscape picture:
        // flat land1a 2-9;
        // wheat: land1a 18-29;
        // green_something: land1a 62-119;  => 58
        // green_flat: land1a 232-289; => 58

        // choose a random background image, green_something 62-119 or green_flat 232-240
         // 30% => choose green_sth 62-119
        // 70% => choose green_flat 232-289
        int startOffset  = ( (rand() % 10 > 6) ? 62 : 232 );
        int imgId = rand() % 58;

        Picture pic = Picture::load( ResourceGroup::land1a, startOffset + imgId );
        tile->setPicture( ResourceGroup::land1a, startOffset + imgId );
        tile->setOriginalImgId( TileHelper::convPicName2Id( pic.name() ) );
      }
    }

    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number
    //
    if( deleteRoad )
    {
      game.city()->updateRoads();
    }
  }
}

GameEventPtr ShowInfoboxEvent::create( const std::string& title, const std::string& text )
{
  ShowInfoboxEvent* ev = new ShowInfoboxEvent();
  ev->_title = title;
  ev->_text = text;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

bool ShowInfoboxEvent::_mayExec(Game& game, unsigned int time) const{  return true;}

void ShowInfoboxEvent::_exec( Game& game, unsigned int )
{
  gui::InfoboxText* msgWnd = new gui::InfoboxText( game.gui()->rootWidget(), _title, _text );
  msgWnd->show();
}


GameEventPtr Pause::create( Mode mode )
{
  Pause* e = new Pause();
  e->_mode = mode;

  GameEventPtr ret( e );
  ret->drop();
  return ret;
}

bool Pause::_mayExec(Game& game, unsigned int time) const{  return true;}

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
        wdg->setTextAlignment( alignCenter, alignCenter );
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
  }
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
}

GameEventPtr WarningMessageEvent::create(const std::string& text)
{
  WarningMessageEvent* ev = new WarningMessageEvent();
  ev->_text = text;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool WarningMessageEvent::_mayExec(Game& game, unsigned int time) const
{
  return true;
}

void WarningMessageEvent::_exec(Game& game, unsigned int)
{
  gui::WindowMessageStack* window = safety_cast<gui::WindowMessageStack*>(
                                      game.gui()->rootWidget()->findChild( gui::WindowMessageStack::defaultID ) );

  if( window && !_text.empty() )
  {
    window->addMessage( _(_text) );
  }
}

} //end namespace events
