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

  virtual unsigned int childCount() const;

  virtual bool IsExpanded() const { return _expanded; }

  virtual void SetExpanded( bool ex ) { _expanded = ex; }

  virtual AbstractAttribute* getChild( unsigned int index );

  //! sets the attribute to use
  virtual void setValue( const Variant& attrib );

  //! sets the parent ID, for identifying where events came from
  void setParentID(int parentID);

  virtual void setFont( Font font );

  //! save the attribute and possibly post the event to its parent
  virtual bool updateAttrib(bool sendEvent=true);

  virtual void setTitle( const std::string& name );

  virtual void setParent4Event( Widget* parent );

  virtual const std::string& title() const;

  virtual void beforeDraw(gfx::Engine &painter);

  static const unsigned int ATTRIBEDIT_ATTRIB_CHANGED;
protected:
  Variant	_attribute;
  Widget* _parentEvent;
  bool _expanded, _isNeedUpdate;
  std::string _name;
  int			_myParentID;
};

}

#endif //__CAESARIA_ATTRIBUTE_H_INCLUDED__
