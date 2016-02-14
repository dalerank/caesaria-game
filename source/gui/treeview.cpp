#include "treeview.hpp"
#include "treeview_item.hpp"
#include "scrollbar.hpp"
#include "core/event.hpp"
#include "core/color_list.hpp"
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
  bool drawBack, bool scrollBarVertical, bool scrollBarHorizontal)
  : Widget( parent, id, rectangle ),
  _root(0), _selected(0),
  _itemHeight( 0 ),
  _indentWidth( 0 ),
	ScrollBarH( 0 ),
	ScrollBarV( 0 ),
  _lastEventNode( 0 ),
  _linesVisible( true ),
  _selecting( false ),
  _clip( clip ),
  _drawBack( drawBack ),
  _imageLeftOfIcon( true ),
  _currentDrawState( stNormal ),
	_d( new TreeViewPrivate )
{
#ifdef _DEBUG
  setDebugName( "TreeView" );
#endif

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

  _root = new TreeViewItem( this );
  _root->isExpanded_ = true;

	recalculateItemsRectangle();
}

//! destructor
TreeView::~TreeView()
{}

void TreeView::recalculateItemsRectangle()
{
  Font curFont = Font::create( FONT_1 );
	TreeViewItem*	node;

	if( _d->font != curFont )
	{
		_d->font = curFont;
    _itemHeight = 0;

    if( _d->font.isValid() )
      _itemHeight = _d->font.getTextSize( "A" ).height() + 2;
	}

  _indentWidth = math::clamp<int>( _itemHeight, 9, 15) - 1;

  _d->totalItemSize = Size( 0, 0 );
  node = _root->getFirstChild();
	while( node )
	{
    _d->totalItemSize += Size( 0, _itemHeight );
    _d->totalItemSize.setWidth( math::max( _d->totalItemSize.width(), node->screenRight() - _root->screenLeft() ) );
		node = node->getNextVisible();
	}

	//сделаем скролбары видимыми если элементы выходят за границы отображения по вертикали
  ScrollBarV->setVisible( _d->totalItemSize.height() > (int)height() );
  ScrollBarV->setMaxValue( math::max<int>(0, _d->totalItemSize.height() - height() + _itemHeight) );

	//сделаем скролбары видимыми если элементы выходят за границы отображения по горизонтали
  ScrollBarH->setVisible( _d->totalItemSize.width() > (int)width() );
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
          _selecting = false;
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
        case NEvent::Mouse::mouseWheel:
					if ( ScrollBarV )
            ScrollBarV->setValue( ScrollBarV->value() + (event.mouse.wheel < 0 ? -1 : 1) * -10 );
					return true;
        break;

        case NEvent::Mouse::btnLeftPressed:

          if ( isFocused() && !absoluteClippingRect().isPointInside(p) )
					{
						removeFocus();
						return false;
					}

					if( isFocused() &&
              (
                ( ScrollBarV && ScrollBarV->absoluteRect().isPointInside( p ) && ScrollBarV->onEvent( event ) ) ||
                ( ScrollBarH && ScrollBarH->absoluteRect().isPointInside( p ) &&	ScrollBarH->onEvent( event ) )
              )
						)
					{
						return true;
					}

          _selecting = true;
					setFocus();
					return true;
        break;

        case NEvent::Mouse::mouseLbtnRelease:
					if( isFocused() &&
            (	( ScrollBarV && ScrollBarV->absoluteRect().isPointInside( p ) && ScrollBarV->onEvent( event ) ) ||
            ( ScrollBarH && ScrollBarH->absoluteRect().isPointInside( p ) &&	ScrollBarH->onEvent( event ) )
						)
						)
					{
						return true;
					}

          _selecting = false;
					removeFocus();
          mouseAction( event.mouse.x, event.mouse.y );
					return true;
        break;

        case NEvent::Mouse::moved:
          if( _selecting )
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
  TreeViewItem*		oldSelected = _selected;
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
  if( _itemHeight != 0 && ScrollBarV )
	{
    selIdx = ( ( ypos - 1 ) + ScrollBarV->value() ) / _itemHeight;
	}

	hitNode = 0;
  node = _root->getFirstChild();
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

  if( hitNode && xpos > hitNode->level() * _indentWidth )
	{
    _selected = hitNode;
	}

	if( hitNode && !onlyHover
    && xpos < hitNode->level() * _indentWidth
    && xpos > ( hitNode->level() - 1 ) * _indentWidth
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
    _lastEventNode = hitNode;
    parent()->onEvent( event );
    _lastEventNode = 0;
	}

  if( _selected && !_selected->isVisible() )
	{
    _selected = 0;
	}

	// post selection news

  if( !onlyHover && _selected != oldSelected )
	{
		if( oldSelected )
		{
      event.gui.type = guiTreeviewNodeSelect;
      _lastEventNode = oldSelected;
      parent()->onEvent( event );
      _lastEventNode = 0;
		}
    if( _selected )
		{
      event.gui.type = guiTreeviewNodeSelect;
      _lastEventNode = _selected;
      parent()->onEvent( event );
      _lastEventNode = 0;
		}
	}
}

