#include "property_rect.hpp"
#include "property_string.hpp"
#include "core/event.hpp"
#include "widget_factory.hpp"
#include "core/utils.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(AttributeRect)

AttributeRect::AttributeRect(Widget *parent, int myParentID)
  : AbstractAttribute( parent, myParentID )
{
  label_ = new Label( this, Rect( 0, 0, width(), height() ) );
  label_->setSubElement(true);
  label_->setTextAlignment( align::upperLeft, align::center );
  label_->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight );
}

AttributeRect::~AttributeRect() {}

unsigned int AttributeRect::childCount() const { return 4; }

AbstractAttribute *AttributeRect::getChild(unsigned int index)
{
  if( index >= childCount() )
    return 0;

  std::string text[] = { "\t\t\tLeft", "\t\t\tTop", "\t\t\tRight", "\t\t\tBottom" };

  AttributeString* attr = safety_cast<AttributeString*>( findChild( text[ index ] ));
  if( !attr )
    {
      attr = new AttributeString( this, -1 );
      attr->setTitle( text[ index ] );
      attr->setParent4Event( this );
      attr->setEditText( tokens_[ index ] );
    }

  return attr;
}

void AttributeRect::setValue(const Variant &value)
{
  std::string splitText = value.toString();
  label_->setText( splitText );

  tokens_ = utils::split( tokens_, L", ", 2 );

  for( unsigned int i=0; i < tokens_.size(), i < childCount(); i++)
    {
      AttributeString* ed = safety_cast< AttributeString* >( getChild( i ) );

      if( ed )
        ed->setEditText( tokens_[ i ] );
    }

  AbstractAttribute::setAttrib(attribs, attribIndex);
}

bool AttributeRect::onEvent(const NEvent &e)
{
  if( e.EventType == sEventGui && e.GuiEvent.EventType == guiEditboxChanged )
    _isNeedUpdate = true;

  return AbstractAttribute::onEvent( e );
}

bool AttributeRect::updateAttrib(bool sendEvent)
{
  if (!_attribs)
    return true;

  std::string retText;
  for( unsigned int i=0; i < childCount(); i++)
    {
      if( AttributeString* editor = safety_cast< AttributeString* >( getChild( i ) ) )
        {
          retText.append( editor->GetEditText() );
          retText.append( ( i != ( GetChildAttributesCount() - 1) ) ? L", " : L"" );
        }
    }

  label_->setText( retText  );
  _attribs->setAttribute( _index, label_->getText() );

  return AbstractAttribute::updateAttrib( sendEvent );
}


}//end namespace gui
