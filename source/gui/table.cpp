#include "table.hpp"
#include "label.hpp"
#include "scrollbar.hpp"
#include "pushbutton.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "core/flagholder.hpp"

#define ARROW_PAD 15
#define DEFAULT_SCROLLBAR_SIZE 16

namespace gui
{

class Cell : public Label
{
public:
  Cell( Widget* parent, const Rect& rectangle) : Label( parent, rectangle )
  {
      element = 0;
      Data = 0;
  }

	Widget* element;
	void *Data;
};


struct Row
{
	Row() {}

	void erase( int index )
	{
		std::vector<Cell*>::iterator r = Items.begin();
		std::advance( r, index );
		(*r)->deleteLater();
		Items.erase( r );
	}

	std::vector<Cell*> Items;
};

class HidingElement : public Label
{
public:
  HidingElement( Widget* parent, const Rect& rectangle )
      : Label( parent, rectangle )
  {

  }

  bool isPointInside(const Point& point) const
  {
      return false;
  }
};

class Column : public Label
{
public:
    Column( Widget* parent, const Rect& rectangle )
        : Label( parent, rectangle ), OrderingMode( columnOrderingNone)
    {}

    TableColumnOrderingMode OrderingMode;

    bool isPointInside(const Point& point) const
    {
        return false;
    }
};

typedef std::vector< Column* > Columns;
typedef Columns::iterator ColumnIterator;
typedef std::vector< Row > Rows;
typedef Rows::iterator RowIterator;

class Table::Impl : public FlagHolder<DrawFlag>
{
public:
	Columns columns;
	Rows rows;

  Widget* header;
  Widget* itemsArea;
  ScrollBar* verticalScrollBar;
  ScrollBar* horizontalScrollBar;
  bool needRefreshCellsGeometry;
  unsigned int  cellLastTimeClick;

  void insertRow( Row& row, int index )
  {
    Rows::iterator it = rows.begin();
    std::advance( it, index );
    rows.insert(it, row);
  }

  void eraseRow( int index )
  {
    Rows::iterator it = rows.begin();
    std::advance( it, index );
    rows.erase(it);
  }
};

//! constructor
Table::Table( Widget* parent,
								int id, const Rect& rectangle, bool clip,
								bool drawBack, bool moveOverSelect)
: Widget( parent, id, rectangle ),
	Clip(clip), MoveOverSelect(moveOverSelect),
	Selecting(false), CurrentResizedColumn(-1), ResizeStart(0), ResizableColumns(true),
	ItemHeight(0), overItemHeight_(0), TotalItemHeight(0), TotalItemWidth(0), _selectedRow(-1), _selectedColumn(-1),
	CellHeightPadding(2), CellWidthPadding(5), ActiveTab(-1),
	CurrentOrdering( rowOrderingNone ),
	_d( new Impl )
{
	#ifdef _DEBUG
			setDebugName( L"NrpTable" );
	#endif
		setDrawFlag( drawBorder );
		setDrawFlag( drawRows );
		setDrawFlag( drawColumns );
		setDrawFlag( drawActiveRow );

  _d->cellLastTimeClick = 0;
  _d->header = new HidingElement( this, Rect( 0, 0, width(), DEFAULT_SCROLLBAR_SIZE ) );
  _d->header->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::upperLeft );
  _d->header->setSubElement( true );

  _d->itemsArea = new HidingElement( this, Rect( 0, DEFAULT_SCROLLBAR_SIZE, width(), height() ) );
  _d->itemsArea->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight );
  _d->itemsArea->setSubElement( true );

	_d->verticalScrollBar = new ScrollBar( this, Rect( width() - DEFAULT_SCROLLBAR_SIZE, DEFAULT_SCROLLBAR_SIZE,
														width(), height() ), ScrollBar::Vertical );
	_d->verticalScrollBar->upButton()->hide();
	_d->verticalScrollBar->downButton()->hide();
	_d->verticalScrollBar->grab();
	_d->verticalScrollBar->setNotClipped(false);
	_d->verticalScrollBar->setSubElement(true);
	_d->verticalScrollBar->setVisibleFilledArea( false );
	_d->verticalScrollBar->setAlignment( align::lowerRight, align::lowerRight, align::upperLeft, align::lowerRight );

	_d->horizontalScrollBar = new ScrollBar( this, Rect( 0, height() - DEFAULT_SCROLLBAR_SIZE,
															width(), height() ), ScrollBar::Horizontal );
	_d->horizontalScrollBar->upButton()->hide();
	_d->horizontalScrollBar->downButton()->hide();
	_d->horizontalScrollBar->grab();
	_d->horizontalScrollBar->setNotClipped(false);
	_d->horizontalScrollBar->setSubElement(true);
	_d->horizontalScrollBar->setVisibleFilledArea( false );
	_d->horizontalScrollBar->setAlignment( align::upperLeft, align::lowerRight, align::lowerRight, align::lowerRight );

	recalculateHeights();
	refreshControls();
}

