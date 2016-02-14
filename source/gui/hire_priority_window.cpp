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

#include "hire_priority_window.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "core/gettext.hpp"
#include "core/event.hpp"
#include "gameautopause.hpp"
#include "widgetescapecloser.hpp"
#include "core/utils.hpp"

#include <set>

namespace gui
{

namespace dialog
{

class HirePriority::Impl
{
public:
  city::industry::Type type;
  int priority;
  std::set<PushButton*> prButtons;

public signals:
  Signal2<city::industry::Type, int> onAcceptPrioritySignal;
};

HirePriority::HirePriority(Widget* p, city::industry::Type type, int priority)
  : Window( p,  Rect( 0, 0, 416, 144 ), "" ), _d( new Impl )
{
  Logger::warning( "HirePriorityWnd: show" );

  _d->type = type;
  _d->priority = priority;

  WidgetClosers::insertTo( this, KEY_RBUTTON );
  GameAutoPauseWidget::insertTo( this );

  Label& lbTitle = add<Label>( Rect( 10, 10, width()-10, 10+35), _("##priority_level##") );
  lbTitle.setFont( FONT_5 );
  lbTitle.setTextAlignment( align::center, align::center );

  Label& lbExit = add<Label>( Rect( 0, height() - 30, width(), height() - 10), _("##right_click_to_exit##") );
  lbExit.setFont( FONT_1 );
  lbExit.setTextAlignment( align::center, align::center );

  Point start( 65, 44 );
  Size btnSize( 28, 28 );
  for( int k=0; k < 9; k++ )
  {
    PushButton& btn = add<PushButton>( Rect( start, btnSize), utils::i2str( k+1 ), k+1, false, PushButton::flatBorderLine );
    btn.setIsPushButton( true );
    btn.setPressed( priority > 0 ? k+1 == priority : false );
    btn.setTooltipText( _("##priority_button_tolltip##") );
    start += Point( btnSize.width() + 5, 0 );
    _d->prButtons.insert( &btn );
  }

  PushButton& noPr = add<PushButton>( Rect( 68, 78, 364, 104 ), _("##no_priority##"), 0, false, PushButton::flatBorderLine );
  _d->prButtons.insert( &noPr );
  noPr.setPressed( priority == 0 );

  moveToCenter();
  setModal();
}

HirePriority::~HirePriority(){}

bool HirePriority::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    if( _d->prButtons.count( static_cast<PushButton*>( event.gui.caller ) ) )
    {
      for( auto btn : _d->prButtons )
      {
        btn->setPressed( btn == event.gui.caller );
      }
      _d->priority = event.gui.caller->ID();
      emit _d->onAcceptPrioritySignal( _d->type, _d->priority );

      deleteLater();

      return true;
    }
  }

  return Widget::onEvent( event );
}

Signal2<city::industry::Type, int>& HirePriority::onAcceptPriority() { return _d->onAcceptPrioritySignal; }

}//end namespace dialog

}//end namespace gui
