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
  int wantSend, maxMoney;
  Label* lbDonation;

  void updateDonationText();

public slots:
  void sendMoney() { emit sendMoneySignal( wantSend ); }

public signals:
  Signal1<int> sendMoneySignal;
};

CityDonation::CityDonation( Widget* p, int money )
  : Window( p, Rect( 0, 0, 1, 1 ), "" ), __INIT_IMPL(CityDonationWindow)
{
  __D_IMPL(d,CityDonationWindow)
  d->maxMoney = money;
  d->wantSend = 0;

  setupUI( ":/gui/money2city.gui" );
  setCenter( parent()->center() );

  INIT_WIDGET_FROM_UI( Label*, lbBlack )
  INIT_WIDGET_FROM_UI( PushButton*, btnCancel )
  INIT_WIDGET_FROM_UI( PushButton*, btnSend )
  GET_DWIDGET_FROM_UI( d, lbDonation )

  d->updateDonationText();

  CONNECT( btnSend, onClicked(), _dfunc().data(), Impl::sendMoney );
  CONNECT( btnSend, onClicked(), this, CityDonation::deleteLater );
  CONNECT( btnCancel, onClicked(), this, CityDonation::deleteLater );

  if( money == 0 )
  {
    Widgets widgets = lbBlack->children();
    foreach( it, widgets ) (*it)->hide();

    lbBlack->setText( _("##no_money_for_donation##") );
    btnSend->hide();
  }
}

CityDonation::~CityDonation() {}

bool CityDonation::onEvent(const NEvent& event)
{
  __D_IMPL(d,CityDonationWindow)
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->ID();
    if( id > 0 )
    {
      int maxMoney = d->maxMoney;
      int wantSend = d->wantSend;
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

      d->wantSend =  math::clamp( wantSend, 0, maxMoney );
      d->updateDonationText();
    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal1<int>& CityDonation::onSendMoney() { return _dfunc()->sendMoneySignal; }

void CityDonation::Impl::updateDonationText()
{
  std::string text = utils::format( 0xff, "%s %d from %d dn", _("##donation_is##"), wantSend, maxMoney );
  if( lbDonation ) lbDonation->setText( text );
}

}//end namespace dialog

}//end namespace gui
