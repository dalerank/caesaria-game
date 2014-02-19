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
//#include "building/metadata.hpp"
#include "city/funds.hpp"
#include "gui/environment.hpp"
#include "gui/info_box.hpp"
#include "gfx/tilemap.hpp"
#include "game/game.hpp"
#include "dispatcher.hpp"
#include "core/stringhelper.hpp"
#include "gui/label.hpp"
#include "gui/empiremap_window.hpp"
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

bool GameEvent::tryExec(Game& game, uint time)
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

bool ClearLandEvent::_mayExec(Game& game, uint time) const
{
  return true;
}

void ClearLandEvent::_exec( Game& game, uint )
{
  Tilemap& tmap = game.getCity()->getTilemap();

  Tile& cursorTile = tmap.at( _pos );

  if( cursorTile.getFlag( Tile::isDestructible ) )
  {
    Size size( 1 );
    TilePos rPos = _pos;

    TileOverlayPtr overlay = cursorTile.getOverlay();

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
      size = overlay->getSize();
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

      if( tile->getFlag( Tile::tlMeadow ) )
      {
        tile->setPicture( TileHelper::convId2PicName( tile->getOriginalImgId() ) );
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
        tile->setOriginalImgId( TileHelper::convPicName2Id( pic.getName() ) );
      }
    }

    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number
    //
    if( deleteRoad )
    {
      game.getCity()->updateRoads();
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

bool ShowInfoboxEvent::_mayExec(Game& game, uint time) const
{
  return true;
}

void ShowInfoboxEvent::_exec( Game& game, uint )
{
  gui::InfoboxText* msgWnd = new gui::InfoboxText( game.getGui()->getRootWidget(), _title, _text );
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

bool Pause::_mayExec(Game& game, uint time) const
{
  return true;
}

void Pause::_exec(Game& game, uint)
{
  gui::Widget* rootWidget = game.getGui()->getRootWidget();
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
        Size scrSize = rootWidget->getSize();
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

bool ChangeSpeed::_mayExec(Game& game, uint) const
{
  return true;
}

void ChangeSpeed::_exec(Game& game, uint)
{
  game.changeTimeMultiplier( _value );
}


GameEventPtr FundIssueEvent::create(int type, int value)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_value = value;
  ev->_type = type;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr FundIssueEvent::import(Good::Type good, int qty)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_type = CityFunds::importGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr FundIssueEvent::exportg(Good::Type good, int qty)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_type = CityFunds::exportGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool FundIssueEvent::_mayExec(Game& game, uint time) const
{
  return true;
}

void FundIssueEvent::_exec(Game& game, uint )
{
  if( _type == CityFunds::importGoods )
  {
    int price = game.getCity()->getTradeOptions().getSellPrice( _gtype );
    _value = -price * _qty / 100;
  }
  else if( _type == CityFunds::exportGoods )
  {
    int price = game.getCity()->getTradeOptions().getBuyPrice( _gtype );
    _value = price * _qty / 100;
  }

  game.getCity()->getFunds().resolveIssue( FundIssue( _type, _value ) );
}


GameEventPtr ShowEmpireMapWindow::create(bool show)
{
  ShowEmpireMapWindow* ev = new ShowEmpireMapWindow();
  ev->_show = show;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool ShowEmpireMapWindow::_mayExec(Game& game, uint time) const
{
  return true;
}

void ShowEmpireMapWindow::_exec(Game& game, uint)
{
  List<gui::EmpireMapWindow*> wndList = game.getGui()->getRootWidget()->findChildren<gui::EmpireMapWindow*>();

  if( _show )
  {
    if( !wndList.empty() )
    {
      wndList.front()->bringToFront();
    }
    else
    {
      gui::EmpireMapWindow::create( game.getEmpire(), game.getCity(), game.getGui()->getRootWidget(), -1 );
    }
  }
  else
  {
    if( !wndList.empty() )
    {
      wndList.front()->deleteLater();
    }
  }
}


GameEventPtr ShowAdvisorWindow::create(bool show, int advisor)
{
  ShowAdvisorWindow* ev = new ShowAdvisorWindow();
  ev->_show = show;
  ev->_advisor = advisor;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool ShowAdvisorWindow::_mayExec(Game& game, uint time) const
{
  return true;
}

void ShowAdvisorWindow::_exec(Game& game, uint)
{
  List<gui::AdvisorsWindow*> wndList = game.getGui()->getRootWidget()->findChildren<gui::AdvisorsWindow*>();

  if( _show )
  {
    if( !wndList.empty() )
    {
      wndList.front()->bringToFront();
      wndList.front()->showAdvisor( (AdvisorType)_advisor );
    }
    else
    {
      gui::AdvisorsWindow::create( game.getGui()->getRootWidget(), -1, (AdvisorType)_advisor, game.getCity() );
    }
  }
  else
  {
    if( !wndList.empty() )
    {
      wndList.front()->deleteLater();
    }
  }
}

GameEventPtr WarningMessageEvent::create(const std::string& text)
{
  WarningMessageEvent* ev = new WarningMessageEvent();
  ev->_text = text;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool WarningMessageEvent::_mayExec(Game& game, uint time) const
{
  return true;
}

void WarningMessageEvent::_exec(Game& game, uint)
{
  gui::WindowMessageStack* window = safety_cast<gui::WindowMessageStack*>(
                                      game.getGui()->getRootWidget()->findChild( gui::WindowMessageStack::defaultID ) );

  if( window && !_text.empty() )
  {
    window->addMessage( _(_text) );
  }
}

} //end namespace events
