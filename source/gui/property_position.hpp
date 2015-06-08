#ifndef __CAESARIA_POSITION_ATTRIBUTE_H_INCLUDED__
#define __CAESARIA_POSITION_ATTRIBUTE_H_INCLUDED__

#include "property_rect.hpp"

namespace gui
{

class PointAttribute : public RectAttribute
{
public:
  PointAttribute(Widget* parent, int myParentID=-1 );

  virtual AbstractAttribute* getChild( unsigned int index );

  virtual unsigned int childCount() const;

protected:
  virtual int _getValue(unsigned int index) const;
  virtual std::string _fullText() const;
};

class SizeAttribute : public RectAttribute
{
public:
  SizeAttribute(Widget* parent, int myParentID=-1 );

  virtual AbstractAttribute* getChild( unsigned int index );

  virtual unsigned int childCount() const;
protected:
  virtual int _getValue(unsigned int index) const;
  virtual std::string _fullText() const;
};

}//end namespace gui

#endif //__CAESARIA_POSITION_ATTRIBUTE_H_INCLUDED__
