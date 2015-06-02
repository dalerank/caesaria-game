#include "property_position.hpp"
#include "widget_factory.hpp"
#include "property_string.hpp"
#include "widget_helper.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(PositionAttribute)
REGISTER_CLASS_IN_WIDGETFACTORY(SizeAttribute)

PositionAttribute::PositionAttribute(Widget* parent, int myParentID) :
  RectAttribute( parent, myParentID)
{
}

AbstractAttribute* PositionAttribute::getChild(unsigned int index)
{
  if( index >= childCount() )
    return 0;

  std::string text[] = { "\t\t\tx", "\t\t\ty" };

  StringAttribute* attr = findChildA< StringAttribute* >( text[ index ], false, this );
  if( !attr )
  {
    attr = new StringAttribute( this, -1 );
    attr->setTitle( text[ index ] );
    attr->setParent4Event( this );
    attr->setEditText( tokens_[ index ] );
  }

  return attr;
}

unsigned int PositionAttribute::childCount() const { return 2; }

SizeAttribute::SizeAttribute(Widget* parent, int myParentID) :
  RectAttribute( parent, myParentID)
{
}

AbstractAttribute* SizeAttribute::getChild(unsigned int index)
{
  if( index >= childCount() )
    return 0;

  std::string text[] = { "\t\t\twidth", "\t\t\theight" };

  StringAttribute* attr = findChildA< StringAttribute* >( text[ index ], false, this );
  if( !attr )
  {
    attr = new StringAttribute( this, -1 );
    attr->setTitle( text[ index ] );
    attr->setParent4Event( this );
    attr->setEditText( tokens_[ index ] );
  }

  return attr;
}

unsigned int SizeAttribute::childCount() const { return 2; }

}//end namespace gui
