#include "property_string.hpp"
#include "core/event.hpp"
#include "editbox.hpp"
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(AttributeString)

AttributeString::AttributeString(Widget *parent, int myParentID)
  : AbstractAttribute( parent, myParentID ), _attribEditBox( NULL )
{
  _attribEditBox = new EditBox( this, Rect( 0, 0, width(), height() ) , "", -1 );
  _attribEditBox->setGeometry( RectF( 0, 0, 1, 1 ) );
  _attribEditBox->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight );
  _attribEditBox->setSubElement( true );
  _attribEditBox->setDrawBorder( false );
  _attribEditBox->setDrawBackground( false );
}

gui::AttributeString::~AttributeString()
{
}

bool AttributeString::onEvent(const NEvent &e)
{
  if( e.EventType == sEventGui && e.gui.type == guiEditboxChanged )
    _isNeedUpdate = true;

  return AbstractAttribute::onEvent( e );
}

void AttributeString::setValue( const Variant& value )
{
  _attribEditBox->setText( value.toString() );
  AbstractAttribute::setValue( value );
}

bool AttributeString::updateAttrib(bool sendEvent)
{
  //_attribs->setAttribute(_index, _attribEditBox->getText());
  //_attribEditBox->setText( _attribs->getAttributeAsString( _index ) );

  return AbstractAttribute::updateAttrib(sendEvent);
}

std::string AttributeString::GetEditText() const { return _attribEditBox->text(); }

void AttributeString::setFont(Font font)
{
  if( _attribEditBox ) _attribEditBox->setFont( font );
}

void AttributeString::setEditText(const std::string &text)
{
  _attribEditBox->setText( text );
}

void AttributeString::_finalizeResize()
{

}

}//end namespace gui
