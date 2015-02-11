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

#include "mainmenu.hpp"
#include "contextmenuitem.hpp"
#include "core/event.hpp"
#include "environment.hpp"

namespace gui
{
#define DEFAULT_MENU_HEIGHT 15

//! constructor
MainMenu::MainMenu( Widget* parent, const Rect& rectangle, const int id )
	: ContextMenu( parent, rectangle, id, false, true)
{
  setCloseHandling( cmIgnore );
#ifdef _DEBUG
  setDebugName( "MainMenu" );
#endif

  setTextAlignment( align::center, align::center );
  setNotClipped(false);
  _recalculateSize();
}

ContextMenuItem* MainMenu::addItem(const std::string& text, int commandId, bool enabled, bool hasSubMenu, bool checked, bool autoChecking)
{
  ContextMenuItem* ret = ContextMenu::addItem( text, commandId, enabled, hasSubMenu, checked, autoChecking );
  if( ret && ret->subMenu() )
  {
    //ret->getSubMenu()->setStyle( getStyle().getSubStyle( NES_SUBMENU ).getName() );
    ret->setFlag( ContextMenuItem::drawSubmenuSprite, false );
    ret->setBackgroundMode( Label::bgNone );
  }
  //refItem.alignEnabled = true;
  //refItem.horizontal = EGUIA_CENTER;
  //refItem.vertical = EGUIA_CENTER;

  return ret;
}

//! called if an event happened.
bool MainMenu::onEvent(const NEvent& event)
{
  if (enabled())
  {
    switch(event.EventType)
    {
    case sEventGui:
      switch(event.gui.type)
      {
      case guiElementFocusLost:
        if (event.gui.caller == this && !isMyChild(event.gui.element))
        {
          _closeAllSubMenus();
          _setHovered( -1 );
        }
        break;
      case guiElementFocused:
        if (event.gui.caller == this )
        {
          bringToFront();
        }
        break;
      default:  break;
      }
      break;

    case sEventMouse:
      switch(event.mouse.type)
      {
      case mouseLbtnPressed:
      {
        if (!ui()->hasFocus(this))
        {
                ui()->setFocus(this);
        }

        bringToFront();

         Point p(event.mouse.pos() );
         bool shouldCloseSubMenu = _hasOpenSubMenu();
         if (!absoluteClippingRect().isPointInside(p))
         {
                 shouldCloseSubMenu = false;
         }
         _isHighlighted( event.mouse.pos(), true);
         if ( shouldCloseSubMenu )
         {
           ui()->removeFocus(this);
         }

         return true;
      }

      case mouseLbtnRelease:
      {
        Point p(event.mouse.pos() );
        if (!absoluteClippingRect().isPointInside(p))
        {
                int t = _sendClick(p);
                if ((t==0 || t==1) && isFocused())
                        removeFocus();
        }

        return true;
      }

      case mouseMoved:
      {
        if (ui()->hasFocus(this) && hovered() >= 0)
        {
          int oldHighLighted = hovered();
          _isHighlighted( event.mouse.pos(), true);
          if ( hovered() < 0 )
          {
            _setHovered( oldHighLighted );   // keep last hightlight active when moving outside the area
          }
        }
        return true;
      }
      default:	break;
    }
    break;
		
    default: break;
    }
  }

  return Widget::onEvent(event);
}

void MainMenu::_recalculateSize()
{
  Rect parentRect = parent()->clientRect(); // client rect of parent

  //AbstractSkin* skin = getEnvironment()->getSkin();
  Font font = Font::create( FONT_2_WHITE );
  
  int hg = std::max<int>( DEFAULT_MENU_HEIGHT, height() );
  setGeometry( Rect( parentRect.UpperLeftCorner.x(), parentRect.UpperLeftCorner.y(),
                                                                           parentRect.LowerRightCorner.x(), parentRect.UpperLeftCorner.y() + hg ) );
  Rect rect;

  rect.UpperLeftCorner = parentRect.UpperLeftCorner;
  hg = std::max<int>( font.getTextSize("A").height(), hg );
  //if (skin && height < skin->getSize ( EGDS_MENU_HEIGHT ))
  //	height = skin->getSize(EGDS_MENU_HEIGHT);
  int width = rect.UpperLeftCorner.x();
  int i;

  for( i=0; i<(int)itemCount(); ++i)
  {
    ContextMenuItem* refItem = item( i );
    if ( refItem->isSeparator() )
    {
            refItem->setDimmension( Size( 16, height() ) );
    }
    else
    {
      Size itemSize = font.getTextSize( refItem->text() ) + Size( 20, 0 );
      itemSize.setHeight( height() );
      refItem->setDimmension( itemSize );
    }

   refItem->setOffset( width );
   width += refItem->dimmension().width();
  }

  // recalculate submenus
  for( i=0; i<(int)itemCount(); ++i )
  {
    ContextMenuItem* refItem = item( i );

    Rect rectangle( refItem->offset(), 0, refItem->offset() + refItem->dimmension().width(), hg );
    refItem->setGeometry( rectangle );

    if( refItem->subMenu() )
    {
      // move submenu
      Size itemSize = refItem->subMenu()->absoluteRect().size();
      refItem->subMenu()->setGeometry( Rect( refItem->offset(), hg,
      refItem->offset() + itemSize.width()-5, hg+itemSize.height() ));
    }
  }
}

}//end namespace gui