//! destructor
Table::~Table()
{
  delete _d;
}

void Table::addColumn(const std::string& caption, unsigned int  columnIndex)
{
  Column* columnHeader = new Column( _d->header, Rect( 0, 0, 1, 1 ) );
  columnHeader->setSubElement( true );
  columnHeader->setText( caption );
  columnHeader->setGeometry( Rect( 0, 0,
                                      font_.getTextSize(caption).width() + (CellWidthPadding * 2) + ARROW_PAD,
                                      _d->header->height() ) );

  columnHeader->OrderingMode = columnOrderingNone;

	if ( columnIndex >= _d->columns.size() )
	{
		_d->columns.push_back( columnHeader );
		RowIterator it = _d->rows.begin();
		for ( ; it != _d->rows.end(); ++it )
		{
			(*it).Items.push_back( new Cell( _d->itemsArea, Rect( 0, 0, 1, 1 ) ) );
		}
	}
	else
	{
		ColumnIterator ci = _d->columns.begin();
		std::advance( ci, columnIndex );
		_d->columns.insert( ci, columnHeader);
		RowIterator it = _d->rows.begin();
		for( ; it != _d->rows.end(); ++it )
		{
			std::vector<Cell*>::iterator addIt = (*it).Items.begin();
			std::advance( addIt, columnIndex );
			(*it).Items.insert( addIt, new Cell( _d->itemsArea, Rect( 0, 0, 1, 1 ) ));
		}
	}

	if (ActiveTab == -1)
		ActiveTab = 0;

  recalculateColumnsWidth_();
  recalculateCells_();
  recalculateScrollBars_();
}

//! remove a column from the table
void Table::removeColumn(unsigned int  columnIndex)
{
	if ( columnIndex < _d->columns.size() )
	{
		ColumnIterator cIt = _d->columns.begin();
		std::advance( cIt, columnIndex );
		_d->columns.erase( cIt );

		RowIterator it = _d->rows.begin();
		for( ; it != _d->rows.end(); ++it )
		{
			(*it).erase( columnIndex );
		}
	}

	if ( (int)columnIndex <= ActiveTab )
		ActiveTab = _d->columns.size() ? 0 : -1;

	recalculateColumnsWidth_();
}

int Table::getColumnCount() const {	return _d->columns.size();}
int Table::getRowCount() const {	return _d->rows.size();}

bool Table::setActiveColumn(int idx, bool doOrder )
{
	if (idx < 0 || idx >= (int)_d->columns.size())
		return false;

	bool changed = (ActiveTab != idx);

	ActiveTab = idx;
	if ( ActiveTab < 0 )
		return false;

	if ( doOrder )
	{
		switch ( _d->columns[idx]->OrderingMode )
		{
			case columnOrderingNone:
				CurrentOrdering = rowOrderingNone;
				break;

			case columnOrderingCustom:
			{
				CurrentOrdering = rowOrderingNone;

				NEvent event;
				event.EventType = sEventGui;
				event.gui.caller = this;
				event.gui.element = 0;
				event.gui.type = guiTableHeaderChanged;
				parent()->onEvent(event);
			}
			break;

			case columnOrderingAscending:
				CurrentOrdering = rowOrderingAscending;
				break;

			case columnOrderingDescending:
				CurrentOrdering = rowOrderingDescending;
				break;

			case columnOrderingAscendingDescending:
				CurrentOrdering = (rowOrderingAscending == CurrentOrdering ? rowOrderingDescending : rowOrderingAscending);
				break;

			default:
				CurrentOrdering = rowOrderingNone;
		}

		orderRows( getActiveColumn(), CurrentOrdering );
	}

	if( changed )
	{
		NEvent event;
		event.EventType = sEventGui;
		event.gui.caller = this;
		event.gui.element = 0;
		event.gui.type = guiTableHeaderChanged;
		parent()->onEvent(event);
	}

	return true;
}


