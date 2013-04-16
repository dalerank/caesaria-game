// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_guienv.hpp"

#include "oc3_widgetprivate.hpp"
#include "oc3_rectangle.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_event.hpp"
#include "oc3_label.hpp"
#include "oc3_time.hpp"

class GuiEnv::Impl
{
public:
    struct SToolTip
    {
        Widget* Element;
        Uint32 LastTime;
        Uint32 EnterTime;
        Uint32 LaunchTime;
        Uint32 RelaunchTime;
    };

    SToolTip toolTip;
    bool preRenderFunctionCalled;

    Widget* hovered;
    Widget* focusedElement;
    Widget* hoveredNoSubelement;

    Point lastHoveredMousePos;

    Widget::Widgets deletionQueue;

    Rect _desiredRect;
	GfxEngine* engine;
	Point cursorPos;
};

GuiEnv::GuiEnv( GfxEngine& engine )
: Widget( 0, -1, Rect( Point( 0, 0), engine.getScreenSize() ) ),
 _d( new Impl )
{
    setDebugName( "GuiEnv" );

    _d->preRenderFunctionCalled = false;
    _d->hovered = 0;
    _d->focusedElement = 0;
    _d->hoveredNoSubelement = 0;
    _d->lastHoveredMousePos = Point();
	_d->engine = &engine;

    //INITIALIZE_FILESYSTEM_INSTANCE;

    _environment = this;

    _d->toolTip.Element = 0;
    _d->toolTip.LastTime = 0;
    _d->toolTip.EnterTime = 0;
    _d->toolTip.LaunchTime = 1000;
    _d->toolTip.RelaunchTime = 500;
}

//! Returns if the element has focus
bool GuiEnv::hasFocus( const Widget* element) const
{
    return (element == _d->focusedElement);
}

GuiEnv::~GuiEnv()
{
}

Widget* GuiEnv::getRootWidget()
{
	return this;
}

void GuiEnv::threatDeletionQueue_()
{
    Widget::ChildIterator it = _d->deletionQueue.begin();
    for( ; it != _d->deletionQueue.end(); it++ )
    {
        try{ (*it)->remove(); }
        catch(...){}
    }

    _d->deletionQueue.clear();
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
    _OC3_DEBUG_BREAK_IF( !_d->preRenderFunctionCalled && "Called OnPreRender() function needed" );

	Widget::draw( *_d->engine );

    drawTooltip_( DateTime::getElapsedTime() );

    // make sure tooltip is always on top
    if(_d->toolTip.Element)
       _d->toolTip.Element->draw( *_d->engine );

    _d->preRenderFunctionCalled = false;
}

bool GuiEnv::setFocus( Widget* element )
{
    if (_d->focusedElement == element)
    {
        return false;
    }

    // GUI Environment should not get the focus
    if (element == this)
        element = 0;

    // stop element from being deleted
    if (element)
        element->grab();

    // focus may change or be removed in this call
    Widget *currentFocus = 0;
    if (_d->focusedElement)
    {
        currentFocus = _d->focusedElement;
        currentFocus->grab();

        if( _d->focusedElement->onEvent( NEvent::Gui( _d->focusedElement, element, OC3_ELEMENT_FOCUS_LOST ) ) )
        {
            if (element)
                element->drop();
            currentFocus->drop();

            return false;
        }

        currentFocus->drop();
        currentFocus = 0;
    }

    if (element)
    {
        currentFocus = _d->focusedElement;
        if (currentFocus)
            currentFocus->grab();

        // send focused event
        if( element->onEvent( NEvent::Gui( element, _d->focusedElement, OC3_ELEMENT_FOCUSED ) ))
        {
            if (element)
                element->drop();
            if (currentFocus)
                currentFocus->drop();

            return false;
        }
    }

    if (currentFocus)
        currentFocus->drop();

    if (_d->focusedElement)
        _d->focusedElement->drop();

    // element is the new focus so it doesn't have to be dropped
    _d->focusedElement = element;

    return true;}

Widget* GuiEnv::getFocus() const
{
	return _d->focusedElement;
}

bool GuiEnv::isHovered( const Widget* element )
{
	return element != NULL ? (element == _d->hovered) : false;
}

