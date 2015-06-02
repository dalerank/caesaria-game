#include "property_attribute.hpp"
#include "core/event.hpp"
#include "core/hash.hpp"

namespace gui
{

const unsigned int AbstractAttribute::ATTRIBEDIT_ATTRIB_CHANGED = Hash( "ATTR" );

AbstractAttribute::AbstractAttribute(Widget *parent, int myParentID) :
  Widget( parent, -1, Rect(0, 0, parent->width(), parent->height()) ),
  _attribute( 0 ),
  _parentEvent( 0 ), _expanded( false ), _isNeedUpdate( false ),
  _name( "NewAttribute" ), _myParentID(myParentID)
{
#ifdef _DEBUG
    setDebugName( "ArtributeEdit");
#endif
  setGeometry( RectF( 0, 0, 1.f, 1.f) );
  setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight );
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

AbstractAttribute* AbstractAttribute::getChild(unsigned int index) { return NULL; }

void AbstractAttribute::setValue( const Variant& attribute )
{
  _attribute = attribute;
  updateAttrib(false);
}

void AbstractAttribute::setParentID(int parentID) {  _myParentID = parentID; }

void AbstractAttribute::setFont(Font font) {}

bool AbstractAttribute::updateAttrib(bool sendEvent)
{
  if(enabled() && sendEvent)
  {
    // build event and pass to parent
    NEvent event;
    event.EventType = (SysEventType)ATTRIBEDIT_ATTRIB_CHANGED;
    event.user.data1 = _myParentID;
    return parent() ? parent()->onEvent(event) : false;
  }

  return true;
}

void AbstractAttribute::beforeDraw(gfx::Engine &painter)
{
  if( size() != parent()->size() )
    setGeometry( RectF( 0, 0, 1, 1 ) );

  Widget::beforeDraw( painter );
}

void AbstractAttribute::setTitle(const std::string &name) {  _name = name;}
void AbstractAttribute::setParent4Event(Widget *parent){  _parentEvent = parent;}
const std::string &AbstractAttribute::title() const { return _name; }
unsigned int AbstractAttribute::childCount() const{ return 0; }

}//end namespace gui