void TreeView::updateItems()
{
	_d->needUpdateItems = true;
}

NColor TreeView::_getCurrentNodeColor( TreeViewItem* node )
{
    NColor textCol = 0xffc0c0c0;

    if ( enabled() )
    {
        textCol = ( node == _selected ) ? 0xffffffff : 0xff000000;
    }

    return textCol;
}

Font TreeView::getCurrentNodeFont_( TreeViewItem* node )
{
  Font font;
  if ( enabled() )
      font = Font::create( node == _selected ? FONT_1_WHITE : FONT_1 );

  if( font.isValid() )
  {
      if( node->font().isValid()  )
          return node->font();

      return font;
  }

  return Font::create( FONT_1 );
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
    if( _clip )
		{
      clipRect = &absoluteClippingRectRef();
		}

		// draw background
    Rect frameRect( absoluteRect() );

    Rect clientClip( absoluteRect() );
    clientClip.rtop() += 1;
    clientClip.rleft() += 1;
    clientClip.rright() = screenRight();//_absoluteRect.LowerRightCorner.X;
    clientClip.rbottom() -= 1;

		if ( ScrollBarV )
      clientClip.rright() -= DEFAULT_SCROLLBAR_SIZE;
		if ( ScrollBarH )
      clientClip.rbottom() -= DEFAULT_SCROLLBAR_SIZE;

		if( clipRect )
		{
			clientClip.clipAgainst( *clipRect );
			frameRect = clientClip;
		}
		else
		{
      frameRect = absoluteRect();
		}

    frameRect -= Point( ScrollBarH->value(), ScrollBarV->value() );

    Rect startRect = frameRect;
    TreeViewItem* node = _root->getFirstChild();

		while( node )
		{
      frameRect = startRect + Point( 1 + node->level() * _indentWidth, 0 );
      startRect += Point( 0, _itemHeight );
			
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
        fontNode = Font::create( FONT_1 );

      nodeRect._bottomright = nodeRect.lefttop()
        + Point( fontNode.getTextSize( node->text() ).width() + _indentWidth, _itemHeight );
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
  if( _clip )
	{
    clipRect = &absoluteClippingRectRef();
	}

	// draw background
  if( _drawBack )
	{
    drawRect( absoluteRect(), 0xffffffff, painter, 0 );
	}

  Rect clientClip( absoluteRect() );
  clientClip.rtop() += 1;
  clientClip.rleft() += 1;
  clientClip.rright() = screenRight();
  clientClip.rbottom() -= 1;

	if ( ScrollBarV )
    clientClip.rright() -= DEFAULT_SCROLLBAR_SIZE;
	if ( ScrollBarH )
    clientClip.rbottom() -= DEFAULT_SCROLLBAR_SIZE;

	if( clipRect )
		clientClip.clipAgainst( *clipRect );

  TreeViewItem* node = _root->getFirstChild();

  Rect frameRect;
  bool isFirst = true;
  Point prevPoint;
	while( node )
	{
    frameRect = node->absoluteRect();

		if( node->hasChildren() )
		{
      Rect expanderRect( frameRect.lefttop() + Point( -_indentWidth + 2, ( _indentWidth - 4 ) / 2 ),
                         Size( _indentWidth - 4, _indentWidth - 4 ) );

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

    drawRect( frameRect, ColorList::blue, painter, 0);
    // draw the lines if neccessary
    if( _linesVisible )
		{
      Rect rc;

			// horizontal line
      Point lp = frameRect.lefttop() + Point( -(_indentWidth) + 1, _itemHeight / 2 );
      Point rp = lp + Point( (node->hasChildren() ? 2 : _indentWidth - 3), 0 );

      painter.drawLine( ColorList::red, lp, rp );

      if( isFirst )
      {
        isFirst = false;
        prevPoint = lp;
      }
      else
      {
        rp = Point( lp.x(), prevPoint.y() );
        painter.drawLine( ColorList::red, lp, rp );
      }

      if( node->getParentItem() != _root )
			{
				// vertical line
        int offsetY = ( node == node->getParentItem()->getFirstChild() ? _indentWidth : 0 );
        rc.setTop( frameRect.top() - ( _itemHeight - offsetY ) / 2 );

        rc.setRight( rc.left() + 1 );
        drawRect( rc, 0xffc0c0c0, painter, clipRect );

				// the vertical lines of all parents
				TreeViewItem* nodeTmp = node->getParentItem();
        rc.setTop( frameRect.top() );

        for( int n = 0; n < node->level() - 2; ++n )
				{
          rc -= Point( _indentWidth, 0 );

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
    _imageLeftOfIcon = bLeftOf;
}

bool TreeView::getImageLeftOfIcon() const
{
    return _imageLeftOfIcon;
}

TreeViewItem* TreeView::getLastEventNode() const
{
    return _lastEventNode;
}

}