void GuiEnv::deleteLater( Widget* ptrElement )
{
	try
	{
        if( !ptrElement || !isMyChild( ptrElement ) )
		{
            //std::out << L"Elm isn't my child" << core::MsgTerminator();
			return;
		}

		if( ptrElement == getFocus() || ptrElement->isMyChild( getFocus() ) )
			_d->focusedElement = 0;

		if( _d->hovered == ptrElement || ptrElement->isMyChild( _d->hovered ) )
		{
			_d->hovered = 0;
			_d->hoveredNoSubelement = 0;
		}

        Widget::ChildIterator it = _d->deletionQueue.begin();
        for( ; it != _d->deletionQueue.end(); it++ )
        {
			if( *it == ptrElement )
				return;
        }

		_d->deletionQueue.push_back( ptrElement );
	}
	catch(...)
	{}
}

Widget* GuiEnv::createStandartTooltip_()
{
    Font styleFont = FontCollection::instance().getFont( FONT_2 );

    Label* elm = new Label( this, Rect( 0, 0, 2, 2 ), _d->hoveredNoSubelement->getTooltipText(), true, true );
    elm->setSubElement(true);
    elm->grab();

    Size size( elm->getTextWidth(), elm->getTextHeight() );
    Rect rect( _d->cursorPos, size );
    //rect.constrainTo( getAbsoluteRect() );
    rect -= Point( size.getWidth() + 20, -20 );
    elm->setGeometry( rect );

    return elm;
}

//
void GuiEnv::drawTooltip_( unsigned int time )
{
    // launch tooltip
    if ( _d->toolTip.Element == 0
         && _d->hoveredNoSubelement && _d->hoveredNoSubelement != getRootWidget()
		 && (time - _d->toolTip.EnterTime >= _d->toolTip.LaunchTime
         || (time - _d->toolTip.LastTime >= _d->toolTip.RelaunchTime && time - _d->toolTip.LastTime < _d->toolTip.LaunchTime))
		 && _d->hoveredNoSubelement->getTooltipText().size()
        )
    {
        //
		//AbstractFont* font = FontManager::instance().getFont( ElementStyleNames[ NES_TOOLTIP ] );
        Widget* selfToolTip = NULL;

        if( _d->hoveredNoSubelement )
        {
            NEvent e;
            _d->hoveredNoSubelement->onEvent( e );
        }

        //
        if( !selfToolTip )
            selfToolTip = createStandartTooltip_();

        _d->toolTip.Element = selfToolTip;
        selfToolTip->setGeometry( selfToolTip->getRelativeRect() + Point( 1, 1 ) );
    }

    if( _d->toolTip.Element && _d->toolTip.Element->isVisible() )	// (isVisible() check only because we might use visibility for ToolTip one day)
    {
        _d->toolTip.LastTime = time;

        // got invisible or removed in the meantime?
        if( !_d->hoveredNoSubelement ||
            !_d->hoveredNoSubelement->isVisible() ||
            !_d->hoveredNoSubelement->getParent()
            )	// got invisible or removed in the meantime?
        {
            _d->toolTip.Element->deleteLater();
            _d->toolTip.Element = 0;
        }
    }
}

void GuiEnv::updateHoveredElement( const Point& mousePos )
{
    Widget* lastHovered = _d->hovered;
    Widget* lastHoveredNoSubelement = _d->hoveredNoSubelement;
    _d->lastHoveredMousePos = mousePos;

	// Get the real Hovered
    _d->hovered = getRootWidget()->getElementFromPoint( mousePos );

    if ( _d->toolTip.Element && _d->hovered == _d->toolTip.Element )
    {
        // When the mouse is over the ToolTip we remove that so it will be re-created at a new position.
        // Note that ToolTip.EnterTime does not get changed here, so it will be re-created at once.
        _d->toolTip.Element->deleteLater();
        _d->toolTip.Element->hide();
        _d->toolTip.Element = 0;
        _d->hovered = getRootWidget()->getElementFromPoint( mousePos );
    }

    // for tooltips we want the element itself and not some of it's subelements
    if( _d->hovered != getRootWidget() )
	{
		_d->hoveredNoSubelement = _d->hovered;
		while ( _d->hoveredNoSubelement && _d->hoveredNoSubelement->isSubElement() )
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
        if( lastHovered )
		{
            lastHovered->onEvent( NEvent::Gui( lastHovered, 0, OC3_ELEMENT_LEFT ) );
			lastHovered->drop();
		}

        if( _d->hovered )
		{
            _d->hovered->onEvent( NEvent::Gui( _d->hovered, _d->hovered, OC3_ELEMENT_HOVERED ) );
			_d->hovered->grab();
		}
    }

    if ( lastHoveredNoSubelement != _d->hoveredNoSubelement )
    {
        if( _d->toolTip.Element )
        {
            _d->toolTip.Element->deleteLater();
            _d->toolTip.Element = 0;
        }

        if( _d->hoveredNoSubelement )
        {
            _d->toolTip.EnterTime = DateTime::getElapsedTime();
        }
    }
}

