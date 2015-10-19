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
#include "game/funds.hpp"
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
#include "city/statistic.hpp"

using namespace gfx;
using namespace events;

namespace gui
{

namespace advisorwnd
{

PushButton* Parlor::_addButton( Advisor adv, const int picId, std::string tooltip )
{
  Point tabButtonPos( (width() - 636) / 2 + 10, height() / 2 + 192 + 10);

  PushButton* btn = new TexturedButton( this, tabButtonPos + Point( 48, 0 ) * (adv-1), Size( 40 ),
                                        adv, picId, picId, picId + 13 );
  btn->setIsPushButton( true );
  btn->setTooltipText( tooltip );
  return btn;
}

void Parlor::_initButtons()
{
  if( _model.isNull() )
  {
    Logger::warning( "Parlor model is null. Cant init buttons" );
    return;
  }

  auto buttons = children().select<PushButton>();
  for( auto&& btn : buttons )
    btn->deleteLater();

  for( auto& item : _model->items() )
    _addButton( item.type, item.pic, _( fmt::format( "##visit_{}_advisor##", item.tooltip ) ) );

  PushButton* btn = _addButton( advisor::unknown, 609 );
  btn->setIsPushButton( false );

  CONNECT( btn, onClicked(), this, Parlor::deleteLater );
}

Parlor::Parlor( Widget* parent, int id )
  : Window( parent, Rect( Point(0, 0), parent->size() ), "", id )
{
  setupUI( ":/gui/advisors.gui" );

  GameAutoPause::insertTo( this, true );
  WidgetEscapeCloser::insertTo( this );

  INIT_WIDGET_FROM_UI( Image*, imgBgButtons )

  if( imgBgButtons )
    imgBgButtons->setPosition( Point( (width() - 636) / 2, height() / 2 + 192) );
}

void Parlor::setModel(ParlorModel* model)
{
  _model.reset( model );
  model->setParent( this );
  _initButtons();

  if( model != nullptr )
  {
    CONNECT( this, onSwitchAdvisor, model, ParlorModel::switchAdvisor );
  }
}

void Parlor::draw(gfx::Engine& engine )
{
  if( !visible() )
    return;

  Window::draw( engine );
}

bool Parlor::onEvent( const NEvent& event )
{
  if( event.EventType == sEventMouse && event.mouse.type == mouseRbtnRelease )
  {
    deleteLater();
    return true;
  }

  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->ID();
    if( id >= 0 && id < advisor::unknown )
    {
      emit onSwitchAdvisor( Advisor( event.gui.caller->ID() ) );
    }
  }

  return Widget::onEvent( event );
}

void Parlor::showAdvisor(Advisor type) { emit onSwitchAdvisor(type); }

Parlor* Parlor::create(Widget* parent, int id, const Advisor type, ParlorModel* model )
{
  Parlor* ret = new Parlor( parent, id );
  ret->setModel( model );
  ret->showAdvisor( type );

  return ret;
}

class ParlorModel::Impl
{
public:
  Widget* parent;
  Widget* advisorPanel;

  Point offset;
  PlayerCityPtr city;
};

ParlorModel::ParlorModel(PlayerCityPtr city)
  : __INIT_IMPL(ParlorModel)
{
  _dfunc()->city = city;
}


void ParlorModel::sendMoney2City(int money)
{
 auto event = Payment::create( econ::Issue::donation, money );
 event->dispatch();
}

void ParlorModel::showEmpireMapWindow()
{
  __D_IMPL(d,ParlorModel)
  d->advisorPanel->parent()->deleteLater();
  auto event = ShowEmpireMap::create( d->parent );
  event->dispatch();
}

void ParlorModel::setParent(Widget* parlor)
{
  _dfunc()->parent = parlor;
}

Parlor::Items ParlorModel::items()
{
  Parlor::Items items = { { advisor::employers,     255, "labor" },
                          { advisor::military,      256, "military"},
                          { advisor::empire,        257, "imperial"},
                          { advisor::ratings,       258, "rating"},
                          { advisor::trading,       259, "trade"},
                          { advisor::population,    260, "population"},
                          { advisor::health,        261, "health"},
                          { advisor::education,     262, "education"},
                          { advisor::entertainment, 263, "entertainment"},
                          { advisor::religion,      264, "religion"},
                          { advisor::finance,       265, "financial"},
                          { advisor::main,          266, "chief"} };

  return items;
}

void ParlorModel::switchAdvisor(Advisor type)
{
  __D_IMPL(d,ParlorModel)
  if( type >= advisor::unknown )
    return;

  auto buttons = d->parent->children().select<PushButton>();
  for( auto btn : buttons )
  {
    btn->setPressed( btn->ID() == type );
  }

  if( d->advisorPanel )
  {
    d->advisorPanel->deleteLater();
    d->advisorPanel = 0;
  }

  if( type == advisor::employers )  { d->advisorPanel = new advisorwnd::Employer( d->city, d->parent, advisor::employers );  }
  else if( type == advisor::military )
  {
    auto forts = d->city->statistic().objects.find<Fort>();
    d->advisorPanel = new advisorwnd::Legion( d->parent, advisor::military, d->city, forts );
  }
  else if( type == advisor::population ) { d->advisorPanel = new advisorwnd::Population( d->city, d->parent, advisor::population ); }
  else if( type ==  advisor::empire )     d->advisorPanel = new advisorwnd::Emperor( d->city, d->parent, advisor::empire );
  else if( type == advisor::ratings )     d->advisorPanel = new advisorwnd::Ratings( d->parent, advisor::ratings, d->city );
  else if( type == advisor::trading )
  {
    auto tradeWindow = new advisorwnd::Trade( d->city, d->parent, advisor::trading );
    d->advisorPanel =  tradeWindow;
    CONNECT( tradeWindow, onEmpireMapRequest(), this, ParlorModel::showEmpireMapWindow );
  }
  else if( type == advisor::education )    d->advisorPanel = new advisorwnd::Education( d->city, d->parent, -1 );
  else if( type == advisor::health ) d->advisorPanel = new advisorwnd::Health( d->city, d->parent, -1 );
  else if( type == advisor::entertainment ) d->advisorPanel = new advisorwnd::Entertainment( d->city, d->parent, -1 );
  else if( type == advisor::religion ) d->advisorPanel = new advisorwnd::Religion( d->city, d->parent, -1 );
  else if( type == advisor::finance ) d->advisorPanel = new advisorwnd::Finance( d->city, d->parent, -1 );
  else if( type == advisor::main ) d->advisorPanel = new advisorwnd::Chief( d->city, d->parent, -1 );
}

}//end namespace advisorwnd

}//end namespace gui
