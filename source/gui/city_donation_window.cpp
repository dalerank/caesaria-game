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

#include "city_donation_window.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "core/logger.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "core/utils.hpp"
#include "widget_helper.hpp"

namespace gui
{

namespace dialog
{

class CityDonation::Impl
{
public:
  struct {
    int current;
    int available;
  } money;

  Signal1<int> sendMoneySignal;
};

CityDonation::CityDonation( Widget* p, int money )
  : Window( p, Rect( 0, 0, 1, 1 ), "" ), __INIT_IMPL(CityDonationWindow)
{
  __D_REF(d,CityDonationWindow)
  d.money.available = money;

  setupUI( ":/gui/money2city.gui" );
  setAvailableMoney( money );
  _linkButtons2Actions();
  _updateDonationText();

  moveToCenter();
  setModal();
}

CityDonation::~CityDonation() {}
Signal1<int>& CityDonation::onSendMoney() { return _dfunc()->sendMoneySignal; }

void CityDonation::setAvailableMoney(int money)
{
  INIT_WIDGET_FROM_UI( Label*, lbBlack )
  INIT_WIDGET_FROM_UI( PushButton*, btnSend )

  _dfunc()->money.current = 0;

  if( money == 0 )
  {
    for( auto& widget : lbBlack->children() )
      widget->hide();

    lbBlack->setText( _("##no_money_for_donation##") );
  }

  btnSend->setVisible( money != 0 );
}

bool CityDonation::_onButtonClicked(Widget* sender)
{
  __D_REF(d,CityDonationWindow)
  int id = sender->ID();
  if( id > 0 )
  {
    int maxMoney = d.money.available;
    int wantSend = d.money.current;
    if( ((id & 0x0f00) == 0x0f00) )
    {
      int multiplier = id & 0xff;
      wantSend = multiplier == 0xff ? maxMoney : (multiplier * 500);
    }
    else if( (id & 0x1000) == 0x1000 )
    {
      int offset = (id & 0xf) == 1 ? -10 : 10;
      wantSend += offset;
    }

    d.money.current = math::clamp( wantSend, 0, maxMoney );
    _updateDonationText();
  }

  return true;
}

void CityDonation::_updateDonationText()
{
  __D_REF(d,CityDonationWindow)
  INIT_WIDGET_FROM_UI( Label*, lbDonation )
  if( lbDonation )
  {
    std::string text = fmt::format( "{} {} from {} dn", _("##donation_is##"), d.money.current, d.money.available );
    lbDonation->setText( text );
  }
}

void CityDonation::_linkButtons2Actions()
{
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnSend, onClicked(),   CityDonation::_sendMoney )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnSend, onClicked(),   CityDonation::deleteLater )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnCancel, onClicked(), CityDonation::deleteLater )
}

void CityDonation::_sendMoney()
{
  __D_REF(d,CityDonationWindow)
  emit d.sendMoneySignal( d.money.current );
}

}//end namespace dialog

}//end namespace gui