//! Returns the next element in the tab group starting at the focused element
Widget* GuiEnv::getNextWidget(bool reverse, bool group)
{
    // start the search at the root of the current tab group
    Widget *startPos = getFocus() ? getFocus()->getTabGroup() : 0;
    int startOrder = -1;

    // if we're searching for a group
    if (group && startPos)
    {
        startOrder = startPos->getTabOrder();
    }
    else
        if (!group && getFocus() && !getFocus()->hasTabGroup())
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
            startPos = getRootWidget(); // start at the root

        // find the element
        Widget *closest = 0;
        Widget *first = 0;
        startPos->getNextWidget(startOrder, reverse, group, first, closest);

        if (closest)
            return closest; // we found an element
        else if (first)
            return first; // go to the end or the start
        else if (group)
            return getRootWidget(); // no group found? root group
        else
            return 0;
}

//! posts an input event to the environment
bool GuiEnv::handleEvent( const NEvent& event )
{
    switch(event.EventType)
    {
    case OC3_GUI_EVENT:
        // hey, why is the user sending gui events..?
        break;

    case OC3_MOUSE_EVENT:
        _d->cursorPos = event.MouseEvent.getPosition();
        switch( event.MouseEvent.Event )
        {
        case OC3_LMOUSE_PRESSED_DOWN:
        case OC3_RMOUSE_PRESSED_DOWN:
            if ( (_d->hovered && _d->hovered != getFocus()) || !getFocus() )
            {
                setFocus(_d->hovered);
            }

            // sending input to focus
            if (getFocus() && getFocus()->onEvent(event))
                return true;

            // focus could have died in last call
            if (!getFocus() && _d->hovered)
            {

                return _d->hovered->onEvent(event);
            }
        break;

        case OC3_LMOUSE_LEFT_UP:
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
            if( _d->hovered )
                return _d->hovered->onEvent( event );
        break;
        }
    break;

    case OC3_KEYBOARD_EVENT:
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
            if (event.EventType == OC3_KEYBOARD_EVENT &&
                event.KeyboardEvent.PressedDown &&
                event.KeyboardEvent.Key == KEY_TAB)
            {
                Widget *next = getNextWidget(event.KeyboardEvent.Shift, event.KeyboardEvent.Control);
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

Widget* GuiEnv::_CheckParent( Widget* parent )
{
    return parent ? parent : this;
}

Widget* GuiEnv::getHoveredElement() const
{
    return _d->hovered;
}

void GuiEnv::beforeDraw()
{
	const Size screenSize( _d->engine->getScreenSize() );
	const Point rigthDown = getRootWidget()->getAbsoluteRect().LowerRightCorner;
	if( rigthDown.getX() != screenSize.getWidth() || rigthDown.getY() != screenSize.getHeight() )
	{
		// resize gui environment
		setGeometry( Rect( Point( 0, 0 ), screenSize ) );
	}

    threatDeletionQueue_();

	updateHoveredElement( _d->cursorPos );

    for( ConstChildIterator it = Widget::_d->children.begin(); it != Widget::_d->children.end(); ++it)
         (*it)->beforeDraw( *_d->engine );

	if( _d->toolTip.Element )
		_d->toolTip.Element->bringToFront();

    _d->preRenderFunctionCalled = true;
}

bool GuiEnv::removeFocus( Widget* element)
{
	if( _d->focusedElement && _d->focusedElement == element )
	{
		if( _d->focusedElement->onEvent( NEvent::Gui( _d->focusedElement,  0, OC3_ELEMENT_FOCUS_LOST )) )
		{

			return false;
		}
	}
	if (_d->focusedElement)
	{
		_d->focusedElement->drop();
		_d->focusedElement = 0;
	}

	return true;
}

void GuiEnv::animate( unsigned int time )
{
    Widget::animate( time );
}

Point GuiEnv::getCursorPos() const
{
    return _d->cursorPos;
}