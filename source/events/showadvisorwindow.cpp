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
#include "city/statistic.hpp"
#include "core/variant_map.hpp"
#include "objects/senate.hpp"
#include "core/logger.hpp"
#include "gui/widget_helper.hpp"
#include "factory.hpp"

using namespace gui::advisorwnd;

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
  if( adv.type() == Variant::String ) { _advisor = advisor::fromString( adv.toString() ); }
  else { _advisor = (advisor::Type)adv.toInt(); }
}

bool ShowAdvisorWindow::_mayExec(Game& game, unsigned int time) const {  return true; }

ShowAdvisorWindow::ShowAdvisorWindow()
  : _show( false ), _advisor( advisor::unknown )
{
}

void ShowAdvisorWindow::_exec(Game& game, unsigned int)
{
  bool advEnabled = game.city()->getOption( PlayerCity::adviserEnabled ) > 0;
  if( !advEnabled )
  {
    GameEventPtr e = WarningMessage::create( "##not_available##", 1 );
    e->dispatch();
    return;
  }

  bool haveSenate = game.city()->statistic().objects.count( object::senate ) > 0;
  if( !haveSenate )
  {
    GameEventPtr e = WarningMessage::create( "##build_senate_for_advisors##", 1 );
    e->dispatch();
    return;
  }

  Parlor* parlor = gui::findChildA<Parlor*>( true, game.gui()->rootWidget() );

  if( _show )
  {
    if( parlor )
    {
      parlor->bringToFront();
      parlor->showAdvisor( _advisor );
    }
    else
    {
      Parlor::create( game.gui()->rootWidget(), -1, _advisor, game.city() );
    }
  }
  else
  {
    if( parlor )
    {
      parlor->deleteLater();
    }
  }
}

} //end namespace events
