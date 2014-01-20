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

#include "advisors_window.hpp"

#include "gfx/picture.hpp"
#include "gfx/pictureconverter.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "gui/texturedbutton.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/decorator.hpp"
#include "gui/label.hpp"
#include "city/city.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "advisor_employers_window.hpp"
#include "advisor_legion_window.hpp"
#include "advisor_emperor_window.hpp"
#include "advisor_ratings_window.hpp"
#include "advisor_trade_window.hpp"
#include "advisor_education_window.hpp"
#include "advisor_health_window.hpp"
#include "advisor_entertainment_window.hpp"
#include "advisor_religion_window.hpp"
#include "advisor_finance_window.hpp"
#include "advisor_chief_window.hpp"
#include "core/foreach.hpp"
#include "city/funds.hpp"
#include "events/event.hpp"
#include "city/requestdispatcher.hpp"
#include "game/settings.hpp"
#include "gui/image.hpp"

namespace gui
{

class AdvisorsWindow::Impl
{
public:
  Widget* advisorPanel;

  Point offset;
  PictureRef tabBg;

  PlayerCityPtr city;

  void sendMoney2City( int money );
  void showEmpireMapWindow();
};

PushButton* AdvisorsWindow::addButton( const int pos, const int picId, std::string tooltip )
{
  Point tabButtonPos( (getWidth() - 636) / 2 + 10, getHeight() / 2 + 192 + 10);

  PushButton* btn = new TexturedButton( this, tabButtonPos + Point( 48, 0 ) * pos, Size( 40 ), pos, picId, picId, picId + 13 );
  btn->setIsPushButton( true );
  btn->setTooltipText( tooltip );
  return btn;
}

AdvisorsWindow::AdvisorsWindow( Widget* parent, int id )
: Widget( parent, id, Rect( Point(0, 0), parent->getSize() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  setupUI( GameSettings::rcpath( "/gui/advisors.gui" ) );
  _d->advisorPanel = 0;

  Point tabButtonPos( (getWidth() - 636) / 2, getHeight() / 2 + 192);

  new gui::Image( this, tabButtonPos, Picture::load( ResourceGroup::menuMiddleIcons, 14 ) );
  addButton( ADV_EMPLOYERS, 255, _("##visit_labor_advisor##") );
  addButton( ADV_LEGION, 256, _("##visit_military_advisor##") );
  addButton( ADV_EMPIRE, 257, _("##visit_imperial_advisor##") );
  addButton( ADV_RATINGS, 258, _("##visit_rating_advisor##" ) );
  addButton( ADV_TRADING, 259, _("##visit_trade_advisor##") );
  addButton( ADV_POPULATION, 260, _("##visit_population_advisor##")  );
  addButton( ADV_HEALTH, 261, _("##visit_health_advisor##") );
  addButton( ADV_EDUCATION, 262, _("##visit_education_advisor##") );
  addButton( ADV_ENTERTAINMENT, 263, _("##visit_entertainment_advisor##") );
  addButton( ADV_RELIGION, 264, _("##visit_religion_advisor##") );
  addButton( ADV_FINANCE, 265, _("##visit_financial_advisor##") );
  addButton( ADV_MAIN, 266, _("##visit_chief_advisor##") );

  PushButton* btn = addButton( ADV_COUNT, 609 );
  btn->setIsPushButton( false );

  CONNECT( btn, onClicked(), this, AdvisorsWindow::deleteLater );
}

void AdvisorsWindow::showAdvisor( const AdvisorType type )
{
  if( type >= ADV_COUNT )
    return;

  Widget::Widgets children = getChildren();
  foreach( Widget* child, children )
  {
    if( PushButton* btn = safety_cast< PushButton* >( child ) )
    {
      btn->setPressed( btn->getID() == type );
    }
  }

  if( _d->advisorPanel )
  {
    _d->advisorPanel->deleteLater();
    _d->advisorPanel = 0;
  }

  switch( type )
  {
  case ADV_EMPLOYERS: _d->advisorPanel = new AdvisorEmployerWindow( _d->city, this, ADV_EMPLOYERS ); break;
  case ADV_LEGION: _d->advisorPanel = new AdvisorLegionWindow( this, ADV_LEGION ); break;
  case ADV_EMPIRE:
    {
      AdvisorEmperorWindow* wnd = new AdvisorEmperorWindow( _d->city, this, playerMoney, ADV_EMPIRE );

      _d->advisorPanel = wnd;
      CONNECT( wnd, onSendMoney(), _d.data(), Impl::sendMoney2City );
    }
  break;
  case ADV_RATINGS: _d->advisorPanel = new AdvisorRatingsWindow( this, ADV_RATINGS, _d->city ); break;
  case ADV_TRADING:
    {
      AdvisorTradeWindow* wnd = new AdvisorTradeWindow( _d->city, this, ADV_TRADING );
      _d->advisorPanel =  wnd;
      CONNECT( wnd, onEmpireMapRequest(), _d.data(), Impl::showEmpireMapWindow );
    }
  break;

  case ADV_EDUCATION: _d->advisorPanel = new AdvisorEducationWindow( _d->city, this, -1 ); break;
  case ADV_HEALTH: _d->advisorPanel = new AdvisorHealthWindow( _d->city, this, -1 ); break;
  case ADV_ENTERTAINMENT: _d->advisorPanel = new AdvisorEntertainmentWindow( _d->city, this, -1 ); break;
  case ADV_RELIGION: _d->advisorPanel = new AdvisorReligionWindow( _d->city, this, -1 ); break;
  case ADV_FINANCE: _d->advisorPanel = new AdvisorFinanceWindow( _d->city, this, -1 ); break;
  case ADV_MAIN: _d->advisorPanel = new AdvisorChiefWindow( _d->city, this, -1 );

  default:
  break;
  }
}

void AdvisorsWindow::draw( GfxEngine& engine )
{
  if( !isVisible() )
    return;

  Widget::draw( engine );
}

bool AdvisorsWindow::onEvent( const NEvent& event )
{
  if( event.EventType == sEventMouse && event.mouse.type == mouseRbtnRelease )
  {
    deleteLater();
    return true;
  }

  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->getID();
    if( id >= 0 && id < ADV_COUNT )
    {
      showAdvisor( (AdvisorType)event.gui.caller->getID() );
    }
  }

  return Widget::onEvent( event );
}

AdvisorsWindow* AdvisorsWindow::create( Widget* parent, int id, const AdvisorType type, PlayerCityPtr city )
{
  AdvisorsWindow* ret = new AdvisorsWindow( parent, id );
  ret->_d->city = city;
  ret->showAdvisor( type );

  return ret;
}

void AdvisorsWindow::Impl::sendMoney2City(int money)
{
 events::GameEventPtr event = events::FundIssueEvent::create( CityFunds::donation, money );
 event->dispatch();
}

void AdvisorsWindow::Impl::showEmpireMapWindow()
{
  advisorPanel->getParent()->deleteLater();
  events::GameEventPtr event = events::ShowEmpireMapWindow::create( true );
  event->dispatch();
}

}//end namespace gui
