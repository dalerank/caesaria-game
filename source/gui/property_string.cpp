#include "property_string.hpp"
#include "core/event.hpp"
#include "editbox.hpp"
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(StringAttribute)

StringAttribute::StringAttribute(Widget *parent, int myParentID)
  : AbstractAttribute( parent, myParentID ), _attribEditBox( NULL )
{
  _attribEditBox = new EditBox( this, Rect( 0, 0, width(), height() ) , "", -1 );
  _attribEditBox->setGeometry( RectF( 0, 0, 1, 1 ) );
  _attribEditBox->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight );
  _attribEditBox->setSubElement( true );
  _attribEditBox->setDrawBorder( false );
  _attribEditBox->setDrawBackground( false );
}

StringAttribute::~StringAttribute()
{
}

bool StringAttribute::onEvent(const NEvent &e)
{
  if( e.EventType == sEventGui && e.gui.type == guiEditboxChanged )
    _isNeedUpdate = true;

  return AbstractAttribute::onEvent( e );
}

void StringAttribute::setValue( const Variant& value )
{
  _attribEditBox->setText( value.toString() );
  AbstractAttribute::setValue( value );
}

bool StringAttribute::updateAttrib(bool sendEvent)
{
  //_attribs->setAttribute(_index, _attribEditBox->getText());
  //_attribEditBox->setText( _attribs->getAttributeAsString( _index ) );

  return AbstractAttribute::updateAttrib(sendEvent);
}

std::string StringAttribute::editText() const { return _attribEditBox->text(); }

void StringAttribute::setFont(const Font& font)
{
  if( _attribEditBox ) _attribEditBox->setFont( font );
}

void StringAttribute::setEditText(const std::string &text)
{
  _attribEditBox->setText( text );
}

void StringAttribute::_finalizeResize() {}

}//end namespace gui