int Table::getActiveColumn() const
{
	return ActiveTab;
}


TableRowOrderingMode Table::getActiveColumnOrdering() const
{
	return CurrentOrdering;
}


void Table::setColumnWidth(unsigned int  columnIndex, unsigned int  width)
{
	if ( columnIndex < _d->columns.size() )
	{
		const unsigned int  MIN_WIDTH = font_.getTextSize(_d->columns[columnIndex]->text() ).width() + (CellWidthPadding * 2);
		if ( width < MIN_WIDTH )
			width = MIN_WIDTH;

		_d->columns[ columnIndex ]->setWidth( width );
	}

	recalculateColumnsWidth_();
	recalculateCells_();
}

//! Get the width of a column
unsigned int  Table::getColumnWidth(unsigned int  columnIndex) const
{
	if ( columnIndex >= _d->columns.size() )
		return 0;

	return _d->columns[columnIndex]->width();
}

void Table::setResizableColumns(bool resizable)
{
	ResizableColumns = resizable;
}

bool Table::hasResizableColumns() const
{
	return ResizableColumns;
}

unsigned int  Table::addRow(unsigned int  rowIndex)
{
	if ( rowIndex > _d->rows.size() )
		rowIndex = _d->rows.size();

	Row row;

	if ( rowIndex == _d->rows.size() )
		_d->rows.push_back( row );
	else
		_d->insertRow( row, rowIndex );

  _d->rows[rowIndex].Items.resize( _d->columns.size() );

  for ( unsigned int  i = 0 ; i < _d->columns.size() ; ++i )
    _d->rows[rowIndex].Items[ i ] = NULL;

	for ( unsigned int  i = 0 ; i < _d->columns.size() ; ++i )
		_d->rows[rowIndex].Items[ i ] = new Cell( _d->itemsArea, Rect( 0, 0, 1, 1 ) );

  recalculateHeights();
  recalculateCells_();
  recalculateScrollBars_();
  return rowIndex;
}


void Table::removeRow(unsigned int  rowIndex)
{
	if ( rowIndex > _d->rows.size() )
		return;

    for( unsigned int  colNum=0; colNum < _d->columns.size(); colNum++ )
        removeCellElement( rowIndex, colNum );

  _d->eraseRow( rowIndex );

	if ( !(_selectedRow < int(_d->rows.size())) )
		_selectedRow = _d->rows.size() - 1;

	recalculateHeights();
	recalculateScrollBars_();
}

//! adds an list item, returns id of item
void Table::setCellText(unsigned int  rowIndex, unsigned int  columnIndex, const std::string& text)
{
	if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
	{
		_d->rows[rowIndex].Items[columnIndex]->setText( text );
	}
}

void Table::setCellText(unsigned int  rowIndex, unsigned int  columnIndex, const std::string& text, NColor color)
{
	if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
	{
		_d->rows[rowIndex].Items[columnIndex]->setText( text );
				_d->rows[rowIndex].Items[columnIndex]->setColor( color );
	}
}


void Table::setCellTextColor(unsigned int  rowIndex, unsigned int  columnIndex, NColor color)
{
	if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
	{
		_d->rows[rowIndex].Items[columnIndex]->setColor( color );
	}
}


void Table::setCellData(unsigned int  rowIndex, unsigned int  columnIndex, void *data)
{
	if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
	{
		_d->rows[rowIndex].Items[columnIndex]->Data = data;
	}
}


std::string Table::getCellText(unsigned int  rowIndex, unsigned int  columnIndex ) const
{
	if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
	{
		return _d->rows[rowIndex].Items[columnIndex]->text();
	}

	return std::string();
}


