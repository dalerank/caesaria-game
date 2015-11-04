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

#include "advisor_emperor_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "objects/construction.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "groupbox.hpp"
#include "listbox.hpp"
#include "listboxitem.hpp"
#include "core/logger.hpp"
#include "city/city.hpp"
#include "city/request.hpp"
#include "core/foreach.hpp"
#include "good/helper.hpp"
#include "game/gamedate.hpp"
#include "gameautopause.hpp"
#include "city/statistic.hpp"
#include "city/victoryconditions.hpp"
#include "city/requestdispatcher.hpp"
#include "game/player.hpp"
#include "dialogbox.hpp"
#include "events/fundissue.hpp"
#include "change_salary_window.hpp"
#include "game/funds.hpp"
#include "world/empire.hpp"
#include "world/emperor.hpp"
#include "city_donation_window.hpp"
#include "emperorgiftwindow.hpp"
#include "gui/environment.hpp"
#include "gui/dialogbox.hpp"
#include "texturedbutton.hpp"
#include "dictionary.hpp"
#include "advisor_request_button.hpp"
#include "gui/widget_helper.hpp"
#include "game/gift.hpp"

using namespace gfx;
using namespace events;
using namespace  city;

namespace gui
{

namespace advisorwnd
{

namespace {
  Point requestButtonOffset = Point( 0, 55 );
  Size requestButtonSize = Size( 560, 40 );

