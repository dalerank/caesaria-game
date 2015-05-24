#ifndef __CAESARIA_ATTRIBUTE_H_INCLUDED__
#define __CAESARIA_ATTRIBUTE_H_INCLUDED__

#include "widget.hpp"
#include "core/variant_map.hpp"

namespace gui
{

class AbstractAttribute : public Widget
{
public:
  //! constructor
  AbstractAttribute( Widget* parent, int myParentID);

  virtual ~AbstractAttribute();

  virtual bool onEvent(const NEvent &e);

  virtual unsigned int GetChildAttributesCount() const;

  virtual bool IsExpanded() const { return _expanded; }

  virtual void SetExpanded( bool ex ) { _expanded = ex; }

  virtual AbstractAttribute* GetChildAttribute( unsigned int index );

  //! sets the attribute to use
  virtual void setAttrib(const Variant &attribs, const std::string& name);

  //! sets the parent ID, for identifying where events came from
  void setParentID(int parentID);

  virtual void setFont( Font font );

  //! save the attribute and possibly post the event to its parent
  virtual bool updateAttrib(bool sendEvent=true);

  virtual void setAttributeName( const std::string& name );

  virtual void setParent4Event( Widget* parent );

  virtual const std::string& GetAttributeName() const { return _name; }

  static const unsigned int ATTRIBEDIT_ATTRIB_CHANGED;
protected:
  Variant	_attribs;
  Widget* _parentEvent;
  bool _expanded, _isNeedUpdate;
  std::string _name;
  int			_myParentID;
};

}

#endif //__CAESARIA_ATTRIBUTE_H_INCLUDED__