void* Table::getCellData(unsigned int  rowIndex, unsigned int  columnIndex ) const
{
	if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
	{
		return _d->rows[rowIndex].Items[columnIndex]->Data;
	}

	return 0;
}


//! clears the list
void Table::clear()
{
    _selectedRow = -1;

    clearRows();

    ColumnIterator cit = _d->columns.begin();
    for( ; cit != _d->columns.end(); cit++ )
        (*cit)->deleteLater();

	_d->rows.clear();
	_d->columns.clear();

	if (_d->verticalScrollBar)
		_d->verticalScrollBar->setValue(0);
	if ( _d->horizontalScrollBar )
		_d->horizontalScrollBar->setValue(0);

	recalculateHeights();
	recalculateColumnsWidth_();
}

void Table::clearContent()
{
    for( unsigned int  rowNum=0; rowNum < _d->rows.size(); rowNum++ )
        for( unsigned int  colNum=0; colNum < _d->columns.size(); colNum++ )
            removeCellElement( rowNum, colNum );

    recalculateCells_();
}

void Table::clearRows()
{
		_selectedRow = -1;

	const Widgets& tableAreaChilds = _d->itemsArea->children();
	ConstChildIterator wit = tableAreaChilds.begin();
	for( ; wit != tableAreaChilds.end(); wit++ )
		(*wit)->deleteLater();

	_d->rows.clear();

	if (_d->verticalScrollBar)
		_d->verticalScrollBar->setValue(0);

	recalculateHeights();
}

/*!
*/
int Table::getSelected() const
{
	return _selectedRow;
}

//! set wich row is currently selected
void Table::setSelected( int index )
{
	_selectedRow = -1;
	if ( index >= 0 && index < (int) _d->rows.size() )
		_selectedRow = index;
}


void Table::recalculateColumnsWidth_()
{
  TotalItemWidth=0;
    ColumnIterator it = _d->columns.begin();
  for ( ; it != _d->columns.end(); ++it )
    {
        (*it)->setLeft( TotalItemWidth );
    TotalItemWidth += (*it)->width();
    }
}


void Table::recalculateHeights()
{
  TotalItemHeight = 0;
  Font curFont = Font::create( FONT_2 );
  if( font_ != curFont )
  {
    font_ = curFont;
    ItemHeight = 0;

		if( font_.isValid() )
			ItemHeight = overItemHeight_ == 0 ? font_.getTextSize("A").height() + (CellHeightPadding * 2) : overItemHeight_;
	}

	TotalItemHeight = ItemHeight * _d->rows.size();		//  header is not counted, because we only want items
}


