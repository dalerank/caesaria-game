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

#include "environment.hpp"

#include "core/rectangle.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "label.hpp"
#include "core/time.hpp"
#include "core/foreach.hpp"
#include "widget_factory.hpp"
#include "core/logger.hpp"

using namespace gfx;

namespace gui
{

class Ui::Impl
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

  gfx::Engine* engine;
  Point cursorPos;
  WidgetFactory factory;

  WidgetPtr createStandartTooltip( Widget* parent );
  void threatDeletionQueue();
};

Ui::Ui(Engine& painter )
  : Widget( 0, -1, Rect() ), _d( new Impl )
{
  setDebugName( "GuiEnv" );

  _d->preRenderFunctionCalled = false;
  _d->hovered = 0;
  _d->focusedElement = 0;
  _d->hoveredNoSubelement = 0;
  _d->lastHoveredMousePos = Point();
  _d->engine = &painter;
  _environment = this;
  _d->toolTip.element;
  _d->toolTip.LastTime = 0;
  _d->toolTip.EnterTime = 0;
  _d->toolTip.LaunchTime = 1000;
  _d->toolTip.RelaunchTime = 500;

  setGeometry( Rect( Point(), painter.virtualSize() ) );
}

//! Returns if the element has focus
bool Ui::hasFocus( const Widget* element) const
{
  return ( _d->focusedElement.object() == element );
}

Ui::~Ui() {}

Widget* Ui::rootWidget() {	return this; }

void Ui::Impl::threatDeletionQueue()
{
  foreach( widget, deletionQueue )
  {
    try{ (*widget)->remove(); }
    catch(...){}
  }

  deletionQueue.clear();
}

void Ui::clear()
{
  // Remove the focus
  setFocus( this );

  _updateHovered( Point( -9999, -9999 ) );

  foreach( it, children() )
    deleteLater( *it );
}

void Ui::setFocus() {}
void Ui::removeFocus() {}
void Ui::beforeDraw(Engine& painter) {}
void Ui::draw(Engine& painter) {}
bool Ui::isHovered() const { return false; }

void Ui::draw()
{
  if( !_d->preRenderFunctionCalled )
  {
    Logger::warning( "Call beforeDraw() function needed" );
    return;
  }

  Widget::draw( *_d->engine );

  _drawTooltip( DateTime::elapsedTime() );
  _d->preRenderFunctionCalled = false;
}

bool Ui::setFocus( Widget* element )
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

Widget* Ui::getFocus() const { return _d->focusedElement.object(); }

bool Ui::isHovered( const Widget* element )
{
  return element != NULL ? (_d->hovered.object() == element) : false;
}

Widget* Ui::findWidget(int id)
{
  return Widget::findChild( id, true );
}

void Ui::deleteLater( Widget* ptrElement )
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

Widget* Ui::createWidget(const std::string& type, Widget* parent)
{
  return _d->factory.create( type, parent );
}

WidgetPtr Ui::Impl::createStandartTooltip( Widget* parent )
{
  Label* elm = new Label( parent, Rect( 0, 0, 2, 2 ), hoveredNoSubelement->tooltipText(), true, Label::bgSimpleWhite );
  elm->setSubElement(true);
  elm->setTextAlignment( align::upperLeft, align::upperLeft );
  elm->setTextOffset( Point( 5, 5 ) );

  Size tlpSize( elm->textWidth() + 20, elm->textHeight() + 2 );
  if( tlpSize.width() > parent->width() * 0.75 )
  {
    tlpSize.setWidth( parent->width() * 0.5 );
    tlpSize.setHeight( elm->textHeight() * 2 + 10 );
    elm->setWordwrap( true );
  }

  Rect rect( cursorPos, tlpSize );

  rect -= Point( tlpSize.width() + 20, -20 );
  elm->setGeometry( rect );

  return elm;
}

void Ui::_drawTooltip( unsigned int time )
{
  // launch tooltip
  if( _d->toolTip.element.isNull()
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
    _d->toolTip.element->setGeometry( _d->toolTip.element->relativeRect() + Point( 1, 1 ) );
    if( _d->toolTip.element->screenBottom() > (int)height() ||
         _d->toolTip.element->screenLeft() < (int)0 )
    {
      Rect geom = _d->toolTip.element->absoluteRect();
      geom.constrainTo( absoluteRect() );
      _d->toolTip.element->setGeometry( geom );
    }
  }

  if( _d->toolTip.element.isValid() && _d->toolTip.element->visible() )	// (isVisible() check only because we might use visibility for ToolTip one day)
  {
    _d->toolTip.LastTime = time;

    // got invisible or removed in the meantime?
    if( _d->hoveredNoSubelement.isNull()
        || !_d->hoveredNoSubelement->visible()
        || !_d->hoveredNoSubelement->parent() )
    {
      _d->toolTip.element->deleteLater();
      _d->toolTip.element = WidgetPtr();
    }
  }
}

