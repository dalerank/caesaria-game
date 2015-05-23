// This file is part of the "Irrlicht Engine".
// written by Reinhard Ostermeier, reinhard@nospam.r-ostermeier.de

#ifndef __NRP_TREE_VIEW_H_INCLUDED__
#define __NRP_TREE_VIEW_H_INCLUDED__

#include "widget.hpp"

namespace gui
{

class TreeViewItem;
class ScrollBar;
class TreeViewPrivate;

//! Default tree view GUI element.
class TreeView : public Widget
{
    friend class TreeViewItem;
public:
    //! constructor
    TreeView( Widget* parent,
        const Rect& rectangle, int id, bool clip = true,
			  bool drawBack = false, bool scrollBarVertical = true, 
			  bool scrollBarHorizontal = true );

    //! destructor
    virtual ~TreeView();

    //! returns the root node (not visible) from the tree.
    virtual TreeViewItem* getRoot() const
    { return _root; }

    //! returns the selected node of the tree or 0 if none is selected
    virtual TreeViewItem* getSelected() const
    { return _selected; }

    //! returns true if the tree lines are visible
    virtual bool getLinesVisible() const
    { return _linesVisible; }

    //! sets if the tree lines are visible
    virtual void setLinesVisible( bool visible )
    { _linesVisible = visible; }

    //! called if an event happened.
    virtual bool onEvent( const NEvent &event );

    //! prepare element and its children for drawing
    void beforeDraw( gfx::Engine& painter );

    //! draws the element and its children
    virtual void draw( gfx::Engine& painter );

    //! Sets if the image is left of the icon. Default is true.
    virtual void setImageLeftOfIcon( bool bLeftOf );

    //! Returns if the Image is left of the icon. Default is true.
    virtual bool getImageLeftOfIcon() const;

    //! Returns the node which is associated to the last event.
    virtual TreeViewItem* getLastEventNode() const;

	virtual void updateItems(); 

private:
  //! calculates the heigth of an node and of all visible nodes.
  void recalculateItemsRectangle();

  //! executes an mouse action (like selectNew of CGUIListBox)
  void mouseAction( int xpos, int ypos, bool onlyHover = false );
  NColor _getCurrentNodeColor( TreeViewItem* node);
  Font getCurrentNodeFont_( TreeViewItem* node);

  TreeViewItem*	_root;
  TreeViewItem*	_selected;
  TreeViewItem*	_hoverNode;
  int			_itemHeight;
  int			_indentWidth;

	ScrollBar*		ScrollBarH;
  ScrollBar*		ScrollBarV;

  TreeViewItem*	_lastEventNode;
  bool			_linesVisible;
  bool			_selecting;
  bool			_clip;
  bool			_drawBack;
  bool			_imageLeftOfIcon;

  ElementState _currentDrawState;

  ScopedPtr<TreeViewPrivate> _d;
};

}

#endif //__NRP_TREE_VIEW_H_INCLUDED__