// automatic enabled/disabling and resizing of scrollbars
void Table::recalculateScrollBars_()
{
	if ( !_d->horizontalScrollBar || !_d->verticalScrollBar )
		return;

	bool wasHorizontalScrollBarVisible = _d->horizontalScrollBar->visible();
	bool wasVerticalScrollBarVisible = _d->verticalScrollBar->visible();
	_d->horizontalScrollBar->setVisible(false);
	_d->verticalScrollBar->setVisible(false);

	// CAREFUL: near identical calculations for tableRect and clientClip are also done in draw
	// area of table used for drawing without scrollbars
	Rect tableRect( _d->itemsArea->relativeRect() );

	// needs horizontal scroll be visible?
	if( TotalItemWidth > tableRect.width() )
	{
		tableRect.LowerRightCorner.ry() -= _d->horizontalScrollBar->height();
		_d->horizontalScrollBar->setVisible( true );
		_d->horizontalScrollBar->setMaxValue( math::max<int>(0,TotalItemWidth - tableRect.width() ) );
	}

	// needs vertical scroll be visible?
	if( TotalItemHeight > tableRect.height() )
	{
		tableRect.LowerRightCorner.rx() -= _d->verticalScrollBar->width();
		_d->verticalScrollBar->setVisible( true );
		_d->verticalScrollBar->setMaxValue( math::max<int>(0,TotalItemHeight - tableRect.height() + 2 * _d->verticalScrollBar->absoluteRect().width()));

		// check horizontal again because we have now smaller clientClip
		if ( !_d->horizontalScrollBar->visible() )
		{
			if( TotalItemWidth > tableRect.width() )
			{
				tableRect.LowerRightCorner.ry() -= _d->horizontalScrollBar->height();
				_d->horizontalScrollBar->setVisible(true);
				_d->horizontalScrollBar->setMaxValue( math::max<int>(0,TotalItemWidth - tableRect.width() ) );
			}
		}
	}

	// find the correct size for the vertical scrollbar
	if ( _d->verticalScrollBar->visible() )
	{
		if  (!wasVerticalScrollBarVisible )
			_d->verticalScrollBar->setValue(0);

		int offset = _d->horizontalScrollBar->visible()
						? _d->horizontalScrollBar->height()
						: 0;

		_d->verticalScrollBar->setHeight( height()-(1 + offset) );
		_d->verticalScrollBar->setLeft( width() - _d->verticalScrollBar->width() );
	}

	// find the correct size for the horizontal scrollbar
	if ( _d->horizontalScrollBar->visible() )
	{
		if ( !wasHorizontalScrollBarVisible )
			_d->horizontalScrollBar->setValue(0);

		int offset = _d->verticalScrollBar->visible()
						? _d->verticalScrollBar->width()
						: 0;
		_d->horizontalScrollBar->setWidth( width()-(1+offset) );
		_d->horizontalScrollBar->setTop( height() - _d->horizontalScrollBar->height() );

	}

	_d->itemsArea->setWidth( width() - ( _d->verticalScrollBar->visible() ? 1 : 0 ) * _d->verticalScrollBar->width() );
	_d->itemsArea->setHeight( height() - _d->header->height()
								- ( _d->horizontalScrollBar->visible() ? 1 : 0 ) * _d->horizontalScrollBar->height() );

  _d->verticalScrollBar->bringToFront();
  _d->horizontalScrollBar->bringToFront();
}


void Table::refreshControls()
{
  updateAbsolutePosition();

		recalculateColumnsWidth_();
	recalculateHeights();
		recalculateScrollBars_();
		recalculateCells_();
}

void Table::recalculateCells_()
{
		int yPos = -_d->verticalScrollBar->value();
	int xPos = -_d->horizontalScrollBar->value();
		RowIterator rit = _d->rows.begin();
		for( ; rit != _d->rows.end(); rit++ )
		{
				ColumnIterator cit = _d->columns.begin();
				for( int index=0; cit != _d->columns.end(); cit++, index++ )
				{
						Rect rectangle( (*cit)->left() + xPos, yPos, (*cit)->right() + xPos, yPos + ItemHeight );
						(*rit).Items[ index ]->setGeometry( rectangle );
			(*rit).Items[ index ]->sendToBack();
				}

        yPos += ItemHeight;
    }
}