  enum { favourLimiter=20, maxFavourValue=100 };
}

class Emperor::Impl
{
public:
  gui::Label* lbEmperorFavour;
  gui::Label* lbEmperorFavourDesc;
  gui::Label* lbPost;
  gui::Label* lbPrimaryFunds;
  PushButton* btnSendGift;
  PushButton* btnSend2City;
  PushButton* btnChangeSalary; 
  bool isRequestsUpdated;
};

void Emperor::_showChangeSalaryWindow()
{
  if( game::Date::current() > _city->victoryConditions().finishDate() )
  {
    dialog::Information( ui(), "", _("##disabled_draw_salary_for_free_reign##") );
    return;
  }

  auto&& salaryWindow = ui()->add<dialog::ChangeSalary>( _mayor()->salary() );
  salaryWindow.setRanks( world::EmpireHelper::ranks() );
  salaryWindow.show();

  auto&& btnHelp = salaryWindow.add<TexturedButton>( Point( 12, height() - 39), Size( 24 ), -1, config::id.menu.exitInf );
  CONNECT( &btnHelp, onClicked(), this, Emperor::_showHelp );
  CONNECT( &salaryWindow, onChangeSalary(), this, Emperor::_changeSalary )
}

void Emperor::_showSend2CityWindow()
{
  auto&& donationWindow = ui()->add<dialog::CityDonation>( _mayor()->money() );
  donationWindow.show();

  CONNECT( &donationWindow, onSendMoney(), this, Emperor::_sendMoney );
}

void Emperor::_showGiftWindow()
{
  auto&& dialog = ui()->add<dialog::EmperorGift>( _mayor()->money(),
                                                  _emperor().lastGiftDate( _city->name() ) );
  dialog.show();

  CONNECT( &dialog, onSendGift(), this, Emperor::_sendGift );
}

void Emperor::_updateRequests()
{
  __D_REF(_d,Emperor)
  Rect reqsRect( Point( 32, 91 ), Size( 570, 220 ) );

  auto buttons = findChildren<RequestButton*>();
  for( auto btn : buttons )
    btn->deleteLater();

  RequestList requests;
  auto rqDispatcher = _city->statistic().services.find<request::Dispatcher>();

  if( rqDispatcher.isValid() )
  {
    requests = rqDispatcher->requests();
  }

  if( requests.empty() )
  {
    auto&& label = add<Label>( reqsRect, _("##have_no_requests##") );
    label.setWordwrap( true );
    label.setTextAlignment( align::upperLeft, align::center );
  }
  else
  {
    foreach( r, requests )
    {
      if( !(*r)->isDeleted() )
      {
        bool mayExec = (*r)->isReady( _city );
        auto&& btn = add<RequestButton>( reqsRect.lefttop() + Point( 5, 5 ),
                                         std::distance( requests.begin(), r ), *r );
        btn.setTooltipText( _("##request_btn_tooltip##") );
        btn.setEnabled( mayExec );
        btn.onExecRequest() += makeDelegate( this, &Emperor::_resolveRequest );
      }
    }
  }
  _d.isRequestsUpdated = false;
}

PlayerPtr Emperor::_mayor() {  return _city->mayor(); }
world::Emperor& Emperor::_emperor() { return _city->empire()->emperor(); }

void Emperor::_showHelp()
{
  DictionaryWindow::show( this, "emperor_advisor" );
}

Emperor::Emperor( PlayerCityPtr city, Widget* parent, int id )
: Base( parent, city, id ), __INIT_IMPL(Emperor)
{
  __D_IMPL(_d,Emperor)
  GameAutoPause::insertTo( this );
  _d->isRequestsUpdated = true;

  Widget::setupUI( ":/gui/emperoropts.gui" );

  INIT_WIDGET_FROM_UI( Label*, lbTitle )

  GET_DWIDGET_FROM_UI( _d, lbEmperorFavour )
  GET_DWIDGET_FROM_UI( _d, lbEmperorFavourDesc  )
  GET_DWIDGET_FROM_UI( _d, lbPost )
  GET_DWIDGET_FROM_UI( _d, lbPrimaryFunds )
  GET_DWIDGET_FROM_UI( _d, btnSendGift )
  GET_DWIDGET_FROM_UI( _d, btnSend2City )
  GET_DWIDGET_FROM_UI( _d, btnChangeSalary )

  if( _d->lbEmperorFavour )
    _d->lbEmperorFavour->setText( fmt::format( "{0} {1}", _("##advemp_emperor_favour##"), _city->favour() ) );

  if( _d->lbEmperorFavourDesc )
    _d->lbEmperorFavourDesc->setText( _( _getEmperorFavourStr() ) );

  if( lbTitle )
  {
    std::string text = _mayor()->name();
    if( text.empty() )
      text = _("##emperor_advisor_title##");

    lbTitle->setText( text );
  }

  CONNECT( _d->btnChangeSalary, onClicked(), this, Emperor::_showChangeSalaryWindow );
  CONNECT( _d->btnSend2City, onClicked(), this, Emperor::_showSend2CityWindow );
  CONNECT( _d->btnSendGift, onClicked(), this, Emperor::_showGiftWindow );
}

void Emperor::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  if( _dfunc()->isRequestsUpdated )
  {
    _updateRequests();
  }

  Window::draw( painter );
}

void Emperor::_sendMoney( int money )
{
  _mayor()->appendMoney( -money );
  events::dispatch<Payment>( econ::Issue::donation, money );
}

void Emperor::_sendGift(int money)
{
  if( money > _mayor()->money() )
  {
    dialog::Information( ui(),
                         _("##nomoney_for_gift_title##"),
                         _("##nomoney_for_gift_text##") );
    return;
  }

  _mayor()->appendMoney( -money );
  _city->empire()->emperor().sendGift( Gift( _city->name(), "gift", money ) );
}

void Emperor::_changeSalary( int money )
{
  _mayor()->setSalary( money );

  float salKoeff = world::EmpireHelper::governorSalaryKoeff( ptr_cast<world::City>( _city ) );
  if( salKoeff > 1.f )
  {
    dialog::Information( ui(),
                         _("##changesalary_warning##"),
                         _("##changesalary_greater_salary##") );
  }
}

std::string Emperor::_getEmperorFavourStr()
{
  return fmt::format( "##emperor_favour_%02d##", _city->favour() * favourLimiter / maxFavourValue  );
}

void Emperor::_resolveRequest(RequestPtr request)
{
  if( request.isValid() )
  {
    request->exec( _city );
    _dfunc()->isRequestsUpdated = true;
  }
}

}//end namespace advisorwnd

}//end namespace gui
