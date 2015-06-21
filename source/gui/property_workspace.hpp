#ifndef __CACESARIA_PROPERTY_WORKSPACE_H_INCLUDED__
#define __CACESARIA_PROPERTY_WORKSPACE_H_INCLUDED__

#include "window.hpp"
#include "property_browser.hpp"
#include "stretch_layout.hpp"
#include "treeview.hpp"
#include "objects/overlay.hpp"
#include "scene/base.hpp"

namespace gui
{

class WorkspaceEventHandler;
class Label;

class PropertyWorkspace : public Window
{
public:

	//! constructor
  PropertyWorkspace( Widget* parent, scene::Base* scene, const Rect& rectangle );

	//! destructor
  ~PropertyWorkspace();

	//! this part draws the window
  virtual void draw( gfx::Engine& painter );
	//! handles events
  virtual bool onEvent(const NEvent &event);

	//! change selection
  virtual void setSelectedElement(OverlayPtr sel);

  scene::EventHandlerPtr handler() const;

  void setCity( PlayerCityPtr city );

  void updateTree( Widget* elm );
private:

  void _addChildrenToTree(Widget* parentElement, TreeViewItem* treenode);
  TreeViewItem* _getTreeNode(OverlayPtr element, TreeViewItem* searchnode);
  void _createElementsTreeView();
  void _createTabControl();
  void _update();

  // for dragging the window
  bool                    _resizing;

  OverlayPtr              _selectedElement; // current selected element
  PlayerCityPtr           _city;
  unsigned int            _lastUpdateTime;
  WorkspaceEventHandler*  _eventHandler;
  Label* _editor;
  //PropertyBrowser*        _attribEditor;	// edits the current attribute
  //TreeView*               _treeView;       // tree view of all elements in scene
};

}

#endif // __CACESARIA_PROPERTY_WORKSPACE_H_INCLUDED__