//! called if an event happened.
bool Table::onEvent(const NEvent &event)
{
	if ( enabled())
	{
		switch(event.EventType)
		{
		case sEventGui:
			switch(event.gui.type)
			{
			case guiScrollbarChanged:
				if( event.gui.caller == _d->verticalScrollBar
					|| event.gui.caller == _d->horizontalScrollBar)
				{
					_d->needRefreshCellsGeometry = true;
					return true;
				}
				break;
			case guiElementFocusLost:
				{
					CurrentResizedColumn = -1;
					Selecting = false;
				}
				break;
			default:
				break;
			}
			break;
		case sEventMouse:
			{
				Point p = event.mouse.pos();

				switch(event.mouse.type)
				{
				case mouseWheel:
					_d->verticalScrollBar->setValue( _d->verticalScrollBar->value() + (event.mouse.wheel < 0 ? -1 : 1)*-10);
					return true;

				case mouseLbtnPressed:

					if ( isFocused() &&
						_d->verticalScrollBar->visible() &&
						_d->verticalScrollBar->absoluteRect().isPointInside(p) &&
						_d->verticalScrollBar->onEvent(event))
						return true;

					if ( isFocused() &&
						_d->horizontalScrollBar->visible() &&
						_d->horizontalScrollBar->absoluteRect().isPointInside(p) &&
						_d->horizontalScrollBar->onEvent(event))
						return true;

					if( dragColumnStart( event.mouse.x, event.mouse.y ) )
					{
						setFocus();
						return true;
					}

					if ( selectColumnHeader( event.mouse.x, event.mouse.y ) )
						return true;

					Selecting = true;
					setFocus();
					return true;

				case mouseLbtnRelease:
					CurrentResizedColumn = -1;
					Selecting = false;
					if (!absoluteRect().isPointInside(p))
					{
						removeFocus();
					}

					if ( isFocused() &&
						_d->verticalScrollBar->visible() &&
						_d->verticalScrollBar->absoluteRect().isPointInside(p) &&
						_d->verticalScrollBar->onEvent(event))
					{
						return true;
					}

					if ( isFocused() &&
						_d->horizontalScrollBar->visible() &&
						_d->horizontalScrollBar->absoluteRect().isPointInside(p) &&
						_d->horizontalScrollBar->onEvent(event))
					{
						return true;
					}

					selectNew( event.mouse.x, event.mouse.y, true );
					return true;

				case mouseMoved:
					if ( CurrentResizedColumn >= 0 )
					{
						if ( dragColumnUpdate(event.mouse.x) )
						{
							return true;
						}
					}
					if (Selecting || MoveOverSelect)
					{
						if (absoluteRect().isPointInside(p))
						{
							selectNew(event.mouse.x, event.mouse.y, false );
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

	return Widget::onEvent(event);
}

void Table::setColumnOrdering(unsigned int  columnIndex, TableColumnOrderingMode mode)
{
	if ( columnIndex < _d->columns.size() )
		_d->columns[columnIndex]->OrderingMode = mode;
}

void Table::swapRows(unsigned int  rowIndexA, unsigned int  rowIndexB)
{
	if ( rowIndexA >= _d->rows.size() )
		return;

	if ( rowIndexB >= _d->rows.size() )
		return;

	Row swap = _d->rows[rowIndexA];
	_d->rows[rowIndexA] = _d->rows[rowIndexB];
	_d->rows[rowIndexB] = swap;

	if ( _selectedRow == int(rowIndexA) )
		_selectedRow = rowIndexB;
	else if( _selectedRow == int(rowIndexB) )
		_selectedRow = rowIndexA;

}

bool Table::dragColumnStart(int xpos, int ypos)
{
	if ( !ResizableColumns )
		return false;

	if ( ypos > ( screenTop() + ItemHeight ) )
		return false;

	const int CLICK_AREA = 12;	// to left and right of line which can be dragged
	int pos = screenLeft() + 1;

	if ( _d->horizontalScrollBar && _d->horizontalScrollBar->visible() )
		pos -= _d->horizontalScrollBar->value();

	pos += TotalItemWidth;

	// have to search from the right as otherwise lines could no longer be resized when a column width is 0
	for ( int i = (int)_d->columns.size()-1; i >= 0 ; --i )
	{
		unsigned int  colWidth = _d->columns[i]->width();

		if ( xpos >= (pos - CLICK_AREA) && xpos < ( pos + CLICK_AREA ) )
		{
			CurrentResizedColumn = i;
			ResizeStart = xpos;
			return true;
		}

		pos -= colWidth;
	}

	return false;
}

bool Table::dragColumnUpdate(int xpos)
{
	if ( !ResizableColumns || CurrentResizedColumn < 0 || CurrentResizedColumn >= int(_d->columns.size()) )
	{
		CurrentResizedColumn = -1;
		return false;
	}

	int w = int(_d->columns[CurrentResizedColumn]->width()) + (xpos-ResizeStart);
	if ( w < 0 )
		w = 0;

	setColumnWidth(CurrentResizedColumn, w );
	ResizeStart = xpos;

		recalculateCells_();
	return false;
}

int Table::getCurrentColumn_( int xpos, int ypos )
{
    int pos = screenLeft() + 1;

    if ( _d->horizontalScrollBar && _d->horizontalScrollBar->visible() )
        pos -= _d->horizontalScrollBar->value();

    for ( unsigned int  i = 0 ; i < _d->columns.size() ; ++i )
    {
        unsigned int  colWidth = _d->columns[i]->width();

        if ( xpos >= pos && xpos < ( pos + int(colWidth) ) )
            return i;

        pos += colWidth;
    }

    return -1;
}

bool Table::selectColumnHeader(int xpos, int ypos)
{
	if ( ypos > ( screenTop() + ItemHeight ) )
		return false;

    _selectedColumn = getCurrentColumn_( xpos, ypos );
    if( _selectedColumn >= 0 )
    {
        setActiveColumn( _selectedColumn, true );
        return true;
    }

  return false;
}

void Table::orderRows(int columnIndex, TableRowOrderingMode mode)
{
	Row swap;

	if ( columnIndex == -1 )
		columnIndex = getActiveColumn();
	if ( columnIndex < 0 )
		return;

	if ( mode == rowOrderingAscending )
	{
		for ( int i = 0 ; i < int(_d->rows.size()) - 1 ; ++i )
		{
			for ( int j = 0 ; j < int(_d->rows.size()) - i - 1 ; ++j )
			{
				if ( _d->rows[j+1].Items[columnIndex]->text() < _d->rows[j].Items[columnIndex]->text() )
				{
					swap = _d->rows[j];
					_d->rows[j] = _d->rows[j+1];
					_d->rows[j+1] = swap;

					if ( _selectedRow == j )
						_selectedRow = j+1;
					else if( _selectedRow == j+1 )
						_selectedRow = j;
				}
			}
		}
	}
	else if ( mode == rowOrderingDescending )
	{
		for ( int i = 0 ; i < int(_d->rows.size()) - 1 ; ++i )
		{
			for ( int j = 0 ; j < int(_d->rows.size()) - i - 1 ; ++j )
			{
				if ( _d->rows[j].Items[columnIndex]->text() < _d->rows[j+1].Items[columnIndex]->text() )
				{
					swap = _d->rows[j];
					_d->rows[j] = _d->rows[j+1];
					_d->rows[j+1] = swap;

					if ( _selectedRow == j )
						_selectedRow = j+1;
					else if( _selectedRow == j+1 )
						_selectedRow = j;
				}
			}
		}
	}
}

void Table::selectNew( int xpos, int ypos, bool lmb, bool onlyHover)
{
  int oldSelectedRow = _selectedRow;
    int oldSelectedColumn = _selectedColumn;

	if ( ypos < ( screenTop() + ItemHeight ) )
		return;

	// find new selected item.
	if (ItemHeight!=0)
		_selectedRow = ((ypos - screenTop() - ItemHeight - 1) + _d->verticalScrollBar->value()) / ItemHeight;

		_selectedColumn = getCurrentColumn_( xpos, ypos );

	if (_selectedRow >= (int)_d->rows.size())
		_selectedRow = _d->rows.size() - 1;
	else if (_selectedRow<0)
		_selectedRow = 0;

	// post the news
	if( !onlyHover )
	{
		NEvent event;
		event.EventType = sEventGui;
		event.gui.caller = this;
		event.gui.element = 0;
		event.gui.type = (_selectedRow != oldSelectedRow || _selectedColumn != oldSelectedColumn )
																				? guiTableCellChange
																				: guiTableCellSelected;
		parent()->onEvent(event);

		if( lmb && event.gui.type == guiTableCellSelected
			&& ( DateTime::elapsedTime() - _d->cellLastTimeClick ) < 200 )
		{
			event.gui.type = guiTableCellDblclick;
			parent()->onEvent( event );
		}
		_d->cellLastTimeClick = DateTime::elapsedTime();
				//CallScriptFunction( GUI_EVENT + event.GuiEvent.EventType, this );

        if( _selectedRow < 0 || _selectedColumn < 0 )
            return;
  }
}

void Table::beforeDraw(gfx::Engine &painter)
{
	if( _d->needRefreshCellsGeometry )
	{
		recalculateCells_();
		_d->needRefreshCellsGeometry = false;
	}

		Widget::beforeDraw( painter );
}

//! draws the element and its children
void Table::draw( gfx::Engine& painter )
{
	if ( !visible() )
		return;

	for ( unsigned int  i = 0 ; i < _d->rows.size() ; ++i )
	{
		// draw row seperator
		if( _d->isFlag( drawRowBackground ) )
		{
			//skin->DrawElement( this, cellStyle.Normal(), rowRect, &clientClip, 0 ) ;
		}

		// draw selected row background highlighted
		/*if ((int)i == _selectedRow && _d->isFlag( drawActiveRow ) )
		{
			Rect lineRect( _d->rows[ i ].Items[ 0 ]->absoluteRect() );
			lineRect.LowerRightCorner.X = screenRight();
			skin->drawElement( this, myStyle.Checked(), lineRect, &_d->itemsArea->getAbsoluteClippingRectRef() );
		}*/

        if( _d->isFlag( drawRows ) )
        {
            Rect lineRect( _d->rows[ i ].Items[ 0 ]->absoluteRect() );
            lineRect.UpperLeftCorner.ry() = lineRect.LowerRightCorner.y() - 1;
            lineRect.LowerRightCorner.rx() = screenRight();
            painter.drawLine( 0xffc0c0c0, lineRect.lefttop(), lineRect.rightbottom() );
        }
  }

    //NColor lineColor = getResultColor( 0xffc0c0c0 );
    if ( _d->isFlag( drawColumns ) )
    {
        ColumnIterator it = _d->columns.begin();
        for( ; it != _d->columns.end() ; ++it )
        {
            Rect columnSeparator( Point( (*it)->screenRight(), screenTop() + 1 ),
                                  Size( 1, height() - 2 ) );

            // draw column seperator
            painter.drawLine( 0xffc0c0c0, columnSeparator.lefttop(), columnSeparator.rightbottom() );
        }
    }

  Widget::draw( painter );

	// draw background for whole element
	//if( _d->isFlag( drawBorder ) )
	//	painter->drawRectangleOutline( getAbsoluteRect(), getResultColor(0xff000000), &getAbsoluteClippingRectRef());
}

//! Set some flags influencing the layout of the table
void Table::setDrawFlag( DrawFlag flag, bool enabled )
{
	_d->setFlag( flag, enabled );
}

//! Get the flags which influence the layout of the table
bool Table::isFlag( DrawFlag flag ) const
{
	return _d->isFlag( flag );
}

void Table::setItemHeight( int height )
{
	overItemHeight_ = height;
	ItemHeight = overItemHeight_ == 0 ? font_.getTextSize("A").height() + (CellHeightPadding * 2) : overItemHeight_;
}

int Table::getSelectedColumn() const
{
    return _selectedColumn;
}

void Table::removeChild( Widget* child)
{
    for ( unsigned int  rowIndex = 0 ; rowIndex < _d->rows.size() ; ++rowIndex )
        for ( unsigned int  columnIndex = 0 ; columnIndex < _d->columns.size() ; ++columnIndex )
        {
            Cell* cell = _d->rows[rowIndex].Items[columnIndex];
            if( cell && cell->element == child )
            {
                cell->element = NULL;
                break;
            }
        }

    Widget::removeChild( child );
}

void Table::setCellElement( unsigned int  rowIndex, unsigned int  columnIndex, Widget* elm )
{
    if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
    {
        Cell* cell = _d->rows[rowIndex].Items[columnIndex];

		if( elm != getCellElement( rowIndex, columnIndex ) )
		{
			removeCellElement( rowIndex, columnIndex );

			if( elm )
			{
				cell->addChild( elm );
				cell->element = elm;
			}
		}
		}

    _d->verticalScrollBar->bringToFront();
    _d->horizontalScrollBar->bringToFront();
}

void Table::removeCellElement( unsigned int  rowIndex, unsigned int  columnIndex )
{
    if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
    {
        Cell* cell = _d->rows[rowIndex].Items[columnIndex];

        if( cell->element )
            cell->element->remove();

        cell->element = 0;
    }
}

Widget* Table::getCellElement( unsigned int  rowIndex, unsigned int  columnIndex ) const
{
    if ( rowIndex < _d->rows.size() && columnIndex < _d->columns.size() )
    {
        return _d->rows[rowIndex].Items[columnIndex]->element;
    }

    return NULL;
}

ScrollBar* Table::getVerticalScrolBar()
{
    return _d->verticalScrollBar;
}

}//end namespace gui
