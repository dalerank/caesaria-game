#include "treeview.hpp"
#include "treeview_item.hpp"
#include "scrollbar.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"

#define DEFAULT_SCROLLBAR_SIZE 15

namespace gui
{

class TreeViewPrivate
{
public:
  Size totalItemSize;
	bool needUpdateItems;
	Font font;
};

//! constructor
TreeView::TreeView( Widget* parent,
  const Rect& rectangle, int id, bool clip,
	bool drawBack,bool scrollBarVertical, bool scrollBarHorizontal)
  : Widget( parent, id, rectangle ),
	Root(0), Selected(0),
	itemHeight_( 0 ),
	IndentWidth( 0 ),
	ScrollBarH( 0 ),
	ScrollBarV( 0 ),
	LastEventNode( 0 ),
	LinesVisible( true ),
	Selecting( false ),
	Clip( clip ),
	DrawBack( drawBack ),
	ImageLeftOfIcon( true ),
    _currentDrawState( stNormal ),
	_d( new TreeViewPrivate )
{
#ifdef _DEBUG
  setDebugName( "TreeView" );
#endif

	//IGUISkin* skin = Environment->getSkin();
  int s = DEFAULT_SCROLLBAR_SIZE;//skin->getSize( EGDS_SCROLLBAR_SIZE );

	if ( scrollBarVertical )
	{
        Rect r( width() - s,  0, width(),
                height() - (scrollBarHorizontal ? s : 0 ) );
		ScrollBarV = new ScrollBar( this, r, ScrollBar::Vertical );

		ScrollBarV->setSubElement(true);
    ScrollBarV->setValue( 0 );
	}

	if ( scrollBarHorizontal )
	{
    Rect r( 0, height() - s, width() - s, height() );
		ScrollBarH = new ScrollBar( this, r, ScrollBar::Horizontal );

		ScrollBarH->setSubElement(true);
    ScrollBarH->setValue( 0 );
		ScrollBarH->grab();
	}

	Root = new TreeViewItem( this );
    Root->isExpanded_ = true;

	recalculateItemsRectangle();
}

//! destructor
TreeView::~TreeView()
{
	delete _d;
}

void TreeView::recalculateItemsRectangle()
{
  Font curFont = Font::create( FONT_3 );
	TreeViewItem*	node;

	if( _d->font != curFont )
	{
		_d->font = curFont;
		itemHeight_ = 0;

    if( _d->font.isValid() )
      itemHeight_ = _d->font.getTextSize( "A" ).height() + 4;
	}

  IndentWidth = math::clamp<int>( itemHeight_, 9, 15) - 1;

  _d->totalItemSize = Size( 0, 0 );
	node = Root->getFirstChild();
	while( node )
	{
    _d->totalItemSize += Size( 0, itemHeight_ );
    _d->totalItemSize.setWidth( math::max( _d->totalItemSize.width(), node->screenRight() - Root->screenLeft() ) );
		node = node->getNextVisible();
	}

	//сделаем скролбары видимыми если элементы выходят за границы отображения по вертикали
  ScrollBarV->setMaxValue( math::max<int>(0, _d->totalItemSize.height() - height() + itemHeight_) );
  ScrollBarV->setVisible( _d->totalItemSize.height() > height() );

	//сделаем скролбары видимыми если элементы выходят за границы отображения по горизонтали
  ScrollBarH->setVisible( _d->totalItemSize.width() > width() );
  ScrollBarH->setMaxValue( math::max<int>(0, _d->totalItemSize.width() - width()) );
}

//! called if an event happened.
bool TreeView::onEvent( const NEvent &event )
{
  if ( enabled() )
	{
		switch( event.EventType )
		{
    case sEventGui:
      switch( event.gui.type )
			{
      case guiScrollbarChanged:
        if( event.gui.caller == ScrollBarV || event.gui.caller == ScrollBarH )
				{
          //int pos = ( ( gui::IGUIScrollBar* )event.GUIEvent.Caller )->getPos();
					updateItems();
					return true;
				}
				break;
      case guiElementFocusLost:
				{
					Selecting = false;
					return false;
				}
				break;
			default:
				break;
			}
			break;
    case sEventMouse:
			{
        Point p( event.mouse.pos() );

        switch( event.mouse.type )
				{
        case mouseWheel:
					if ( ScrollBarV )
            ScrollBarV->setValue( ScrollBarV->value() + (event.mouse.wheel < 0 ? -1 : 1) * -10 );
					return true;
					break;

        case mouseLbtnPressed:

          if ( isFocused() && !absoluteClippingRect().isPointInside(p) )
					{
						removeFocus();
						return false;
					}

					if( isFocused() &&
            (	( ScrollBarV && ScrollBarV->absoluteRect().isPointInside( p ) && ScrollBarV->onEvent( event ) ) ||
            ( ScrollBarH && ScrollBarH->absoluteRect().isPointInside( p ) &&	ScrollBarH->onEvent( event ) )
						)
						)
					{
						return true;
					}

					Selecting = true;
					setFocus();
					return true;
					break;

        case mouseLbtnRelease:
					if( isFocused() &&
            (	( ScrollBarV && ScrollBarV->absoluteRect().isPointInside( p ) && ScrollBarV->onEvent( event ) ) ||
            ( ScrollBarH && ScrollBarH->absoluteRect().isPointInside( p ) &&	ScrollBarH->onEvent( event ) )
						)
						)
					{
						return true;
					}

					Selecting = false;
					removeFocus();
          mouseAction( event.mouse.x, event.mouse.y );
					return true;
					break;

        case mouseMoved:
					if( Selecting )
					{
            if( absoluteRect().isPointInside( p ) )
						{
              mouseAction( event.mouse.x, event.mouse.y, true );
							return true;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}

  return parent()->onEvent( event );
}

/*!
*/
void TreeView::mouseAction( int xpos, int ypos, bool onlyHover /*= false*/ )
{
	TreeViewItem*		oldSelected = Selected;
	TreeViewItem*		hitNode = 0;
  int						selIdx;
  int						n;
	TreeViewItem*		node;
	NEvent					event;

  event.EventType			= sEventGui;
  event.gui.caller	= this;
  event.gui.element = 0;

  xpos -= screenLeft();//_absoluteRect.UpperLeftCorner.X;
  ypos -= screenTop();//_absoluteRect.UpperLeftCorner.Y;

	// find new selected item.
	if( itemHeight_ != 0 && ScrollBarV )
	{
    selIdx = ( ( ypos - 1 ) + ScrollBarV->value() ) / itemHeight_;
	}

	hitNode = 0;
	node = Root->getFirstChild();
	n = 0;
	while( node )
	{
		if( selIdx == n )
		{
			hitNode = node;
			break;
		}
		node = node->getNextVisible();
		++n;
	}

	if( hitNode && xpos > hitNode->getLevel() * IndentWidth )
	{
		Selected = hitNode;
	}

	if( hitNode && !onlyHover
		&& xpos < hitNode->getLevel() * IndentWidth
		&& xpos > ( hitNode->getLevel() - 1 ) * IndentWidth
		&& hitNode->hasChildren() )
	{
        hitNode->setExpanded( !hitNode->isExpanded() );

		// post expand/collaps news
        if( hitNode->isExpanded() )
		{
      event.gui.type = guiTreeviewNodeExpand;
		}
		else
		{
      event.gui.type = guiTreeviewNodeCollapse;
		}
		LastEventNode = hitNode;
    parent()->onEvent( event );
		LastEventNode = 0;
	}

	if( Selected && !Selected->isVisible() )
	{
		Selected = 0;
	}

	// post selection news

	if( !onlyHover && Selected != oldSelected )
	{
		if( oldSelected )
		{
      event.gui.type = guiTreeviewNodeSelect;
			LastEventNode = oldSelected;
      parent()->onEvent( event );
			LastEventNode = 0;
		}
		if( Selected )
		{
      event.gui.type = guiTreeviewNodeSelect;
			LastEventNode = Selected;
      parent()->onEvent( event );
			LastEventNode = 0;
		}
	}
}

void TreeView::updateItems()
{
	_d->needUpdateItems = true;
}

NColor TreeView::_GetCurrentNodeColor( TreeViewItem* node )
{
    NColor textCol = 0xffc0c0c0;
    ElementState state = stDisabled;

    if ( enabled() )
    {
        textCol = ( node == Selected ) ? 0xffffffff : 0xff000000;
        state = ( node == Selected ) ? stPressed : stNormal;
    }

    return textCol;
}

Font TreeView::getCurrentNodeFont_( TreeViewItem* node )
{
  Font font;
  if ( enabled() )
      font = Font::create( node == Selected ? FONT_2_WHITE : FONT_2 );

  if( font.isValid() )
  {
      if( node->font().isValid()  )
          return node->font();

      return font;
  }

  return Font::create( FONT_2 );
}

void TreeView::beforeDraw( gfx::Engine& painter )
{
  if( !visible() )
		return;

	if( _d->needUpdateItems )
	{
		_d->needUpdateItems = false;

		recalculateItemsRectangle(); // if the font changed

    Rect* clipRect = 0;
		if( Clip )
		{
      clipRect = &absoluteClippingRectRef();
		}

		// draw background
    Rect frameRect( absoluteRect() );

    Rect clientClip( absoluteRect() );
    clientClip.UpperLeftCorner.ry() += 1;
    clientClip.UpperLeftCorner.rx() += 1;
    clientClip.LowerRightCorner.rx() = screenRight();//_absoluteRect.LowerRightCorner.X;
    clientClip.LowerRightCorner.ry() -= 1;

		if ( ScrollBarV )
      clientClip.LowerRightCorner.rx() -= DEFAULT_SCROLLBAR_SIZE;
		if ( ScrollBarH )
      clientClip.LowerRightCorner.ry() -= DEFAULT_SCROLLBAR_SIZE;

		if( clipRect )
		{
			clientClip.clipAgainst( *clipRect );
			frameRect = clientClip;
		}
		else
		{
      frameRect = absoluteRect();
		}
		//frameRect.LowerRightCorner.X = getScreenRight() - DEFAULT_SCROLLBAR_SIZE;
		//frameRect.LowerRightCorner.Y = getScreenTop() + ItemHeight;

    frameRect -= Point( ScrollBarH->value(), ScrollBarV->value() );

    Rect startRect = frameRect;
		TreeViewItem* node = Root->getFirstChild();

		while( node )
		{
			frameRect = startRect + Point( 1 + node->getLevel() * IndentWidth, 0 );
			startRect += Point( 0, itemHeight_ );
			
			Point offset( 0, 0 );
			TreeViewItem* itemOffset = node->getParentItem();
			while( itemOffset != 0 )
			{
        offset += itemOffset->lefttop();//getRelativePosition().UpperLeftCorner;
				itemOffset = itemOffset->getParentItem();
			}

      Rect nodeRect = frameRect - absoluteRect().lefttop() - offset;
      Font fontNode = node->font();
      if( !fontNode.isValid() )
        fontNode = Font::create( FONT_2 );

			nodeRect.LowerRightCorner = nodeRect.UpperLeftCorner 
        + Point( fontNode.getTextSize( node->text() ).width() + IndentWidth, itemHeight_ );
			node->setGeometry( nodeRect );		

			node = node->getNextVisible();
		}
	}

  Widget::beforeDraw( painter );
}

void drawRect( const Rect& r, const NColor& color, gfx::Engine& e, Rect* clipRect )
{
  //if( clipRect )

  e.drawLine( color,r.lefttop(), r.righttop() );
  e.drawLine( color,r.lefttop(), r.leftbottom() );
  e.drawLine( color,r.leftbottom(), r.rightbottom() );
  e.drawLine( color,r.rightbottom(), r.righttop() );
}

//! draws the element and its children
void TreeView::draw( gfx::Engine& painter )
{
  if( !visible() )
		return;

  Rect* clipRect = 0;
	if( Clip )
	{
    clipRect = &absoluteClippingRectRef();
	}

	// draw background
	if( DrawBack )
	{
    drawRect( absoluteRect(), 0xffffffff, painter, 0 );
	}

  Rect clientClip( absoluteRect() );
  clientClip.UpperLeftCorner.ry() += 1;
  clientClip.UpperLeftCorner.rx() += 1;
  clientClip.LowerRightCorner.rx() = screenRight();
  clientClip.LowerRightCorner.ry() -= 1;

	if ( ScrollBarV )
    clientClip.LowerRightCorner.rx() -= DEFAULT_SCROLLBAR_SIZE;
	if ( ScrollBarH )
    clientClip.LowerRightCorner.ry() -= DEFAULT_SCROLLBAR_SIZE;

	if( clipRect )
		clientClip.clipAgainst( *clipRect );

	TreeViewItem* node = Root->getFirstChild();
    //ElementStyle& itemStyle = myStyle.getSubStyle( NES_ITEM );
  Rect frameRect;
	while( node )
	{
    frameRect = node->absoluteRect();

		if( node->hasChildren() )
		{
			//рамка для плюса
      Rect expanderRect( frameRect.UpperLeftCorner + Point( -IndentWidth + 2, ( IndentWidth - 4 ) / 2 ),
                         Size( IndentWidth - 4, IndentWidth - 4 ) );

      drawRect( expanderRect, 0xffc0c0c0, painter, clipRect );

			// horizontal '-' line
      Point center = expanderRect.center();
      Point offset( expanderRect.width() / 3, 0 );

      drawRect( Rect( center - offset, center + offset + Point( 1,1 ) ),
                0xff000000, painter, clipRect );

			if( !node->isExpanded() )
			{
				// vertical '+' line
        offset = Point( 0, expanderRect.width() / 3 );
        drawRect( Rect( center - offset, center + offset + Point( 1,1 ) ),
                  0xff000000,
                  painter, clipRect );
			}
		}			

  		// draw the lines if neccessary
		if( LinesVisible )
		{
      Rect rc;

			// horizontal line
      rc.UpperLeftCorner = frameRect.UpperLeftCorner + Point( -( IndentWidth * 3 / 2 ) - 1, itemHeight_ / 2 );
			
      rc.LowerRightCorner.rx() = frameRect.UpperLeftCorner.rx() + (node->hasChildren() ? -IndentWidth + 2 : -2);
      rc.LowerRightCorner.ry() = rc.UpperLeftCorner.ry() + 1;

      drawRect( rc, 0xffc0c0c0, painter, clipRect );

			if( node->getParentItem() != Root )
			{
				// vertical line
        int offsetY = ( node == node->getParentItem()->getFirstChild() ? IndentWidth : 0 );
        rc.UpperLeftCorner.ry() = frameRect.UpperLeftCorner.y() - ( itemHeight_ - offsetY ) / 2;

        rc.LowerRightCorner.rx() = rc.UpperLeftCorner.rx() + 1;
        drawRect( rc, 0xffc0c0c0, painter, clipRect );

				// the vertical lines of all parents
				TreeViewItem* nodeTmp = node->getParentItem();
        rc.UpperLeftCorner.ry() = frameRect.UpperLeftCorner.y();

        for( int n = 0; n < node->getLevel() - 2; ++n )
				{
          rc -= Point( IndentWidth, 0 );

					if( nodeTmp != nodeTmp->getParentItem()->getLastChild() )
					{
            drawRect( rc, 0xffc0c0c0, painter, clipRect );
					}
					nodeTmp = nodeTmp->getParentItem();
				}
			}
		}

		node = node->getNextVisible();
	}


	// draw items
  Widget::draw( painter );
}


void TreeView::setImageLeftOfIcon( bool bLeftOf )
{
    ImageLeftOfIcon = bLeftOf;
}

bool TreeView::getImageLeftOfIcon() const
{
    return ImageLeftOfIcon;
}

TreeViewItem* TreeView::getLastEventNode() const
{
    return LastEventNode;
}

}
