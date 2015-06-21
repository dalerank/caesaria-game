#include "property_rect.hpp"
#include "property_string.hpp"
#include "core/event.hpp"
#include "widget_factory.hpp"
#include "core/utils.hpp"
#include "widget_helper.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(RectAttribute)

RectAttribute::RectAttribute(Widget *parent, int myParentID)
  : AbstractAttribute( parent, myParentID )
{
  _label = new Label( this, Rect( 0, 0, width(), height() ) );
  _label->setSubElement(true);
  _label->setTextAlignment( align::upperLeft, align::center );
  _label->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight );
}

RectAttribute::~RectAttribute() {}

unsigned int RectAttribute::childCount() const { return 4; }

AbstractAttribute* RectAttribute::getChild(unsigned int index)
{
  if( index >= childCount() )
    return 0;

  std::string text[] = { "\t\t\tleft", "\t\t\ttop", "\t\t\tright", "\t\t\tbottom" };

  StringAttribute* attr = findChildA<StringAttribute*>( text[ index ], false, this );
  if( !attr )
  {
    attr = new StringAttribute( this, -1 );
    attr->setTitle( text[ index ] );
    attr->setParent4Event( this );
    attr->setEditText( utils::i2str( _getValue( index ) ) );
  }

  return attr;
}

void RectAttribute::setValue(const Variant &value)
{
  _value = value;

  _label->setText( _fullText() );

  for( unsigned int i=0; i < childCount(); i++)
  {
    StringAttribute* ed = safety_cast< StringAttribute* >( getChild( i ) );

    if( ed )
      ed->setEditText( utils::i2str( _getValue( i ) ) );
  }

  AbstractAttribute::setValue( value );
}

bool RectAttribute::onEvent(const NEvent &e)
{
  if( e.EventType == sEventGui && e.gui.type == guiEditboxChanged )
    _isNeedUpdate = true;

  return AbstractAttribute::onEvent( e );
}

bool RectAttribute::updateAttrib(bool sendEvent)
{
  if (!_attribute)
    return true;

  std::string retText;
  for( unsigned int i=0; i < childCount(); i++)
  {
    if( StringAttribute* editor = safety_cast< StringAttribute* >( getChild( i ) ) )
    {
      retText.append( editor->editText() );
      retText.append( ( i != ( childCount() - 1) ) ? ", " : "" );
    }
  }

  _label->setText( retText  );
  //_attribs->setAttribute( _index, _label->text() );

  return AbstractAttribute::updateAttrib( sendEvent );
}

int RectAttribute::_getValue(unsigned int index) const
{
  Rect r = _value.toRect();
  int values[] = { r.left(), r.top(), r.right(), r.bottom() };
  return index < childCount() ? values[index] : 0;
}

std::string RectAttribute::_fullText() const
{
  Rect r = _value.toRect();
  return utils::format( 0xff, "[%d,%d,%d,%d]", r.left(), r.top(), r.right(), r.bottom() );
}


}//end namespace gui
