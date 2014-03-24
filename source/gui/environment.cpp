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

#include "environment.hpp"

#include "widgetprivate.hpp"
#include "core/rectangle.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "label.hpp"
#include "core/time.hpp"
#include "core/foreach.hpp"
#include "widget_factory.hpp"

namespace gui
{

class GuiEnv::Impl
{
public:
  struct SToolTip
  {
    WidgetPtr element;
    unsigned int LastTime;
    unsigned int EnterTime;
    unsigned int LaunchTime;
    unsigned int RelaunchTime;
  };


  SToolTip toolTip;
  bool preRenderFunctionCalled;

  WidgetPtr hovered;
  WidgetPtr focusedElement;
  WidgetPtr hoveredNoSubelement;

  Point lastHoveredMousePos;

  Widget::Widgets deletionQueue;

  Rect _desiredRect;
  GfxEngine* engine;
  Point cursorPos;
  WidgetFactory factory;

  WidgetPtr createStandartTooltip( Widget* parent );
  void threatDeletionQueue();
};

GuiEnv::GuiEnv( GfxEngine& painter )
  : Widget( 0, -1, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
  setDebugName( "GuiEnv" );

  _d->preRenderFunctionCalled = false;
  _d->hovered = 0;
  _d->focusedElement = 0;
  _d->hoveredNoSubelement = 0;
  _d->lastHoveredMousePos = Point();
  _d->engine = &painter;

  //INITIALIZE_FILESYSTEM_INSTANCE;

  _environment = this;

  _d->toolTip.element;
  _d->toolTip.LastTime = 0;
  _d->toolTip.EnterTime = 0;
  _d->toolTip.LaunchTime = 1000;
  _d->toolTip.RelaunchTime = 500;

  setGeometry( Rect( 0, 0, painter.screenWidth(), painter.screenHeight() ) );
}

//! Returns if the element has focus
bool GuiEnv::hasFocus( const Widget* element) const
{
    return ( _d->focusedElement.object() == element );
}

GuiEnv::~GuiEnv()
{
}

Widget* GuiEnv::rootWidget()
{
	return this;
}

void GuiEnv::Impl::threatDeletionQueue()
{
  foreach( widget, deletionQueue )
  {
    try{ (*widget)->remove(); }
    catch(...){}
  }

  deletionQueue.clear();
}

void GuiEnv::clear()
{
  // Remove the focus
  setFocus( this );

  updateHoveredElement( Point( -9999, -9999 ) );

  for( ConstChildIterator it = getChildren().begin(); it != getChildren().end(); it++ )
    deleteLater( *it );
}

void GuiEnv::draw()
{
  _CAESARIA_DEBUG_BREAK_IF( !_d->preRenderFunctionCalled && "Called OnPreRender() function needed" );

  Widget::draw( *_d->engine );

  drawTooltip_( DateTime::getElapsedTime() );

  // make sure tooltip is always on top
  //if(_d->toolTip.Element.isValid() )
  //{
  //   _d->toolTip.Element->draw( *_d->engine );
  //}

  _d->preRenderFunctionCalled = false;
}

bool GuiEnv::setFocus( Widget* element )
{
    if( _d->focusedElement == element )
    {
        return false;
    }

    // guard element from being deleted
    // not delete this line
    WidgetPtr saveElement = element;

    // focus may change or be removed in this call
    WidgetPtr currentFocus;
    if( _d->focusedElement.isValid() )
    {
      currentFocus = _d->focusedElement;

      if( _d->focusedElement->onEvent( NEvent::Gui( _d->focusedElement.object(), element, guiElementFocusLost ) ) )
      {
        return false;
      }

      currentFocus = WidgetPtr();
    }

    if( element )
    {
      currentFocus = _d->focusedElement;

      // send focused event
      if( element->onEvent( NEvent::Gui( element, _d->focusedElement.object(), guiElementFocused ) ))
      {
        currentFocus = WidgetPtr();

        return false;
      }
    }

    // element is the new focus so it doesn't have to be dropped
    _d->focusedElement = element;

    return true;
}

Widget* GuiEnv::getFocus() const
{
  return _d->focusedElement.object();
}

bool GuiEnv::isHovered( const Widget* element )
{
  return element != NULL ? (_d->hovered.object() == element) : false;
}

void GuiEnv::deleteLater( Widget* ptrElement )
{
	try
	{
    if( !ptrElement || !isMyChild( ptrElement ) )
		{
			return;
		}

		if( ptrElement == getFocus() || ptrElement->isMyChild( getFocus() ) )
    {
			_d->focusedElement = WidgetPtr();
    }

		if( _d->hovered.object() == ptrElement || ptrElement->isMyChild( _d->hovered.object() ) )
		{
			_d->hovered = WidgetPtr();
			_d->hoveredNoSubelement = WidgetPtr();
		}

    foreach( widget, _d->deletionQueue )
    {
      if( (*widget) == ptrElement )
      {
				return;
      }
    }

		_d->deletionQueue.push_back( ptrElement );
	}
	catch(...)
	{}
}

Widget* GuiEnv::createWidget(const std::string& type, Widget* parent)
{
	return _d->factory.create( type, parent );
}

WidgetPtr GuiEnv::Impl::createStandartTooltip( Widget* parent )
{
  Label* elm = new Label( parent, Rect( 0, 0, 2, 2 ), hoveredNoSubelement->tooltipText(), true, Label::bgSimpleWhite );
  elm->setSubElement(true);

  Size size( elm->getTextWidth() + 20, elm->getTextHeight() + 2 );
  Rect rect( cursorPos, size );

  rect -= Point( size.width() + 20, -20 );
  elm->setGeometry( rect );
  elm->setTextAlignment( alignCenter, alignCenter );

  return elm;
}

//
void GuiEnv::drawTooltip_( unsigned int time )
{
    // launch tooltip
    if ( _d->toolTip.element.isNull()
         && _d->hoveredNoSubelement.isValid() && _d->hoveredNoSubelement.object() != rootWidget()
    		 && (time - _d->toolTip.EnterTime >= _d->toolTip.LaunchTime
         || (time - _d->toolTip.LastTime >= _d->toolTip.RelaunchTime && time - _d->toolTip.LastTime < _d->toolTip.LaunchTime))
		     && _d->hoveredNoSubelement->tooltipText().size()
        )
    {
      if( _d->hoveredNoSubelement.isValid() )
      {
        NEvent e;
        _d->hoveredNoSubelement->onEvent( e );
      }

      _d->toolTip.element = _d->createStandartTooltip( this );
      _d->toolTip.element->setGeometry( _d->toolTip.element->getRelativeRect() + Point( 1, 1 ) );
    }

    if( _d->toolTip.element.isValid() && _d->toolTip.element->isVisible() )	// (isVisible() check only because we might use visibility for ToolTip one day)
    {
      _d->toolTip.LastTime = time;

      // got invisible or removed in the meantime?
      if( _d->hoveredNoSubelement.isNull()
          || !_d->hoveredNoSubelement->isVisible() 
          || !_d->hoveredNoSubelement->getParent() )
      {
        _d->toolTip.element->deleteLater();
        _d->toolTip.element = WidgetPtr();
      }
    }
}

void GuiEnv::updateHoveredElement( const Point& mousePos )
{
  WidgetPtr lastHovered = _d->hovered;
  WidgetPtr lastHoveredNoSubelement = _d->hoveredNoSubelement;
  _d->lastHoveredMousePos = mousePos;

	// Get the real Hovered
  _d->hovered = rootWidget()->getElementFromPoint( mousePos );

  if( _d->toolTip.element.isValid() && _d->hovered == _d->toolTip.element )
  {
    // When the mouse is over the ToolTip we remove that so it will be re-created at a new position.
    // Note that ToolTip.EnterTime does not get changed here, so it will be re-created at once.
    _d->toolTip.element->deleteLater();
    _d->toolTip.element->hide();
    _d->toolTip.element = WidgetPtr();
    _d->hovered = rootWidget()->getElementFromPoint( mousePos );
  }

  // for tooltips we want the element itself and not some of it's subelements
  if( _d->hovered != rootWidget() )
	{
		_d->hoveredNoSubelement = _d->hovered;
		while ( _d->hoveredNoSubelement.isValid() && _d->hoveredNoSubelement->isSubElement() )
		{
			_d->hoveredNoSubelement = _d->hoveredNoSubelement->getParent();
		}
	}
  else
	{
    _d->hoveredNoSubelement = 0;
	}

  if( _d->hovered != lastHovered )
  {
    if( lastHovered.isValid() )
		{
			lastHovered->onEvent( NEvent::Gui( lastHovered.object(), 0, guiElementLeft ) );
		}

    if( _d->hovered.isValid() )
		{
			_d->hovered->onEvent( NEvent::Gui( _d->hovered.object(), _d->hovered.object(), guiElementHovered ) );
		}
  }

  if ( lastHoveredNoSubelement != _d->hoveredNoSubelement )
  {
    if( _d->toolTip.element.isValid() )
    {
      _d->toolTip.element->deleteLater();
      _d->toolTip.element = WidgetPtr();
    }

    if( _d->hoveredNoSubelement.isValid() )
    {
      _d->toolTip.EnterTime = DateTime::getElapsedTime();
    }
  }
}

//! Returns the next element in the tab group starting at the focused element
Widget* GuiEnv::getNextWidget(bool reverse, bool group)
{
    // start the search at the root of the current tab group
    Widget *startPos = getFocus() ? getFocus()->tabgroup() : 0;
    int startOrder = -1;

    // if we're searching for a group
    if (group && startPos)
    {
        startOrder = startPos->getTabOrder();
    }
    else
        if (!group && getFocus() && !getFocus()->hasTabgroup())
        {
            startOrder = getFocus()->getTabOrder();
            if (startOrder == -1)
            {
                // this element is not part of the tab cycle,
                // but its parent might be...
                Widget *el = getFocus();
                while (el && el->getParent() && startOrder == -1)
                {
                    el = el->getParent();
                    startOrder = el->getTabOrder();
                }

            }
        }

        if (group || !startPos)
            startPos = rootWidget(); // start at the root

        // find the element
        Widget *closest = 0;
        Widget *first = 0;
        startPos->getNextWidget(startOrder, reverse, group, first, closest);

        if (closest)
            return closest; // we found an element
        else if (first)
            return first; // go to the end or the start
        else if (group)
            return rootWidget(); // no group found? root group
        else
            return 0;
}

//! posts an input event to the environment
bool GuiEnv::handleEvent( const NEvent& event )
{
  switch(event.EventType)
  {
    case sEventGui:
        // hey, why is the user sending gui events..?
        break;

    case sEventMouse:
        _d->cursorPos = event.mouse.pos();
        switch( event.mouse.type )
        {
        case mouseLbtnPressed:
        case mouseRbtnPressed:
            if ( (_d->hovered.isValid() && _d->hovered != getFocus()) || !getFocus() )
            {
                setFocus( _d->hovered.object() );
            }

            // sending input to focus
            if (getFocus() && getFocus()->onEvent(event))
                return true;

            // focus could have died in last call
            if (!getFocus() && _d->hovered.isValid() )
            {

                return _d->hovered->onEvent(event);
            }
        break;

        case mouseLbtnRelease:
//             if( _dragObjectSave && _hovered )
//             {
//                 _hovered->onEvent( NEvent::Drop( event.MouseEvent.getPosition(), _dragObjectSave ) );
//                 _dragObjectSave = NULL;
//             }
//             else
                if( getFocus() )
                    return getFocus()->onEvent( event );
        break;

        default:
            if( _d->hovered.isValid() )
            {
                return _d->hovered->onEvent( event );
            }
        break;
        }
    break;

    case sEventKeyboard:
        {
            /*if( _console && _console->InitKey() == (int)event.KeyboardEvent.Char )							//
            {
                if( _console && !event.KeyboardEvent.Control && event.KeyboardEvent.PressedDown )
                    _console->ToggleVisible();

                return true;
            }*/

            /*if( _console && _console->isVisible() && !event.KeyboardEvent.Control && event.KeyboardEvent.PressedDown )						//
            {																//
                _console->KeyPress( event );
                return true;
            }*/

            if (getFocus() && getFocus()->onEvent(event))
                return true;

            // For keys we handle the event before changing focus to give elements the chance for catching the TAB
            // Send focus changing event
            if( event.EventType == sEventKeyboard &&
                event.keyboard.pressed &&
                event.keyboard.key == KEY_TAB)
            {
                Widget *next = getNextWidget(event.keyboard.shift, event.keyboard.control);
                if (next && next != getFocus())
                {
                    if (setFocus(next))
                        return true;
                }
            }

        }
        break;

//     case NRP_LOG_TEXT_EVENT:								//
//         {
//             /*if( _console )
//                 _console->AppendMessage( String::fromCharArray( event.LogEvent.Text ) ); //
//             return false;
// 			*/
//         }
//     break;

    default:
        break;
  } // end switch


  return false;
}

Widget* GuiEnv::getHoveredElement() const {  return _d->hovered.object(); }

void GuiEnv::beforeDraw()
{
  const Size screenSize( _d->engine->getScreenSize() );
  const Point rigthDown = rootWidget()->absoluteRect().LowerRightCorner;
  
  if( rigthDown.x() != screenSize.width() || rigthDown.y() != screenSize.height() )
  {
    // resize gui environment
    setGeometry( Rect( Point( 0, 0 ), screenSize ) );
  }

  _d->threatDeletionQueue();

  updateHoveredElement( _d->cursorPos );

  foreach( widget, Widget::_d->children )
  {
    (*widget)->beforeDraw( *_d->engine );
  }

  if( _d->toolTip.element.isValid() )
  {
    _d->toolTip.element->bringToFront();
  }

  _d->preRenderFunctionCalled = true;
}

bool GuiEnv::removeFocus( Widget* element)
{
  if( _d->focusedElement.isValid() && _d->focusedElement == element )
  {
    if( _d->focusedElement->onEvent( NEvent::Gui( _d->focusedElement.object(),  0, guiElementFocusLost )) )
    {
      return false;
    }
  }

  if( _d->focusedElement.isValid() )
  {
    _d->focusedElement = WidgetPtr();
  }

  return true;
}

void GuiEnv::animate( unsigned int time )
{
  Widget::animate( time );
}

Point GuiEnv::getCursorPos() const {  return _d->cursorPos; }

}//end namespace gui
