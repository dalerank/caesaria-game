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
#include "world/relations.hpp"
#include "texturedbutton.hpp"
#include "dictionary.hpp"
#include "game/datetimehelper.hpp"
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

namespace
{
  Point requestButtonOffset = Point( 0, 55 );
  Size requestButtonSize = Size( 560, 40 );

  enum { favourLimiter=20, maxFavourValue=100 };
}

class Emperor::Impl
{
public:
  bool isRequestsUpdated;
};

void Emperor::_showChangeSalaryWindow()
{
  if( game::Date::current() > _city->victoryConditions().finishDate() )
  {
    dialog::Information( ui(), "", _("##disabled_draw_salary_for_free_reign##") );
    return;
  }

  auto& salaryWindow = ui()->add<dialog::ChangeSalary>( _mayor()->salary() );

  salaryWindow.add<HelpButton>( Point( 12, height() - 39), "emperor_advisor" );
  salaryWindow.setRanks( world::EmpireHelper::ranks() );
  salaryWindow.show();

  CONNECT_LOCAL( &salaryWindow, onChangeSalary(), Emperor::_changeSalary )
}

void Emperor::_showSend2CityWindow()
{
  auto& donationWindow = ui()->add<dialog::CityDonation>( _mayor()->money() );
  donationWindow.show();

  CONNECT_LOCAL( &donationWindow, onSendMoney(), Emperor::_sendMoney );
}

void Emperor::_showGiftWindow()
{
  auto& dialog = ui()->add<dialog::EmperorGift>( _mayor()->money(),
                                                 _emperor().lastGift( _city->name() ).date() );
  dialog.show();

  CONNECT_LOCAL( &dialog, onSendGift(), Emperor::_sendGift );
}

class GiftDetails : public Window
{
public:
  GiftDetails( Widget* parent, const Size& size, const std::string& title, const world::GiftHistory& history )
   : Window( parent, Rect( Point(), size), title )
  {
    setTextAlignment( align::center, align::center );
    setFont( FONT_3 );
    setTitleRect( Rect( 15, 15, width() - 15, 45 ) );
    add<ExitButton>( Point( width() - 37, 12 ) );

    ListBox& listbox = add<ListBox>( Rect( 15, 45, width()-15, height() - 15 ), -1, true, true );
    listbox.setItemFont( Font::create( FONT_1 ) );
    listbox.setItemHeight( 16 );

    for( auto it=history.rbegin(); it != history.rend(); ++it )
    {
       const Gift& gift = *it;
       std::string text = fmt::format( "{} {} {}", utils::date2str( gift.date(), true ), gift.value(), gift.name() );
       listbox.addItem( text );
    }

    moveToCenter();
    setModal();
  }
};


void Emperor::_showGiftHistory()
{
  const world::Relation& relation = _city->empire()->emperor().relation( _city->name() );
  if( relation.gifts().empty() )
  {
    dialog::Information( ui(), "Note", "You was not sent any gifts to emperor" );
  }
  else
    ui()->add<GiftDetails>( Size( 480, 640 ), _("##history_gift##"), relation.gifts() );
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
    auto& label = add<Label>( reqsRect, _("##have_no_requests##") );
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
        auto& btn = add<RequestButton>( reqsRect.lefttop() + Point( 5, 5 ),
                                        std::distance( requests.begin(), r ), *r );
        btn.setTooltipText( _("##request_btn_tooltip##") );
        btn.setEnabled( mayExec );
        btn.onExecRequest() += makeDelegate( this, &Emperor::_resolveRequest );
      }
    }
  }
  _d.isRequestsUpdated = false;
}

class RqHistoryDetails : public Window
{
public:
  RqHistoryDetails( Widget* parent, const Size& size,
                    const std::string& title, const world::RelationAbilities& history )
   : Window( parent, Rect( Point(), size), title )
  {
    add<ExitButton>( Point( width() - 37, 12 ) );

    ListBox& listbox = add<ListBox>( Rect( 15, 45, width()-15, height() - 15 ), -1, true, true );
    listbox.setItemFont( Font::create( FONT_1 ) );
    listbox.setItemHeight( 16 );

    for( auto it=history.rbegin(); it != history.rend(); ++it )
    {
       const world::RelationAbility& ability = *it;
       if( ability.type == world::RelationAbility::request )
       {
         std::string text = fmt::format( "{} {} {}", utils::date2str( ability.finished, true ),
                                                     ability.successed ? "comply" : "failed",
                                                     ability.message );
         auto& item = listbox.addItem( text );
         item.setTooltip( ability.message );
       }
     }

    moveToCenter();
    setModal();
  }
};

void Emperor::_showRequestsHistory()
{
  world::Emperor& emp = _city->empire()->emperor();
  const auto& relation = emp.relation( _city->name() );
  world::RelationAbilities rqhistory = relation.abilities();

  ui()->add<RqHistoryDetails>( Size( 480, 480 ), "Request's history", rqhistory );
}

PlayerPtr Emperor::_mayor() {  return _city->mayor(); }
world::Emperor& Emperor::_emperor() { return _city->empire()->emperor(); }

Emperor::Emperor(PlayerCityPtr city, Widget* parent)
  : Base( parent, city, advisor::empire ), __INIT_IMPL(Emperor)
{
  __D_IMPL(_d,Emperor)
  GameAutoPause::insertTo( this );
  _d->isRequestsUpdated = true;

  Widget::setupUI( ":/gui/emperoropts.gui" );

  INIT_WIDGET_FROM_UI( Label*, lbTitle )
  INIT_WIDGET_FROM_UI( Label*, lbEmperorFavour )
  INIT_WIDGET_FROM_UI( Label*, lbEmperorFavourDesc )

  if( lbEmperorFavour )
    lbEmperorFavour->setText( fmt::format( "{} {}", _("##advemp_emperor_favour##"), _city->favour() ) );

  if( lbEmperorFavourDesc )
    lbEmperorFavourDesc->setText( _( _getEmperorFavourStr() ) );

  if( lbTitle )
  {
    std::string text = _mayor()->name();
    if( text.empty() )
      text = _("##emperor_advisor_title##");

    lbTitle->setText( text );
  }

  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnChangeSalary, onClicked(), Emperor::_showChangeSalaryWindow )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnSend2City,    onClicked(), Emperor::_showSend2CityWindow )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnSendGift,     onClicked(), Emperor::_showGiftWindow )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnGiftHistory,  onClicked(), Emperor::_showGiftHistory )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnRqHistory,    onClicked(), Emperor::_showRequestsHistory )
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
  _city->empire()->emperor().sendGift( Gift( _city->name(), "gift", money, game::Date::current() ) );
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
  return utils::format( 0xff, "##emperor_favour_%02d##", _city->favour() * favourLimiter / maxFavourValue  );
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
