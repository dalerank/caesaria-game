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
#include "city/cityservice_info.hpp"
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
#include "widget_helper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace {
CAESARIA_LITERALCONST(opened)
CAESARIA_LITERALCONST(critical)
CAESARIA_LITERALCONST(ext)
CAESARIA_LITERALCONST(date)
}

class ScribesListBox : public ListBox
{
public:
  ScribesListBox( Widget* p, const Rect& rect ) : ListBox( p, rect )
  {
    setFlag( selectOnMove, true );
  }

  virtual ListBoxItem& addItem( const std::string& text, Font font=Font(), const int color=0 )
  {
    ListBoxItem& item = ListBox::addItem( text, font, color );

    ListBoxItem::OverrideColor& itemfc = item.OverrideColors[ ListBoxItem::simple ];
    item.OverrideColors[ ListBoxItem::hovered ].font = itemfc.font;
    item.OverrideColors[ ListBoxItem::hovered ].Use = true;
    item.OverrideColors[ ListBoxItem::hovered ].color = 0xffff0000;

    item.setIcon( Picture::load( ResourceGroup::panelBackground, 111 ));

    return item;
  }

public signals:
  Signal1<int> onShowMessage;
  Signal1<int> onRemoveMessage;

protected:
  virtual void _drawItemIcon(gfx::Engine& painter, ListBoxItem& item, const Point& pos, Rect* clipRect)
  {
    VariantMap options = item.data().toMap();
    bool opened = options.get( lc_opened, false );
    bool critical = options.get( lc_critical, false );
    int imgIndex = (critical ? 113 : 111) + (opened ? 1 : 0);
    painter.draw( Picture::load( ResourceGroup::panelBackground, imgIndex ), pos + Point( 2, 2) );
  }

  virtual void _updateItemText(Engine& painter, ListBoxItem& item, const Rect& textRect, Font font, const Rect& frameRect)
  {
    VariantMap options = item.data().toMap();
    DateTime time = options[ lc_date ].toDateTime();

    item.draw( util::date2str( time ), font, Point( 30, 0 ) );
    item.draw( item.text(), font, Point( width() / 2, 0 ));
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
          ListBoxItem& itemUnderMouse = item( index );

          VariantMap options = itemUnderMouse.data().toMap();
          bool opened = options.get( lc_opened, false );

          std::string text = opened ? "" : _("##scribemessages_unread##");
          setTooltipText( text );
        }
      }
      break;
      }
    }

    return ret;
  }
};

class ScribesMessagestWindow::Impl
{
public:
  GameAutoPause locker;
  ScribesListBox* lbxMessages;
  PlayerCityPtr city;
  Label* lbInfo;
  TexturedButton* btnExit;
  TexturedButton* btnHelp;
};

ScribesMessagestWindow::~ScribesMessagestWindow() {}

ScribesMessagestWindow::ScribesMessagestWindow( Widget* p, PlayerCityPtr city )
  : Window( p, Rect( 0, 0, 480, 320 ), "" ), _d( new Impl )
{
  _d->city = city;
  _d->locker.activate();

  setupUI( ":/gui/scribesmessages.gui" );
  setCenter( p->center() );

  WidgetEscapeCloser::insertTo( this );

  WidgetEscapeCloser::insertTo( this );
  _d->lbxMessages = new ScribesListBox( this, Rect( 16, 60, width() - 16, height() - 50 ) );

  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, lbInfo )

  _fillMessages();

  CONNECT( _d->lbxMessages, onShowMessage, this, ScribesMessagestWindow::_showMessage );
  CONNECT( _d->lbxMessages, onRemoveMessage, this, ScribesMessagestWindow::_removeMessage );
  CONNECT( _d->btnExit, onClicked(), this, ScribesMessagestWindow::deleteLater );
  CONNECT( _d->btnHelp, onClicked(), this, ScribesMessagestWindow::_showHelp );
}

void ScribesMessagestWindow::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Widget::draw( painter );
}

void ScribesMessagestWindow::_fillMessages()
{
  _d->lbxMessages->clear();

  city::InfoPtr srvc;
  srvc << _d->city->findService( city::Info::defaultName() );

  bool haveMessages = false;
  if( srvc.isValid() )
  {

    const city::Info::Messages& messages = srvc->messages();
    haveMessages = !messages.empty();
    foreach( it, messages )
    {
      const city::Info::ScribeMessage& mt = *it;
      ListBoxItem& item = _d->lbxMessages->addItem( mt.title );
      VariantMap options;
      options[ lc_opened ] = mt.opened;
      options[ lc_date   ] = mt.date;
      options[ lc_ext    ] = mt.ext;

      item.setData( options );      
    }
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

void ScribesMessagestWindow::_showHelp()
{
  DictionaryWindow::show( this, "scribes_messages" );
}

void ScribesMessagestWindow::_showMessage(int index)
{
  city::InfoPtr srvc;
  srvc << _d->city->findService( city::Info::defaultName() );

  if( srvc.isValid() )
  {
    city::Info::ScribeMessage mt = srvc->getMessage( index );
    mt.opened = true;
    srvc->changeMessage( index, mt );
    EventMessageBox* mbox = new EventMessageBox( parent(), mt.title, mt.text, mt.date, mt.gtype );
    mbox->show();
  }

  _fillMessages();
}

void ScribesMessagestWindow::_removeMessage(int index)
{
  city::InfoPtr srvc;
  srvc << _d->city->findService( city::Info::defaultName() );

  if( srvc.isValid() )
  {
    srvc->removeMessage( index );
  }

  _fillMessages();
}

}//end namespace gui
