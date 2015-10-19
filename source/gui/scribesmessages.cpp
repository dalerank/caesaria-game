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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "scribesmessages.hpp"
#include "gameautopause.hpp"
#include "listbox.hpp"
#include "core/variant_map.hpp"
#include "game/resourcegroup.hpp"
#include "game/datetimehelper.hpp"
#include "core/utils.hpp"
#include "widgetescapecloser.hpp"
#include "city/scribes.hpp"
#include "city/city.hpp"
#include "core/logger.hpp"
#include "core/event.hpp"
#include "widgetescapecloser.hpp"
#include "popup_messagebox.hpp"
#include "texturedbutton.hpp"
#include "core/color.hpp"
#include "dictionary.hpp"
#include "gfx/engine.hpp"
#include "event_messagebox.hpp"
#include "core/gettext.hpp"
#include "gui/label.hpp"
#include "events/playsound.hpp"
#include "widget_helper.hpp"

using namespace gfx;

namespace gui
{

namespace dialog
{

CAESARIA_LITERALCONST(opened)
CAESARIA_LITERALCONST(critical)
CAESARIA_LITERALCONST(ext)
CAESARIA_LITERALCONST(date)

class ScribesListBox : public ListBox
{
public:
  ScribesListBox( Widget* p, const Rect& rect ) : ListBox( p, rect )
  {
    setFlag( selectOnMove, true );
    setItemHeight( Font::create( FONT_1 ).getTextSize( "A" ).width() + 4 );
  }

  virtual ListBoxItem& addItem( const std::string& text, Font font, const int color=0 )
  {
    ListBoxItem& item = ListBox::addItem( text, font, color );

    ListBoxItem::OverrideColor& itemfc = item.OverrideColors[ ListBoxItem::simple ];
    item.OverrideColors[ ListBoxItem::hovered ].font = itemfc.font;
    item.OverrideColors[ ListBoxItem::hovered ].Use = true;
    item.OverrideColors[ ListBoxItem::hovered ].color = 0xffff0000;

    item.setIcon( Picture( ResourceGroup::panelBackground, 111 ));

    return item;
  }

public signals:
  Signal1<int> onShowMessage;
  Signal1<int> onRemoveMessage;

protected:
  int lastIndex=-1;
  Picture pic;

  virtual void _drawItemIcon(gfx::Engine& painter, ListBoxItem& item, const Point& pos, Rect* clipRect)
  {
    bool opened = item.data( literals::opened );
    bool critical = item.data( literals::critical );
    int imgIndex = (critical ? 113 : 111) + (opened ? 1 : 0);
    if( imgIndex != lastIndex )
      pic = Picture( ResourceGroup::panelBackground, imgIndex );
    painter.draw( pic, pos + Point( 2, 2), clipRect );
  }

  virtual void _updateItemText(Engine& painter, ListBoxItem& item, const Rect& textRect, Font font, const Rect& frameRect)
  {
    DateTime time = item.data( literals::date ).toDateTime();

    item.resetPicture( frameRect.size() );

    item.draw( utils::date2str( time, true ), font, Point( 35, 0 ) );
    item.draw( item.text(), font, Point( width() / 2, 0 ) );
  }

  virtual bool onEvent(const NEvent &event)
  {
    bool ret = ListBox::onEvent( event );

    if( event.EventType == sEventMouse )
    {
      switch(event.mouse.type)
      {
      case mouseLbtnRelease: emit onShowMessage( selected() ); break;
      case mouseRbtnRelease: emit onRemoveMessage( selected() ); break;
      default: break;

      case mouseMoved:
      {
        int index = itemAt( event.mouse.pos() );
        if( index >= 0 )
        {
          //ListBoxItem& itemUnderMouse = item((unsigned int) index);

          //bool opened = itemUnderMouse.data( literals::opened );

          //std::string text = opened ? "" : _("##scribemessages_unread##");
          //setTooltipText( text );
          setSelected( index );
        }
      }
      break;
      }
    }

    return ret;
  }
};

class ScribesMessages::Impl
{
public:
  GameAutoPause locker;
  ScribesListBox* lbxMessages;
  PlayerCityPtr city;
  Label* lbInfo;
  TexturedButton* btnExit;
  TexturedButton* btnHelp;
};

ScribesMessages::~ScribesMessages() {}

ScribesMessages::ScribesMessages( Widget* p, PlayerCityPtr city )
  : Window( p, Rect( 0, 0, 480, 320 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();

  setupUI( ":/gui/scribesmessages.gui" );
  setCenter( p->center() );

  WidgetEscapeCloser::insertTo( this );
  _d->lbxMessages = new ScribesListBox( this, Rect( 16, 60, width() - 16, height() - 50 ) );

  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, lbInfo )

  _fillMessages();

  CONNECT( _d->lbxMessages, onShowMessage, this, ScribesMessages::_showMessage );
  CONNECT( _d->lbxMessages, onRemoveMessage, this, ScribesMessages::_removeMessage );
  CONNECT( _d->btnHelp, onClicked(), this, ScribesMessages::_showHelp );
  CONNECT( _d->btnExit, onClicked(), this, ScribesMessages::deleteLater );

  auto event = events::PlaySound::create( "extm_scribes", 1, 100, audio::effects );
  event->dispatch();

  if( _d->lbxMessages ) _d->lbxMessages->setFocus();
  setModal();
}

void ScribesMessages::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Widget::draw( painter );
}

void ScribesMessages::_fillMessages()
{
  _d->lbxMessages->clear();

  const city::Scribes::Messages& messages = _d->city->scribes().messages();
  bool haveMessages = !messages.empty();

  for( auto mt : messages )
  {
    ListBoxItem& item = _d->lbxMessages->addItem( mt.title, Font::create( FONT_1 ) );
    item.setData( literals::opened, mt.opened );
    item.setData( literals::date, mt.date );
    item.setData( literals::ext, mt.ext );
  }

  if( _d->lbInfo && !haveMessages )
  {
    Label* lbDate;
    Label* lbSubj;

    GET_WIDGET_FROM_UI( lbDate )
    GET_WIDGET_FROM_UI( lbSubj )
    lbDate->hide();
    lbSubj->hide();
    _d->lbxMessages->hide();
    _d->lbInfo->setCenter( screenToLocal( center() ) );
    _d->lbInfo->setText( _("##srcw_no_messages##") );
  }
}

void ScribesMessages::_showHelp()
{
  DictionaryWindow::show( this, "scribes_messages" );
}

void ScribesMessages::_showMessage(int index)
{
  city::Scribes::Message mt = _d->city->scribes().getMessage( index );
  _d->city->scribes().readMessage( index );
  Widget* mbox = new infobox::AboutEvent( parent(), mt.title, mt.text, mt.date, mt.gtype );
  mbox->show();

  _fillMessages();
}

void ScribesMessages::_removeMessage(int index)
{
  _d->city->scribes().removeMessage( index );

  _fillMessages();
}

}//end namespace dialog

}//end namespace gui
