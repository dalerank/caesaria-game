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
    { return Root; }

    //! returns the selected node of the tree or 0 if none is selected
    virtual TreeViewItem* getSelected() const
    { return Selected; }

    //! returns true if the tree lines are visible
    virtual bool getLinesVisible() const
    { return LinesVisible; }

    //! sets if the tree lines are visible
    virtual void setLinesVisible( bool visible )
    { LinesVisible = visible; }

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
    NColor _GetCurrentNodeColor( TreeViewItem* node);
    Font getCurrentNodeFont_( TreeViewItem* node);

	TreeViewItem*	Root;
    TreeViewItem*	Selected;
    TreeViewItem*	_hoverNode;
    int			itemHeight_;
    int			IndentWidth;

	ScrollBar*		ScrollBarH;
    ScrollBar*		ScrollBarV;

    TreeViewItem*	LastEventNode;
    bool			LinesVisible;
    bool			Selecting;
    bool			Clip;
    bool			DrawBack;
    bool			ImageLeftOfIcon;

    ElementState _currentDrawState;

	TreeViewPrivate* _d;
};

}
#endif

