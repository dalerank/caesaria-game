#ifndef __CAESARIA_POSITION_ATTRIBUTE_H_INCLUDED__
#define __CAESARIA_POSITION_ATTRIBUTE_H_INCLUDED__

#include "property_rect.hpp"

namespace gui
{

class PositionAttribute : public RectAttribute
{
public:
  PositionAttribute(Widget* parent, int myParentID=-1 );

  virtual AbstractAttribute* getChild( unsigned int index );

  virtual unsigned int childCount() const;
};

class SizeAttribute : public RectAttribute
{
public:
  SizeAttribute(Widget* parent, int myParentID=-1 );

  virtual AbstractAttribute* getChild( unsigned int index );

  virtual unsigned int childCount() const;
};

}//end namespace gui

#endif //__CAESARIA_POSITION_ATTRIBUTE_H_INCLUDED__
