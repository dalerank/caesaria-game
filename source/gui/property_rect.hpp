#ifndef __CAESARIA_RECT_ATTRIBUTE_H_INCLUDED__
#define __CAESARIA_RECT_ATTRIBUTE_H_INCLUDED__

#include "property_attribute.hpp"
#include "label.hpp"

namespace gui
{

class StringAttribute;

class RectAttribute : public AbstractAttribute
{
protected:
  typedef std::map< int, StringAttribute* > Editors;
  typedef Editors::iterator EditIterator;

public:
  RectAttribute( Widget* parent, int myParentID=-1 );

  virtual ~RectAttribute();

  virtual unsigned int childCount() const;

  virtual AbstractAttribute* getChild( unsigned int index );

  virtual void setValue( const Variant& value);

  virtual bool onEvent(const NEvent &e);

  //! save the attribute and possibly post the event to its parent
  virtual bool updateAttrib(bool sendEvent=true);

protected:
  virtual int _getValue(unsigned int index) const;
  virtual std::string _fullText() const;

  Variant _value;
	Label* _label;
};

}
#endif //__CAESARIA_RECT_ATTRIBUTE_H_INCLUDED__
