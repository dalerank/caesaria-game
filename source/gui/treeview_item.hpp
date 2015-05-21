#ifndef _CAESARIA_TREEVIEW_ITEM_INCLUDE_H_
#define _CAESARIA_TREEVIEW_ITEM_INCLUDE_H_

#include "label.hpp"

namespace gui
{

// forward declarations
class TreeView;
class TreeViewItemPrivate;

//! Node for gui tree view
class TreeViewItem : public Label
{
	friend class TreeView;

public:
	//! constructor
	TreeViewItem( TreeViewItem* parent );
	//! destructor
  virtual ~TreeViewItem();

	//! returns the owner (tree view) of this node
	virtual TreeView* getParentTreeView() const;

	//! Returns the parent node of this node.
	virtual TreeViewItem* getParentItem() const;

	//! returns the icon text of the node
  virtual const std::string& getIcon() const
	{ return Icon; }

	//! sets the icon text of the node
  virtual void setIcon( const char* icon );

	//! returns the image index of the node
  virtual unsigned int getImageIndex() const
	{ return ImageIndex; }

	//! sets the image index of the node
  virtual void setImageIndex( unsigned int imageIndex )
	{ ImageIndex = imageIndex; }

	//! returns the image index of the node
  virtual unsigned int getSelectedImageIndex() const
	{ return SelectedImageIndex; }

	//! sets the image index of the node
  virtual void setSelectedImageIndex( unsigned int imageIndex )
	{ SelectedImageIndex = imageIndex; }

	//! returns the user data (void*) of this node
	virtual void* getData() const
	{ return Data; }

    //! sets the user data (void*) of this node
	virtual void setData( void* data )
	{ Data = data; }

	//! returns the child item count
  virtual unsigned int getChildCount() const;

	//! removes all children (recursive) from this node
	virtual void clearChildren();

	//! returns true if this node has child nodes
	virtual bool hasChildren() const;

	//! Adds a new node behind the last child node.
	//! \param text text of the new node
	//! \param icon icon text of the new node
	//! \param imageIndex index of the image for the new node (-1 = none)
	//! \param selectedImageIndex index of the selected image for the new node (-1 = same as imageIndex)
	//! \param data user data (void*) of the new node
	//! \param data2 user data2 (IReferenceCounted*) of the new node
	//! \return
	//! returns the new node
	virtual TreeViewItem* addChildBack(
    const std::string& text,
    const char* icon = 0,
    int imageIndex = -1,
    int selectedImageIndex = -1,
    void* data = 0);

	//! Adds a new node before the first child node.
	//! \param text text of the new node
	//! \param icon icon text of the new node
	//! \param imageIndex index of the image for the new node (-1 = none)
	//! \param selectedImageIndex index of the selected image for the new node (-1 = same as imageIndex)
	//! \param data user data (void*) of the new node
	//! \param data2 user data2 (IReferenceCounted*) of the new node
	//! \return
	//! returns the new node
	virtual TreeViewItem* addChildFront(
    const std::string&		text,
    const char*		icon = 0,
    int					imageIndex = -1,
    int					selectedImageIndex = -1,
    void*					data = 0 );

	//! Adds a new node behind the other node.
	//! The other node has also te be a child node from this node.
	//! \param text text of the new node
	//! \param icon icon text of the new node
	//! \param imageIndex index of the image for the new node (-1 = none)
	//! \param selectedImageIndex index of the selected image for the new node (-1 = same as imageIndex)
	//! \param data user data (void*) of the new node
	//! \param data2 user data2 (IReferenceCounted*) of the new node
	//! \return
	//! returns the new node or 0 if other is no child node from this
	virtual TreeViewItem* insertChildAfter(
		TreeViewItem*	other,
    const char*		text,
    const char*		icon = 0,
    int					imageIndex = -1,
    int					selectedImageIndex = -1,
    void*					data = 0);

	//! Adds a new node before the other node.
	//! The other node has also te be a child node from this node.
	//! \param text text of the new node
	//! \param icon icon text of the new node
	//! \param imageIndex index of the image for the new node (-1 = none)
	//! \param selectedImageIndex index of the selected image for the new node (-1 = same as imageIndex)
	//! \param data user data (void*) of the new node
	//! \param data2 user data2 (IReferenceCounted*) of the new node
	//! \return
	//! returns the new node or 0 if other is no child node from this
	virtual TreeViewItem* insertChildBefore(
		TreeViewItem*	other,
    const char*		text,
    const char*		icon = 0,
    int					imageIndex = -1,
    int					selectedImageIndex = -1,
    void*					data = 0 );

	//! Return the first child note from this node.
	virtual TreeViewItem* getFirstChild() const;

	//! 
  virtual void draw( gfx::Engine& painter );

	//! Return the last child note from this node.
	virtual TreeViewItem* getLastChild() const;

	//! Returns the preverse sibling node from this node.
	virtual TreeViewItem* getPrevSibling() const;

	//! Returns the next sibling node from this node.
	virtual TreeViewItem* getNextSibling() const;

	//! Returns the next visible (expanded, may be out of scrolling) node from this node.
	virtual TreeViewItem* getNextVisible() const;

	//! Deletes a child node.
	virtual bool deleteChild( TreeViewItem* child );

	//! Moves a child node one position up.
	virtual bool moveChildUp( TreeViewItem* child );

	//! Moves a child node one position down.
	virtual bool moveChildDown( TreeViewItem* child );

	//! Returns true if the node is expanded (children are visible).
    virtual bool isExpanded() const;

	//! Sets if the node is expanded.
	virtual void setExpanded( bool expanded );

	//! Returns true if the node is currently selected.
	virtual bool getSelected() const;

	//! Sets this node as selected.
	virtual void setSelected( bool selected );

	//! Returns true if this node is the root node.
	virtual bool isRoot() const;

	//! Returns the level of this node.
  virtual int getLevel() const;

	//! Returns true if this node is visible (all parents are expanded).
	virtual bool isVisible() const;

  virtual void setColor( const NColor& color );

  virtual NColor getColor() const { return _fontColor; }

private:

	TreeViewItem( TreeView* parent );
	void init_();

  TreeView*	owner_;
  TreeViewItem*	itemParent_;
  NColor           _fontColor;
  std::string	Icon;
  int				ImageIndex;
  int				SelectedImageIndex;
  void*				Data;
  bool				isExpanded_;
	
	TreeViewItemPrivate* _d;
};

}

#endif //_CAESARIA_TREEVIEW_ITEM_INCLUDE_H_
