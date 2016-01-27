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
#include "environment.hpp"
#include "widget_helper.hpp"

using namespace gfx;

namespace gui
{

namespace dialog
{

GAME_LITERALCONST(opened)
GAME_LITERALCONST(critical)
GAME_LITERALCONST(ext)
GAME_LITERALCONST(date)

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

    ListBoxItem::OverrideColor& itemfc = item.overrideColors[ ListBoxItem::simple ];
    item.overrideColors[ ListBoxItem::hovered ].font = itemfc.font;
    item.overrideColors[ ListBoxItem::hovered ].Use = true;
    item.overrideColors[ ListBoxItem::hovered ].color = 0xffff0000;

    item.setIcon( gui::rc.panel, gui::message.simple );

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
    int imgIndex = (critical ? gui::message.critial : gui::message.simple) + (opened ? 1 : 0);
    if( imgIndex != lastIndex )
      pic.load( gui::rc.panel, imgIndex );

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
      case NEvent::Mouse::mouseLbtnRelease: emit onShowMessage( selected() ); break;
      case NEvent::Mouse::mouseRbtnRelease: emit onRemoveMessage( selected() ); break;
      default: break;

      case NEvent::Mouse::moved:
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
  ScribesListBox* lbxMessages;
  PlayerCityPtr city;
  Label* lbInfo;
  TexturedButton* btnExit;
};

ScribesMessages::~ScribesMessages() {}

ScribesMessages::ScribesMessages( Widget* p, PlayerCityPtr city )
  : Window( p, Rect( 0, 0, 480, 320 ), "" ), _d( new Impl )
{
  _d->city = city;

  setupUI( ":/gui/scribesmessages.gui" );
  setCenter( p->center() );

  WidgetClose::insertTo( this, KEY_RBUTTON );
  GameAutoPause::insertTo( this );

  _d->lbxMessages = &add<ScribesListBox>( Rect( 16, 60, width() - 16, height() - 50 ) );

  GET_DWIDGET_FROM_UI( _d, lbInfo )

  _fillMessages();

  CONNECT_LOCAL( _d->lbxMessages, onShowMessage, ScribesMessages::_showMessage )
  CONNECT_LOCAL( _d->lbxMessages, onRemoveMessage, ScribesMessages::_removeMessage )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnHelp, onClicked(), ScribesMessages::_showHelp )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnExit, onClicked(), ScribesMessages::deleteLater )

  events::dispatch<events::PlaySound>( "extm_scribes", 1, 100, audio::effects );

  if( _d->lbxMessages )
    _d->lbxMessages->setFocus();

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

void ScribesMessages::_showHelp() { ui()->add<DictionaryWindow>( "scribes_messages" ); }

void ScribesMessages::_showMessage(int index)
{
  city::Scribes::Message mt = _d->city->scribes().getMessage( index );
  _d->city->scribes().readMessage( index );
  ui()->add<infobox::AboutEvent>( mt.title, mt.text, mt.date, mt.gtype );

  _fillMessages();
}

void ScribesMessages::_removeMessage(int index)
{
  _d->city->scribes().removeMessage( index );

  _fillMessages();
}

}//end namespace dialog

}//end namespace gui
