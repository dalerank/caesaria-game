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

#include "city_donation_window.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "game/settings.hpp"
#include "core/logger.hpp"

namespace gui
{

class CityDonationindow::Impl
{
public:
  int wantSend, maxMoney;

public oc3_slots:
  void sendMoney() { sendMoneySignal.emit( wantSend); }

public oc3_signals:
  Signal1<int> sendMoneySignal;
};

CityDonationindow::CityDonationindow( Widget* p, int money )
  : Widget( p, -1, Rect( 0, 0, 1, 1 ) ), __INIT_IMPL(CityDonationindow)
{
  _dfunc()->maxMoney = money;
  _dfunc()->wantSend = 0;

  setupUI( GameSettings::rcpath( "/gui/money2city.gui" ) );
  setCenter( parent()->center() );

  PushButton* btnSend = findChildA<PushButton*>( "btnSend", true, this );
  PushButton* btnCancel = findChildA<PushButton*>( "btnCancel", true, this );

  CONNECT( btnSend, onClicked(), _dfunc().data(), Impl::sendMoney );
  CONNECT( btnSend, onClicked(), this, CityDonationindow::deleteLater );
  CONNECT( btnCancel, onClicked(), this, CityDonationindow::deleteLater );
}

CityDonationindow::~CityDonationindow() {}

bool CityDonationindow::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->getID();
    if( id > 0 && ((id & 0x0f00) == 0x0f00) )
    {
      int multiplier = id & 0xff;
      int maxMoney = _dfunc()->maxMoney;
      _dfunc()->wantSend = math::clamp( (multiplier == 0xff ? maxMoney : (multiplier * 500)), 0, maxMoney );
    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal1<int>&CityDonationindow::onSendMoney() { return _dfunc()->sendMoneySignal;}

}//end namespace gui
