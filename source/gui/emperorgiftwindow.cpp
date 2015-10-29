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
#include "label.hpp"
#include "core/logger.hpp"
#include "listbox.hpp"
#include "core/saveadapter.hpp"
#include "core/gettext.hpp"
#include "widget_helper.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "core/color_list.hpp"
#include "widgetescapecloser.hpp"
#include "core/utils.hpp"
#include "game/gift.hpp"

namespace gui
{

namespace dialog
{

class EmperorGift::Impl
{
public:
  int wantSend, maxMoney;

  void fillGifts( ListBox* lbx );
  unsigned int getGiftCost(Gift::Type type, unsigned int money );

public slots:
  void sendGift() { emit sendGiftSignal( wantSend ); }
  void selectGift( const ListBoxItem& item);

public signals:
  Signal1<int> sendGiftSignal;
};

EmperorGift::EmperorGift(Widget* p, int money , const DateTime &lastgift)
  : Window( p, Rect( 0, 0, 1, 1 ), "" ), __INIT_IMPL(EmperorGift)
{
  __D_REF(d,EmperorGift)
  d.maxMoney = money;
  d.wantSend = 0;

  setupUI( ":/gui/gift4emperor.gui" );
  setCenter( parent()->center() );

  INIT_WIDGET_FROM_UI( Label*, lbLastGiftDate )
  INIT_WIDGET_FROM_UI( ListBox*, lbxGifts )
  INIT_WIDGET_FROM_UI( PushButton*, btnCancel )
  INIT_WIDGET_FROM_UI( PushButton*, btnSend )
  INIT_WIDGET_FROM_UI( Label*, lbPlayerMoney )

  CONNECT( lbxGifts, onItemSelected(), &d, Impl::selectGift );
  CONNECT( btnSend, onClicked(), &d, Impl::sendGift );
  CONNECT( btnSend, onClicked(), this, EmperorGift::deleteLater );
  CONNECT( btnCancel, onClicked(), this, EmperorGift::deleteLater );

  d.fillGifts( lbxGifts );

  if( lbLastGiftDate )
  {
    int monthsLastGift = lastgift.monthsTo( game::Date::current() );
    std::string text = monthsLastGift > 100
                              ? _( "##too_old_sent_gift##")
                              : utils::format( 0xff, "%s  %d  %s",
                                             _("##time_since_last_gift##"),
                                             monthsLastGift,
                                             _("##mo##") );
    lbLastGiftDate->setText( text );
  }

  if( lbPlayerMoney )
  {
    std::string text = utils::format( 0xff, "%s %d Dn", _( "##you_have_money##"), money );
    lbPlayerMoney->setText( text );
  }

  WidgetEscapeCloser::insertTo( this );
  setModal();
}

EmperorGift::~EmperorGift() {}

bool EmperorGift::onEvent(const NEvent& event)
{
  if( event.EventType == sEventMouse && event.mouse.isRightPressed() )
  {
    deleteLater();
    return true;
  }

  return Window::onEvent( event );
}

Signal1<int>& EmperorGift::onSendGift() { return _dfunc()->sendGiftSignal; }

void EmperorGift::Impl::fillGifts(ListBox* lbx)
{
  if( !lbx )
    return;

  VariantMap giftModel = config::load( ":/gifts.model" );
  StringArray gifts = giftModel.get( "items" ).toStringArray();

  lbx->setTextAlignment( align::center, align::center );
  for( int k=0; k < 3; k++ )
  {
    int tag = getGiftCost( (Gift::Type)k, maxMoney );
    std::string giftName = gifts.random();

    if( giftName.empty() )
      return;

    gifts.remove( giftName );

    std::string giftDescription = utils::format( 0xff, "%s : %d", _( giftName ), tag );

    ListBoxItem& item = lbx->addItem( giftDescription );
    item.setTag( tag );
    item.setTextColor( ListBoxItem::simple, tag < maxMoney ? ColorList::black : ColorList::grey );
    item.setEnabled( tag < maxMoney );
  }
}

unsigned int EmperorGift::Impl::getGiftCost( Gift::Type type, unsigned int money)
{
  switch( type )
  {
  case Gift::modest: return money / 8 + 20;
  case Gift::generous: return money / 4 + 50;
  case Gift::lavish: return money / 2 + 100;
  }

  return 100;
}

void EmperorGift::Impl::selectGift(const ListBoxItem& item)
{
  wantSend = item.tag();
}

}//end namespace dialog

}//end namespace gui
