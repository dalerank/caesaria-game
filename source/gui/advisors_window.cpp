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

#include "advisors_window.hpp"

#include "gfx/picture.hpp"
#include "gfx/pictureconverter.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "texturedbutton.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/decorator.hpp"
#include "label.hpp"
#include "city/city.hpp"
#include "core/utils.hpp"
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
#include "image.hpp"
#include "gameautopause.hpp"
#include "events/fundissue.hpp"
#include "core/smartlist.hpp"
#include "objects/military.hpp"
#include "widgetescapecloser.hpp"
#include "events/showempiremapwindow.hpp"
#include "advisor_population_window.hpp"
#include "widget_helper.hpp"
#include "world/empire.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace advisorwnd
{

class AWindow::Impl
{
public:
  GameAutoPause locker;
  Widget* advisorPanel;

  Point offset;

  PlayerCityPtr city;

  void sendMoney2City( int money );
  void showEmpireMapWindow();
};

PushButton* AWindow::addButton( const int pos, const int picId, std::string tooltip )
{
  Point tabButtonPos( (width() - 636) / 2 + 10, height() / 2 + 192 + 10);

  PushButton* btn = new TexturedButton( this, tabButtonPos + Point( 48, 0 ) * pos, Size( 40 ), pos, picId, picId, picId + 13 );
  btn->setIsPushButton( true );
  btn->setTooltipText( tooltip );
  return btn;
}

AWindow::AWindow( Widget* parent, int id )
: Window( parent, Rect( Point(0, 0), parent->size() ), "", id ), _d( new Impl )
{
  _d->locker.activate();
  // use some clipping to remove the right and bottom areas
  setupUI( ":/gui/advisors.gui" );
  _d->advisorPanel = 0;

  WidgetEscapeCloser::insertTo( this );

  INIT_WIDGET_FROM_UI( Image*, imgBgButtons )

  if( imgBgButtons )
    imgBgButtons->setPosition( Point( (width() - 636) / 2, height() / 2 + 192) );

  addButton( advisor::employers,     255, _("##visit_labor_advisor##"        ));
  addButton( advisor::military,      256, _("##visit_military_advisor##"     ));
  addButton( advisor::empire,        257, _("##visit_imperial_advisor##"     ));
  addButton( advisor::ratings,       258, _("##visit_rating_advisor##"       ));
  addButton( advisor::trading,       259, _("##visit_trade_advisor##"        ));
  addButton( advisor::population,    260, _("##visit_population_advisor##"   ));
  addButton( advisor::health,        261, _("##visit_health_advisor##"       ));
  addButton( advisor::education,     262, _("##visit_education_advisor##"    ));
  addButton( advisor::entertainment, 263, _("##visit_entertainment_advisor##"));
  addButton( advisor::religion,      264, _("##visit_religion_advisor##"     ));
  addButton( advisor::finance,       265, _("##visit_financial_advisor##"    ));
  addButton( advisor::main,          266, _("##visit_chief_advisor##"        ));

  PushButton* btn = addButton( advisor::count, 609 );
  btn->setIsPushButton( false );

  CONNECT( btn, onClicked(), this, AWindow::deleteLater );
}

void AWindow::showAdvisor( const constants::advisor::Type type )
{
  if( type >= advisor::count )
    return;

  Widget::Widgets rchildren = children();
  foreach( child, rchildren )
  {
    PushButton* btn = safety_cast< PushButton* >( *child );
    if( btn )
    {
      btn->setPressed( btn->ID() == type );
    }
  }

  if( _d->advisorPanel )
  {
    _d->advisorPanel->deleteLater();
    _d->advisorPanel = 0;
  }

  switch( type )
  {
  case advisor::employers: _d->advisorPanel = new advisorwnd::Employer( _d->city, this, advisor::employers ); break;
  case advisor::military:
  {
    FortList forts;
    forts << _d->city->overlays();
    _d->advisorPanel = new advisorwnd::Legion( this, advisor::military, _d->city, forts );
  }
  break;

  case advisor::population: _d->advisorPanel = new advisorwnd::Population( _d->city, this, advisor::population ); break;

  case advisor::empire: _d->advisorPanel = new advisorwnd::Emperor( _d->city, this, advisor::empire ); break;
  case advisor::ratings: _d->advisorPanel = new advisorwnd::Ratings( this, advisor::ratings, _d->city ); break;
  case advisor::trading:
    {
      advisorwnd::Trade* wnd = new advisorwnd::Trade( _d->city, this, advisor::trading );
      _d->advisorPanel =  wnd;
      CONNECT( wnd, onEmpireMapRequest(), _d.data(), Impl::showEmpireMapWindow );
    }
  break;

  case advisor::education: _d->advisorPanel = new advisorwnd::Education( _d->city, this, -1 ); break;
  case advisor::health: _d->advisorPanel = new advisorwnd::Health( _d->city, this, -1 ); break;
  case advisor::entertainment: _d->advisorPanel = new advisorwnd::Entertainment( _d->city, this, -1 ); break;
  case advisor::religion: _d->advisorPanel = new advisorwnd::Religion( _d->city, this, -1 ); break;
  case advisor::finance: _d->advisorPanel = new advisorwnd::Finance( _d->city, this, -1 ); break;
  case advisor::main: _d->advisorPanel = new advisorwnd::AdvisorChief( _d->city, this, -1 );

  default:
  break;
  }
}

void AWindow::draw(gfx::Engine& engine )
{
  if( !visible() )
    return;

  Window::draw( engine );
}

bool AWindow::onEvent( const NEvent& event )
{
  if( event.EventType == sEventMouse && event.mouse.type == mouseRbtnRelease )
  {
    deleteLater();
    return true;
  }

  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->ID();
    if( id >= 0 && id < advisor::count )
    {
      showAdvisor( (constants::advisor::Type)event.gui.caller->ID() );
    }
  }

  return Widget::onEvent( event );
}

AWindow* AWindow::create(Widget* parent, int id, const constants::advisor::Type type, PlayerCityPtr city )
{
  AWindow* ret = new AWindow( parent, id );
  ret->_d->city = city;
  ret->showAdvisor( type );

  return ret;
}

void AWindow::Impl::sendMoney2City(int money)
{
 events::GameEventPtr event = events::FundIssueEvent::create( city::Funds::donation, money );
 event->dispatch();
}

void AWindow::Impl::showEmpireMapWindow()
{
  advisorPanel->parent()->deleteLater();
  events::GameEventPtr event = events::ShowEmpireMap::create( true );
  event->dispatch();
}

}

}//end namespace gui
