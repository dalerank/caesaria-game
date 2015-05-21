#ifndef __CACESARIA_PROPERTY_WORKSPACE_H_INCLUDED__
#define __CACESARIA_PROPERTY_WORKSPACE_H_INCLUDED__

#include "window.hpp"
#include "property_browser.hpp"
#include "stretch_layout.hpp"
#include "treeview.hpp"
#include "objects/overlay.hpp"

namespace gui
{

class PropertyWorkspace : public Window
{
public:

	//! constructor
  PropertyWorkspace( Widget* parent, const Rect& rectangle );

	//! destructor
  ~PropertyWorkspace();

	//! this part draws the window
  virtual void draw( gfx::Engine& painter );
	//! handles events
  virtual bool onEvent(const NEvent &event);

	//! change selection
  virtual void setSelectedElement(Overlay *sel);

	//! get draggable
	virtual bool isDraggable() const;

	//! get draggable
	virtual void setDraggable(bool draggable);

  TreeView* getTreeView() const;
  PropertyBrowser* getAttributeEditor() const;

  void updateTree( Widget* elm );
private:

  void addChildrenToTree_(Widget* parentElement, TreeViewItem* treenode);
  TreeViewItem* _GetTreeNode(Overlay *element, TreeViewItem* searchnode);
  void createElementsTreeView_();
  void createTabControl_();


  // for dragging the window
	bool                    Dragging;
	bool                    IsDraggable;
	bool                    Resizing;
  Point                   DragStart;

  Overlay*             _selectedElement; // current selected element
  Layout*             _windowLayout;

  PropertyBrowser*    _attribEditor;	// edits the current attribute
  TreeView*          _treeView;       // tree view of all elements in scene
};

}

#endif // __CACESARIA_PROPERTY_WORKSPACE_H_INCLUDED__

