#ifndef __CAESARIA_STRING_ATTRIBUTE_H_INCLUDED__
#define __CAESARIA_STRING_ATTRIBUTE_H_INCLUDED__

#include "property_attribute.hpp"

namespace gui
{

class EditBox;

class AttributeString : public AbstractAttribute
{
public:
  //
  AttributeString( Widget *parent, int myParentID=-1);

  virtual ~AttributeString();

  virtual bool onEvent(const NEvent &e);

  virtual void setValue(const Variant &value);

  //! save the attribute and possibly post the event to its parent
  virtual bool updateAttrib(bool sendEvent=true);

  virtual std::string GetEditText() const;

  virtual void setFont( Font font );

  virtual void setEditText(const std::string& text);

protected:
  virtual void _finalizeResize();

  EditBox* _attribEditBox;
};

}//end namespace gui

#endif //__CAESARIA_STRING_ATTRIBUTE_H_INCLUDED__
