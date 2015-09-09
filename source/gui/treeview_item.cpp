#include "treeview_item.hpp"
#include "treeview.hpp"

namespace gui
{

typedef std::list< TreeViewItem* > NodeList;
typedef NodeList::iterator TvIterator;

class TreeViewItemPrivate
{
public:
	NodeList Children;
};

TreeViewItem::TreeViewItem( TreeViewItem* parent )
  : Label( parent, Rect( 0, 0, 1, 1 ) )
{
	init_();
    itemParent_ = parent;
    owner_ = parent->getParentTreeView();
#ifdef DEBUG
  setDebugName( "NrpTreeViewItem" );
#endif
}

TreeViewItem::TreeViewItem( TreeView* parent )
  : Label( parent, Rect( 0, 0, 1, 1 ) )
{
	init_();
  owner_ = parent;

#ifdef DEBUG
  setDebugName( "NrpTreeViewItem" );
#endif
}

void TreeViewItem::init_()
{
  setNotClipped( false );
  itemParent_ = 0;
	ImageIndex = -1;
	SelectedImageIndex = -1;
	Data = 0;
  isExpanded_ = false;
	_fontColor = 0;
  _d = new TreeViewItemPrivate;

	sendToBack();
}

TreeViewItem::~TreeViewItem()
{
  if( owner_ && this == owner_->getSelected() )
	{
		setSelected( false );
	}

	clearChildren();

	delete _d;
}

unsigned int TreeViewItem::getChildCount() const { return _d->Children.size(); }

bool TreeViewItem::hasChildren() const { return !_d->Children.empty(); }

TreeView* TreeViewItem::getParentTreeView() const { return owner_; }

void TreeViewItem::setIcon( const char* icon ) { Icon = icon; }

void TreeViewItem::clearChildren()
{
	TvIterator	it;

	for( it = _d->Children.begin(); it != _d->Children.end(); it++ )
		( *it )->remove();

	_d->Children.clear();
}

TreeViewItem* TreeViewItem::addChildBack(
  const std::string&		text,
  const char*		icon /*= 0*/,
  int					imageIndex /*= -1*/,
  int					selectedImageIndex /*= -1*/,
  void*					data )
{
	TreeViewItem*	newChild = new TreeViewItem( this );

	_d->Children.push_back( newChild );
	newChild->setText( text );
  newChild->Icon = icon ? icon : "";
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
  if( owner_ )
    owner_->updateItems();
	
	return newChild;
}

TreeViewItem* TreeViewItem::addChildFront(
  const std::string&		text,
  const char*		icon /*= 0*/,
  int					imageIndex /*= -1*/,
  int					selectedImageIndex /*= -1*/,
  void*					data  )
{
	TreeViewItem*	newChild = new TreeViewItem( this );

	_d->Children.push_front( newChild );
	newChild->setText( text );
	newChild->Icon = icon;
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
  if( owner_ )
    owner_->updateItems();

	return newChild;
}

TreeViewItem* TreeViewItem::insertChildAfter(
	TreeViewItem*	other,
  const char*		text,
  const char*		icon /*= 0*/,
  int					imageIndex /*= -1*/,
  int					selectedImageIndex /*= -1*/,
  void*					data  )
{
	TreeViewItem*									newChild = 0;

  foreach( itOther, _d->Children )
	{
		if( other == *itOther )
		{
			newChild = new TreeViewItem( this );
			newChild->setText( text );
			newChild->Icon = icon;
			newChild->ImageIndex = imageIndex;
			newChild->SelectedImageIndex = selectedImageIndex;
			newChild->Data = data;
      ++itOther;
      _d->Children.insert( itOther, newChild );
			break;
		}
	}
  if( owner_ )
    owner_->updateItems();

	return newChild;
}

TreeViewItem* TreeViewItem::insertChildBefore(
	TreeViewItem*	other,
  const char*		text,
  const char*		icon /*= 0*/,
  int					imageIndex /*= -1*/,
  int					selectedImageIndex /*= -1*/,
  void*					data )
{
	TvIterator	itOther;
	TreeViewItem*									newChild = 0;

	for( itOther = _d->Children.begin(); itOther != _d->Children.end(); itOther++ )
	{
		if( other == *itOther )
		{
			newChild = new TreeViewItem( this );
			newChild->setText( text );
			newChild->Icon = icon;
			newChild->ImageIndex = imageIndex;
			newChild->SelectedImageIndex = selectedImageIndex;

      _d->Children.insert( itOther, newChild );
			break;
		}
	}
  if( owner_ )
    owner_->updateItems();

	return newChild;
}

TreeViewItem* TreeViewItem::getFirstChild() const
{
	if( _d->Children.empty() )
	{
		return 0;
	}
	else
	{
		return *( _d->Children.begin() );
	}
}

TreeViewItem* TreeViewItem::getLastChild() const
{
	if( _d->Children.empty() )
	{
		return 0;
	}
	else
	{
    return *( _d->Children.rbegin() );
	}
}

TreeViewItem* TreeViewItem::getPrevSibling() const
{
	TvIterator	itThis;
	TvIterator	itOther;
	TreeViewItem*									other = 0;

    if( itemParent_ )
	{
        for( itThis = itemParent_->_d->Children.begin(); itThis != itemParent_->_d->Children.end(); itThis++ )
		{
			if( this == *itThis )
			{
                if( itThis != itemParent_->_d->Children.begin() )
				{
					other = *itOther;
				}
				break;
			}
			itOther = itThis;
		}
	}
	return other;
}

TreeViewItem* TreeViewItem::getNextSibling() const
{
	TvIterator	itThis;
	TreeViewItem*									other = 0;

  if( itemParent_ )
	{
    for( itThis = itemParent_->_d->Children.begin(); itThis != itemParent_->_d->Children.end(); itThis++ )
		{
			if( this == *itThis )
			{
        if( *itThis != *(itemParent_->_d->Children.rbegin()) )
				{
					other = *( ++itThis );
				}
				break;
			}
		}
	}
	return other;
}

TreeViewItem* TreeViewItem::getNextVisible() const
{
	TreeViewItem*	next = 0;
	TreeViewItem*	node = 0;

	node = const_cast< TreeViewItem* >( this );

    if( node->isExpanded() && node->hasChildren() )
	{
		next = node->getFirstChild();
	}
	else
	{
		next = node->getNextSibling();
	}
	while( !next && node->getParentItem() )
	{
		next = node->getParentItem()->getNextSibling();
		if( !next )
		{
			node = node->getParentItem();
		}
	}

	return next;
}

bool TreeViewItem::deleteChild( TreeViewItem* child )
{
	TvIterator	itChild;
	bool	deleted = false;

	for( itChild = _d->Children.begin(); itChild != _d->Children.end(); itChild++ )
	{
		if( child == *itChild )
		{
			child->drop();
			_d->Children.erase( itChild );
			deleted = true;
			break;
		}
	}
	return deleted;
}

bool TreeViewItem::moveChildUp( TreeViewItem* child )
{
	TvIterator	itChild;
	TvIterator	itOther;
	TreeViewItem*									nodeTmp;
	bool													moved = false;

	for( itChild = _d->Children.begin(); itChild != _d->Children.end(); itChild++ )
	{
		if( child == *itChild )
		{
			if( itChild != _d->Children.begin() )
			{
				nodeTmp = *itChild;
				*itChild = *itOther;
				*itOther = nodeTmp;
				moved = true;
			}
			break;
		}
		itOther = itChild;
	}
	return moved;
}

bool TreeViewItem::moveChildDown( TreeViewItem* child )
{
	TvIterator	itChild;
	TvIterator	itOther;
	TreeViewItem*									nodeTmp;
	bool													moved = false;

	for( itChild = _d->Children.begin(); itChild != _d->Children.end(); itChild++ )
	{
		if( child == *itChild )
		{
      if( itChild != _d->Children.end() )
			{
				itOther = itChild;
				++itOther;
				nodeTmp = *itChild;
				*itChild = *itOther;
				*itOther = nodeTmp;
				moved = true;
			}
			break;
		}
	}
	return moved;
}

void TreeViewItem::setExpanded( bool expanded )
{
  isExpanded_ = expanded;
	if( owner_ )
		owner_->updateItems();
}

void TreeViewItem::setSelected( bool selected )
{
    if( owner_ )
	{
		if( selected )
		{
            owner_->_selected = this;
		}
		else
		{
            if( owner_->_selected == this )
			{
                owner_->_selected = 0;
			}
		}
	}
}

bool TreeViewItem::getSelected() const
{
    if( owner_ )
	{
        return owner_->_selected == this;
	}
	else
	{
		return false;
	}
}

bool TreeViewItem::isRoot() const
{
    return ( owner_ && ( this == owner_->_root ) );
}

int TreeViewItem::level() const
{
    if( itemParent_ )
	{
        return itemParent_->level() + 1;
	}
	else
	{
		return 0;
	}
}

TreeViewItem* TreeViewItem::getParentItem() const
{
    return itemParent_;
}

bool TreeViewItem::isVisible() const
{
    if( itemParent_ )
	{
        return itemParent_->isExpanded() && itemParent_->isVisible();
	}
	else
	{
		return true;
	}
}

void TreeViewItem::setColor(const NColor &color )
{
	_fontColor = color;
}

bool TreeViewItem::isExpanded() const
{
    return isExpanded_;
}

void TreeViewItem::draw( gfx::Engine& painter)
{
	if( !isVisible() )
		return;

  //drawText_( absoluteRect(), &owner_->absoluteClippingRectRef(), painter );

  Label::draw( painter );
}

}