void Ui::_updateHovered( const Point& mousePos )
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
      _d->hoveredNoSubelement = _d->hoveredNoSubelement->parent();
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
      _d->toolTip.EnterTime = DateTime::elapsedTime();
    }
  }
}

//! Returns the next element in the tab group starting at the focused element
Widget* Ui::next(bool reverse, bool group)
{
  // start the search at the root of the current tab group
  Widget *startPos = getFocus() ? getFocus()->tabgroup() : 0;
  int startOrder = -1;

  // if we're searching for a group
  if (group && startPos)
  {
      startOrder = startPos->tabOrder();
  }
  else if (!group && getFocus() && !getFocus()->hasTabgroup())
  {
      startOrder = getFocus()->tabOrder();
      if (startOrder == -1)
      {
          // this element is not part of the tab cycle,
          // but its parent might be...
          Widget *el = getFocus();
          while (el && el->parent() && startOrder == -1)
          {
              el = el->parent();
              startOrder = el->tabOrder();
          }

      }
  }

  if (group || !startPos)
      startPos = rootWidget(); // start at the root

  // find the element
  Widget *closest = 0;
  Widget *first = 0;
  startPos->next(startOrder, reverse, group, first, closest);

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
bool Ui::handleEvent( const NEvent& event )
{  
  switch(event.EventType)
  {
    case sEventGui:
      // hey, why is the user sending gui events..?
    break;

    case sEventMouse:
        _d->cursorPos = event.mouse.pos();

//!!! android fix. update hovered element on every mouse event,
//!   that beforeDraw() function cannot do it correctly
#ifdef CAESARIA_PLATFORM_ANDROID
        _updateHovered( _d->cursorPos );
#endif
//!!! end android fix
        switch( event.mouse.type )
        {
        case mouseLbtnPressed:
        case mouseRbtnPressed:
        {
            if ( (_d->hovered.isValid() && _d->hovered != getFocus()) || !getFocus() )
            {
              setFocus( _d->hovered.object() );
            }

            // sending input to focus
            Widget* inFocus = getFocus();
            if( inFocus )
            {
              bool eventResolved = getFocus()->onEvent(event);
              if( eventResolved )
              {
                return true;
              }
            }

            // focus could have died in last call
            inFocus = getFocus();
            if( !inFocus && _d->hovered.isValid() )
            {                
              return _d->hovered->onEvent(event);
            }
        }
        break;

        case mouseLbtnRelease:
          if( getFocus() )
          {
            return getFocus()->onEvent( event );
          }
        break;

        default:
          if( _d->hovered.isValid() )
          {
            return _d->hovered->onEvent( event );
          }
        break;
        }
    break;

    case sTextInput:
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

          if( getFocus() && getFocus()->onEvent(event))
            return true;

          // For keys we handle the event before changing focus to give elements the chance for catching the TAB
          // Send focus changing event
          if( event.EventType == sEventKeyboard &&
              event.keyboard.pressed &&
              event.keyboard.key == KEY_TAB)
          {
            Widget *wdg = next(event.keyboard.shift, event.keyboard.control);
            if (wdg && wdg != getFocus())
            {
                if( setFocus(wdg) )
                    return true;
            }
          }
        }
        break;
    default:
        break;
  } // end switch


  return false;
}

Widget* Ui::hovered() const {  return _d->hovered.object(); }

void Ui::beforeDraw()
{
  const Size screenSize( _d->engine->virtualSize() );
  const Point& rigthDown = rootWidget()->absoluteRect().LowerRightCorner;
  
  if( rigthDown.x() != screenSize.width() || rigthDown.y() != screenSize.height() )
  {
    // resize gui environment
    setGeometry( Rect( Point( 0, 0 ), screenSize ) );
  }

  _d->threatDeletionQueue();

  _updateHovered( _d->cursorPos );

  const Widgets& rchildren = children();
  foreach( i, rchildren )
  {
    (*i)->beforeDraw( *_d->engine );
  }

  if( _d->toolTip.element.isValid() )
  {
    _d->toolTip.element->bringToFront();
  }

  _d->preRenderFunctionCalled = true;
}

bool Ui::removeFocus( Widget* element)
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

void Ui::animate( unsigned int time )
{
  Widget::animate( time );
}

Point Ui::cursorPos() const {  return _d->cursorPos; }

}//end namespace gui
