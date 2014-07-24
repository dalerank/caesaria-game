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

#include "emperorgiftwindow.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "game/settings.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "listbox.hpp"
#include "core/saveadapter.hpp"
#include "core/gettext.hpp"

namespace gui
{

class EmperorGiftWindow::Impl
{
public:
  typedef enum { modest, generous, lavish } GiftType;
  int wantSend, maxMoney;

  void fillGifts( ListBox* lbx );
  unsigned int getGiftCost( GiftType type, unsigned int money );

public oc3_slots:
  void sendGift() { oc3_emit sendGiftSignal( wantSend ); }
  void selectGift( const ListBoxItem& item);

public oc3_signals:
  Signal1<int> sendGiftSignal;
};

EmperorGiftWindow::EmperorGiftWindow( Widget* p, int money )
  : Widget( p, -1, Rect( 0, 0, 1, 1 ) ), __INIT_IMPL(EmperorGiftWindow)
{
  _dfunc()->maxMoney = money;
  _dfunc()->wantSend = 0;

  setupUI( GameSettings::rcpath( "/gui/gift4emperor.gui" ) );
  setCenter( parent()->center() );

  PushButton* btnSend = findChildA<PushButton*>( "btnSend", true, this );
  PushButton* btnCancel = findChildA<PushButton*>( "btnCancel", true, this );
  ListBox* lbxGifts = findChildA<ListBox*>( "lbxGifts", true, this );

  CONNECT( lbxGifts, onItemSelected(), _dfunc().data(), Impl::selectGift );
  CONNECT( btnSend, onClicked(), _dfunc().data(), Impl::sendGift );
  CONNECT( btnSend, onClicked(), this, EmperorGiftWindow::deleteLater );
  CONNECT( btnCancel, onClicked(), this, EmperorGiftWindow::deleteLater );

  _dfunc()->fillGifts( lbxGifts );
}

EmperorGiftWindow::~EmperorGiftWindow() {}

Signal1<int>& EmperorGiftWindow::onSendGift() { return _dfunc()->sendGiftSignal; }

void EmperorGiftWindow::Impl::fillGifts(ListBox* lbx)
{
  if( !lbx )
    return;

  vfs::Path giftsDesc = SETTINGS_RC_PATH( giftsModel );
  VariantMap giftModel = SaveAdapter::load( giftsDesc );
  StringArray gifts = giftModel.get( "items" ).toStringArray();

  lbx->setTextAlignment( align::center, align::center );
  for( int k=0; k < 3; k++ )
  {
    int tag = getGiftCost( (GiftType)k, maxMoney );
    std::string priceStr = StringHelper::format( 0xff, " : %d", tag );
    ListBoxItem& item = lbx->addItem( _( gifts.random() ) + priceStr );
    item.setTag( tag );
    item.setTextColor( ListBoxItem::simple, tag < maxMoney ? DefaultColors::black : DefaultColors::grey );
    item.setEnabled( tag < maxMoney );
  }
}

unsigned int EmperorGiftWindow::Impl::getGiftCost(EmperorGiftWindow::Impl::GiftType type, unsigned int money)
{
  switch( type )
  {
  case modest: return money / 8 + 20;
  case generous: return money / 4 + 50;
  case lavish: return money / 2 + 100;
  }

  return 100;
}

void EmperorGiftWindow::Impl::selectGift(const ListBoxItem& item)
{
  wantSend = item.tag();
}

}//end namespace gui
