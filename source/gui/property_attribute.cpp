#include "property_attribute.hpp"
#include "core/event.hpp"
#include "core/hash.hpp"

namespace gui
{

const unsigned int AbstractAttribute::ATTRIBEDIT_ATTRIB_CHANGED = Hash( "ATTR" );

AbstractAttribute::AbstractAttribute(Widget *parent, int myParentID) :
  Widget( parent, -1, Rect(0, 0, 1, 1) ),
  _attribs( 0 ),
  _parentEvent( 0 ), _expanded( false ), _isNeedUpdate( false ),
  _name( "NewAttribute" ), _myParentID(myParentID)
{
#ifdef _DEBUG
    setDebugName( "ArtributeEdit");
#endif
}

AbstractAttribute::~AbstractAttribute()
{
}

bool AbstractAttribute::onEvent(const NEvent &e)
{
  if( enabled() )
    {
      if( _parentEvent )
        return _parentEvent->onEvent( e );

      switch( e.EventType )
        {
        case sEventGui:
          switch (e.gui.type)
            {
            case guiElementFocused:
              if( isMyChild(e.gui.caller) )
                bringToFront();
              break;

            case guiElementFocusLost:
              if( _isNeedUpdate )
                {
                  updateAttrib();
                  _isNeedUpdate = false;
                }

              return Widget::onEvent(e);
            default:
              break;
              //return updateAttrib();
            }
          break;

        case sEventKeyboard:
          if( e.keyboard.key == KEY_RETURN )
            updateAttrib();

          return true;

        default:
          break;
        }
    }

  return Widget::onEvent(e);
}

AbstractAttribute *AbstractAttribute::GetChildAttribute(unsigned int index) { return NULL; }

void AbstractAttribute::setAttrib( const Variant& attribs, const std::string& name)
{
  _attribs = attribs;

  std::string rname = name + " (" + attribs.typeName() +  ")";
  setText( rname );

  // get minimum height
  int y=0;
  ConstChildIterator it = children().begin();
  for (; it != children().end(); ++it)
  {
    if( y < (*it)->relativeRect().bottom() )
      y = (*it)->relativeRect().bottom();
  }

  updateAttrib(false);
}

void AbstractAttribute::setParentID(int parentID)
{
  _myParentID = parentID;
}

void AbstractAttribute::setFont(Font font) {}

bool AbstractAttribute::updateAttrib(bool sendEvent)
{
  if (_attribs && enabled() && sendEvent)
  {
    // build event and pass to parent
    NEvent event;
    event.EventType = (SysEventType)ATTRIBEDIT_ATTRIB_CHANGED;
    event.user.data1 = _myParentID;
    return parent() ? parent()->onEvent(event) : false;
  }

  return true;
}

void AbstractAttribute::setAttributeName(const std::string &name)
{
  _name = name;
}

void AbstractAttribute::setParent4Event(Widget *parent)
{
  _parentEvent = parent;
}

unsigned int AbstractAttribute::GetChildAttributesCount() const
{
  return 0;
}

}//end namespace gui
