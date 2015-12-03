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

#include "environment.hpp"

#include "core/rectangle.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "widgetprivate.hpp"
#include "label.hpp"
#include "core/time.hpp"
#include "core/foreach.hpp"
#include "widget_factory.hpp"
#include "console.hpp"
#include "core/logger.hpp"
#include "core/hash.hpp"
#include "core/osystem.hpp"
#include "widgetprivate.hpp"

using namespace gfx;

namespace gui
{
typedef std::map<Ui::Flag, int> Flags;

struct UiTooltipWorker
{
  WidgetPtr element;
  unsigned int lastTime;
  unsigned int enterTime;
  unsigned int launchTime;
  unsigned int relaunchTime;

  Widget* standart(Widget& parent , Widget *hovered, Point cursor);
  void update( unsigned int time, Widget& rootWidget, bool showTooltips,
               WidgetPtr hovered, Point cursor);
};

class Ui::Impl
{
public:  
  UiTooltipWorker tooltip;
  bool preRenderFunctionCalled;

  WidgetPtr focusedElement;

  struct
  {
    WidgetPtr noSubelement;
    WidgetPtr current;
    Point lastMousePos;
  } hovered;

  Widget::Widgets deletionQueue;

  gfx::Engine* engine;
  Size size;
  Point cursorPos;
  Flags flags;
  int consoleId;
  Console* console;

public:
  void threatDeletionQueue();
};

Ui::Ui(Engine& painter )
  : Widget( 0, -1, Rect() ), _d( new Impl )
{
  setDebugName( "Ui" );

  _d->preRenderFunctionCalled = false;
  _d->focusedElement = 0;
  _d->size = painter.screenSize();
  _d->engine = &painter;
  _dfunc()->environment = this;
  _d->tooltip.element;
  _d->tooltip.lastTime = 0;
  _d->tooltip.enterTime = 0;
  _d->tooltip.launchTime = 1000;
  _d->tooltip.relaunchTime = 500;

  Widget::_dfunc()->environment = this;

  setGeometry( Rect( Point(), _d->size ) );

  _d->consoleId = Hash( TEXT(Console) );
  _d->console = 0;

  setFlag( drawDebugArea, 0 );
}

//! Returns if the element has focus
bool Ui::hasFocus( const Widget* element) const
{
  return ( _d->focusedElement.object() == element );
}

Ui::~Ui() {}

Widget* Ui::rootWidget() { return this; }

void Ui::Impl::threatDeletionQueue()
{
  for( auto widget : deletionQueue )
  {
    try{ widget->remove(); }
    catch(...){}
  }

  deletionQueue.clear();
}

void Ui::clear()
{
  // Remove the focus
  setFocus( this );

  _updateHovered( Point( -9999, -9999 ) );

  for( auto widget : children() )
  {
    if( widget != _d->console )
      deleteLater( widget );
  }
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
    Logger::warning( "!!! WARNING: Call beforeDraw() function needed" );
    return;
  }

  Widget::draw( *_d->engine );  

  if( hasFlag( drawDebugArea ) )
    Widget::debugDraw( *_d->engine );

  _d->tooltip.update( DateTime::elapsedTime(), *this, _d->flags[ showTooltips ],
                      _d->hovered.noSubelement, _d->cursorPos );
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
  return element != NULL ? (_d->hovered.current.object() == element) : false;
}

Widget* Ui::findWidget(int id)
{
  return Widget::findChild( id, true );
}

