#ifndef __CAESARIA_RECT_ATTRIBUTE_H_INCLUDED__
#define __CAESARIA_RECT_ATTRIBUTE_H_INCLUDED__

#include "property_attribute.hpp"
#include "label.hpp"

namespace gui
{

class AttributeString;

class AttributeRect : public AbstractAttribute
{
protected:
  typedef std::map< int, AttributeString* > Editors;
  typedef Editors::iterator EditIterator;

public:
  //
  AttributeRect( Widget* parent, int myParentID );

  virtual ~AttributeRect();

  virtual unsigned int childCount() const;

  virtual AbstractAttribute* getChild( unsigned int index );

  virtual void setValue( const Variant& value);

  virtual bool onEvent(const NEvent &e);

  //! save the attribute and possibly post the event to its parent
  virtual bool updateAttrib(bool sendEvent=true);

protected:
  StringArray tokens_;
	Label* label_;
};

}
#endif //__CAESARIA_RECT_ATTRIBUTE_H_INCLUDED__
