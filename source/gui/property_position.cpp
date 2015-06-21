#include "property_position.hpp"
#include "widget_factory.hpp"
#include "property_string.hpp"
#include "widget_helper.hpp"
#include "core/utils.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(PointAttribute)
REGISTER_CLASS_IN_WIDGETFACTORY(SizeAttribute)

PointAttribute::PointAttribute(Widget* parent, int myParentID) :
  RectAttribute( parent, myParentID)
{
}

AbstractAttribute* PointAttribute::getChild(unsigned int index)
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
  }

  return attr;
}

int PointAttribute::_getValue(unsigned int index) const
{
  Point r = _value.toPoint();
  int values[] = { r.x(), r.y() };
  return index < childCount() ? values[index] : 0;
}

std::string PointAttribute::_fullText() const
{
  Point r = _value.toPoint();
  return utils::format( 0xff, "[%d,%d]", r.x(), r.y() );
}

unsigned int PointAttribute::childCount() const { return 2; }

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
  }

  return attr;
}

int SizeAttribute::_getValue(unsigned int index) const
{
  Size r = _value.toSize();
  int values[] = { r.width(), r.height() };
  return index < childCount() ? values[index] : 0;
}

std::string SizeAttribute::_fullText() const
{
  Size r = _value.toSize();
  return utils::format( 0xff, "[%d,%d]", r.width(), r.height() );
}

unsigned int SizeAttribute::childCount() const { return 2; }

}//end namespace gui
