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

#include "showadvisorwindow.hpp"
#include "gui/advisors_window.hpp"
#include "game/game.hpp"
#include "gui/environment.hpp"
#include "warningmessage.hpp"
#include "city/helper.hpp"
#include "core/variant_map.hpp"
#include "objects/senate.hpp"
#include "core/logger.hpp"
#include "factory.hpp"

using namespace constants;

namespace events
{

REGISTER_EVENT_IN_FACTORY(ShowAdvisorWindow, "advisor_window")

GameEventPtr ShowAdvisorWindow::create()
{
  GameEventPtr ret( new ShowAdvisorWindow() );
  ret->drop();
  return ret;
}

GameEventPtr ShowAdvisorWindow::create(bool show, advisor::Type advisor)
{
  ShowAdvisorWindow* ev = new ShowAdvisorWindow();
  ev->_show = show;
  ev->_advisor = advisor;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

void ShowAdvisorWindow::load(const VariantMap &stream)
{
  GameEvent::load( stream );

  _show = stream.get( "show" );
  Variant adv = stream.get( "advisor" );
  if( adv.type() == Variant::String ) { _advisor = advisor::findType( adv.toString() ); }
  else { _advisor = (advisor::Type)adv.toInt(); }
}

bool ShowAdvisorWindow::_mayExec(Game& game, unsigned int time) const {  return true; }

ShowAdvisorWindow::ShowAdvisorWindow() : _show( false ), _advisor( advisor::count )
{
  _show = false;
  _advisor = advisor::count;
}

void ShowAdvisorWindow::_exec(Game& game, unsigned int)
{
  bool advEnabled = game.city()->getOption( PlayerCity::adviserEnabled ) > 0;
  if( !advEnabled )
  {
    events::GameEventPtr e = events::WarningMessage::create( "##not_available##" );
    e->dispatch();
    return;
  }

  city::Helper helper( game.city() );
  SenateList senates = helper.find<Senate>( objects::senate );
  if( senates.empty() )
  {
    events::GameEventPtr e = events::WarningMessage::create( "##build_senate_for_advisors##" );
    e->dispatch();
    return;
  }

  List<gui::advisorwnd::AWindow*> wndList = game.gui()->rootWidget()->findChildren<gui::advisorwnd::AWindow*>();

  if( _show )
  {
    if( !wndList.empty() )
    {
      wndList.front()->bringToFront();
      wndList.front()->showAdvisor( _advisor );
    }
    else
    {
      gui::advisorwnd::AWindow::create( game.gui()->rootWidget(), -1, _advisor, game.city() );
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

} //end namespace events
