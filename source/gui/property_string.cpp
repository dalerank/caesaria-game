#include "property_string.hpp"
#include "core/event.hpp"
#include "editbox.hpp"
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(AttributeString)

AttributeString::AttributeString(Widget *parent, int myParentID) :
  AbstractAttribute( parent, myParentID ), _attribEditBox( NULL )
{
  _attribEditBox = new EditBox( this, Rect( 0, 0, 1, 1 ), "", -1 );
  _attribEditBox->setGeometry( RectF( 0, 0, 1, 1 ) );
  _attribEditBox->setSubElement( true );
  _attribEditBox->setDrawBorder( false );
  _attribEditBox->setDrawBackground( false );
  //_attribEditBox->setAlignment(alignUpperLeft, alignLowerRight, alignUpperLeft, alignLowerRight);
}

bool AttributeString::onEvent(const NEvent &e)
{
  if( e.EventType == sEventGui && e.gui.type == guiEditboxChanged )
    _isNeedUpdate = true;

  return AbstractAttribute::onEvent( e );
}

void AttributeString::setAttrib( const Variant& m, const std::string& name )
{
  _attribEditBox->setText( m.toString() );

  AbstractAttribute::setAttrib( m, name);
}

bool AttributeString::updateAttrib(bool sendEvent)
{
  if (!_attribs)
    return true;

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

}//end namespace gui