Widget* Ui::findWidget(const Point &p)
{
  const Widgets& widgets = children();

  for( auto widget : widgets )
  {
    if( widget->visible() && widget->isPointInside( p ) )
      return widget;
  }

  return this;
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

    if( _d->hovered.current.object() == ptrElement || ptrElement->isMyChild( _d->hovered.current.object() ) )
    {
      _d->hovered.current = WidgetPtr();
      _d->hovered.noSubelement = WidgetPtr();
    }

    for( auto widget : _d->deletionQueue )
    {
      if( widget == ptrElement )
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
  return WidgetFactory::instance().create( type, parent );
}

void Ui::setFlag(Ui::Flag name, int value)
{
  _d->flags[ name ] = value;
}

bool Ui::hasFlag(Ui::Flag name)
{
  return _d->flags[ name ];
}

void Ui::_updateHovered( const Point& mousePos )
{
  WidgetPtr lastHovered = _d->hovered.current;
  WidgetPtr lastHoveredNoSubelement = _d->hovered.noSubelement;
  _d->hovered.lastMousePos = mousePos;

  // Get the real Hovered
  _d->hovered.current = rootWidget()->getElementFromPoint( mousePos );

  if( _d->tooltip.element.isValid() && _d->hovered.current == _d->tooltip.element )
  {
    // When the mouse is over the ToolTip we remove that so it will be re-created at a new position.
    // Note that ToolTip.EnterTime does not get changed here, so it will be re-created at once.
    _d->tooltip.element->deleteLater();
    _d->tooltip.element->hide();
    _d->tooltip.element = WidgetPtr();
    _d->hovered.current = rootWidget()->getElementFromPoint( mousePos );
  }

  // for tooltips we want the element itself and not some of it's subelements
  if( _d->hovered.current != rootWidget() )
  {
    _d->hovered.noSubelement = _d->hovered.current;
    while ( _d->hovered.noSubelement.isValid() && _d->hovered.noSubelement->isSubElement() )
    {
      _d->hovered.noSubelement = _d->hovered.noSubelement->parent();
    }
  }
  else
  {
    _d->hovered.noSubelement = 0;
  }

  if( _d->hovered.current != lastHovered )
  {
    if( lastHovered.isValid() )
    {
      lastHovered->onEvent( NEvent::Gui( lastHovered.object(), 0, guiElementLeft ) );
    }

    if( _d->hovered.current.isValid() )
    {
      _d->hovered.current->onEvent( NEvent::Gui( _d->hovered.current.object(), _d->hovered.current.object(), guiElementHovered ) );
    }
  }

  if ( lastHoveredNoSubelement != _d->hovered.noSubelement )
  {
    if( _d->tooltip.element.isValid() )
    {
      _d->tooltip.element->deleteLater();
      _d->tooltip.element = WidgetPtr();
    }

    if( _d->hovered.noSubelement.isValid() )
    {
      _d->tooltip.enterTime = DateTime::elapsedTime();
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
        if( OSystem::isAndroid() )
        {
          _updateHovered( _d->cursorPos );
        }
//!!! end android fix
        switch( event.mouse.type )
        {
        case mouseLbtnPressed:
        case mouseRbtnPressed:
        {
            if ( (_d->hovered.current.isValid() && _d->hovered.current != getFocus()) || !getFocus() )
            {
              setFocus( _d->hovered.current.object() );
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
            if( !inFocus && _d->hovered.current.isValid() )
            {                
              return _d->hovered.current->onEvent(event);
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
          if( _d->hovered.current.isValid() )
          {
            return _d->hovered.current->onEvent( event );
          }
        break;
        }
    break;

    case sEventTextInput:
    case sEventKeyboard:
        {
          if( _d->console )
          {
            if(  _d->console->initKey() == (int)event.keyboard.symbol )
            {
              if( _d->console && !event.keyboard.control && event.keyboard.pressed )
                  _d->console->toggleVisible();

              return true;
            }

            if( _d->console->visible() && !event.keyboard.control && event.keyboard.pressed )
            {
              _d->console->keyPress( event );
              return true;
            }
          }

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

Widget* Ui::hovered() const { return _d->hovered.current.object(); }

void Ui::beforeDraw()
{  
  const Size& screenSize = _d->size;
  const Point& rigthDown = rootWidget()->absoluteRect().rightbottom();

  if( rigthDown.x() != screenSize.width() || rigthDown.y() != screenSize.height() )
  {
    // resize gui environment
    setGeometry( Rect( Point( 0, 0 ), screenSize ) );
  }

  _d->threatDeletionQueue();

  _updateHovered( _d->cursorPos );

  Widgets rchildren = children();

  if( _d->tooltip.element.isValid() )
  {
    _d->tooltip.element->bringToFront();
  }

  for( auto widget : rchildren )
    widget->beforeDraw( *_d->engine );

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

Size Ui::vsize() const {  return size(); }
Point Ui::cursorPos() const {  return _d->cursorPos; }

Widget* UiTooltipWorker::standart(Widget& parent, Widget* hovered, Point cursor)
{
  Label& elm = parent.add<Label>( Rect( 0, 0, 2, 2 ), hovered->tooltipText(), true, Label::bgSimpleWhite );
  elm.setSubElement(true);
  elm.setTextAlignment( align::upperLeft, align::upperLeft );
  elm.setTextOffset( Point( 5, 5 ) );

  Size tooltipSize( elm.textWidth() + 20, elm.textHeight() + 2 );
  if( tooltipSize.width() > parent.width() * 0.75 )
  {
    tooltipSize.setWidth( parent.width() * 0.5 );
    tooltipSize.setHeight( elm.textHeight() * 2 + 10 );
    elm.setWordwrap( true );
  }

  Rect rect( cursor, tooltipSize );

  rect -= Point( tooltipSize.width() + 20, -20 );
  elm.setGeometry( rect );

  return &elm;
}

void UiTooltipWorker::update( unsigned int time, Widget& rootWidget, bool showTooltips,
                              WidgetPtr hovered, Point cursor )
{
  // launch tooltip
  if( element.isNull()
      && hovered.isValid() && hovered.object() != &rootWidget
      && (time - enterTime >= launchTime
      || (time - lastTime >= relaunchTime && time - lastTime < launchTime))
      && hovered->tooltipText().size()
    )
  {
    if( hovered.isValid() )
    {
      NEvent e;
      hovered->onEvent( e );
    }

    element = standart( rootWidget, hovered.object(), cursor );
    element->addProperty( "tooltip", 1 );
    element->setGeometry( element->relativeRect() + Point( 1, 1 ) );
    if( element->screenBottom() > (int)rootWidget.height() ||
        element->screenLeft() < (int)0 )
    {
      Rect geom = element->absoluteRect();
      geom.constrainTo( rootWidget.absoluteRect() );
      element->setGeometry( geom );
      element->setVisible( showTooltips );
    }
  }

  if( element.isValid() && element->visible() )	// (isVisible() check only because we might use visibility for ToolTip one day)
  {
    lastTime = time;

    // got invisible or removed in the meantime?
    if( hovered.isNull()
        || !hovered->visible()
        || !hovered->parent() )
    {
      element->deleteLater();
      element = WidgetPtr();
    }
  }
}

}//end namespace gui
