#include "table.hpp"
#include "label.hpp"
#include "scrollbar.hpp"
#include "pushbutton.hpp"
#include "gfx/engine.hpp"
#include "core/event.hpp"
#include "core/spring.hpp"
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
  }

	Widget* element;
};

struct Row
{
	Row() {}

	void erase( int index )
	{
    std::vector<Cell*>::iterator r = items.begin();
		std::advance( r, index );
		(*r)->deleteLater();
    items.erase( r );
	}

  std::vector<Cell*> items;
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

  int itemHeight;
  Widget* header;
  Widget* itemsArea;
  ScrollBar* verticalScrollBar;
  ScrollBar* horizontalScrollBar;
  bool needRefreshCellsGeometry;
  math::SpringColor spring;
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

public signals:
  Signal2<int,int> onCellSelectedSignal;
  Signal2<int,int> onCellClickedSignal;
};

//! constructor
Table::Table( Widget* parent,
								int id, const Rect& rectangle, bool clip,
								bool drawBack, bool moveOverSelect)
: Widget( parent, id, rectangle ),
  _clip(clip), _moveOverSelect(moveOverSelect),
  _selecting(false), _currentResizedColumn(-1), _resizeStart(0), _resizableColumns(true),
  _overItemHeight(0), _totalItemHeight(0), _totalItemWidth(0), _selectedRow(-1), _selectedColumn(-1),
  _cellHeightPadding(2), _cellWidthPadding(5), _activeTab(-1),
  _currentOrdering( rowOrderingNone ),
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
  _d->itemHeight = 0;
  _d->spring.setColor( DefaultColors::red );
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

  _recalculateHeights();
	refreshControls();
}

//! destructor
Table::~Table() {}

void Table::addColumn(const std::string& caption, unsigned int  columnIndex)
{
  Column* columnHeader = new Column( _d->header, Rect( 0, 0, 1, 1 ) );
  columnHeader->setSubElement( true );
  columnHeader->setText( caption );
  columnHeader->setGeometry( Rect( 0, 0,
                                   _font.getTextSize(caption).width() + (_cellWidthPadding * 2) + ARROW_PAD,
                                   _d->header->height() ) );

  columnHeader->OrderingMode = columnOrderingNone;

	if ( columnIndex >= _d->columns.size() )
	{
		_d->columns.push_back( columnHeader );
    foreach( it, _d->rows )
      it->items.push_back( new Cell( _d->itemsArea, Rect( 0, 0, 1, 1 ) ) );
	}
	else
	{
		ColumnIterator ci = _d->columns.begin();
		std::advance( ci, columnIndex );
		_d->columns.insert( ci, columnHeader);

    foreach( it, _d->rows  )
		{
      std::vector<Cell*>::iterator addIt = it->items.begin();
			std::advance( addIt, columnIndex );
      it->items.insert( addIt, new Cell( _d->itemsArea, Rect( 0, 0, 1, 1 ) ));
		}
	}

  if (_activeTab == -1)
    _activeTab = 0;

  _recalculateColumnsWidth();
  _recalculateCells();
  _recalculateScrollBars();
}

//! remove a column from the table
void Table::removeColumn(unsigned int  columnIndex)
{
	if ( columnIndex < _d->columns.size() )
	{
		ColumnIterator cIt = _d->columns.begin();
		std::advance( cIt, columnIndex );
		_d->columns.erase( cIt );

    foreach( it, _d->rows )
      it->erase( columnIndex );
	}

  if ( (int)columnIndex <= _activeTab )
    _activeTab = _d->columns.size() ? 0 : -1;

  _recalculateColumnsWidth();
}

int Table::columnCount() const {	return _d->columns.size();}
int Table::rowCount() const {	return _d->rows.size();}

bool Table::setActiveColumn(int idx, bool doOrder )
{
	if (idx < 0 || idx >= (int)_d->columns.size())
		return false;

  bool changed = (_activeTab != idx);

  _activeTab = idx;
  if ( _activeTab < 0 )
		return false;

	if ( doOrder )
	{
		switch ( _d->columns[idx]->OrderingMode )
		{
			case columnOrderingNone:
        _currentOrdering = rowOrderingNone;
				break;

			case columnOrderingCustom:
			{
        _currentOrdering = rowOrderingNone;

				NEvent event;
				event.EventType = sEventGui;
				event.gui.caller = this;
				event.gui.element = 0;
				event.gui.type = guiTableHeaderChanged;
				parent()->onEvent(event);
			}
			break;

			case columnOrderingAscending:
        _currentOrdering = rowOrderingAscending;
				break;

			case columnOrderingDescending:
        _currentOrdering = rowOrderingDescending;
				break;

			case columnOrderingAscendingDescending:
        _currentOrdering = (rowOrderingAscending == _currentOrdering ? rowOrderingDescending : rowOrderingAscending);
				break;

			default:
        _currentOrdering = rowOrderingNone;
		}

    orderRows( activeColumn(), _currentOrdering );
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

int Table::activeColumn() const {  return _activeTab; }
TableRowOrderingMode Table::getActiveColumnOrdering() const {  return _currentOrdering; }

void Table::setColumnWidth(unsigned int  columnIndex, unsigned int  width)
{
	if ( columnIndex < _d->columns.size() )
	{
    const unsigned int  MIN_WIDTH = _font.getTextSize(_d->columns[columnIndex]->text() ).width() + (_cellWidthPadding * 2);
		if ( width < MIN_WIDTH )
			width = MIN_WIDTH;

		_d->columns[ columnIndex ]->setWidth( width );
	}

  _recalculateColumnsWidth();
  _recalculateCells();
}

//! Get the width of a column
unsigned int  Table::columnWidth(unsigned int  columnIndex) const
{
	if ( columnIndex >= _d->columns.size() )
		return 0;

	return _d->columns[columnIndex]->width();
}

void Table::setResizableColumns(bool resizable){	_resizableColumns = resizable; }

bool Table::hasResizableColumns() const
{
  return _resizableColumns;
}

unsigned int Table::addRow(unsigned int  rowIndex)
{
	if ( rowIndex > _d->rows.size() )
		rowIndex = _d->rows.size();

	Row row;

	if ( rowIndex == _d->rows.size() )
		_d->rows.push_back( row );
	else
		_d->insertRow( row, rowIndex );

  _d->rows[rowIndex].items.resize( _d->columns.size() );

  for( unsigned int  i = 0 ; i < _d->columns.size() ; ++i )
    _d->rows[rowIndex].items[ i ] = NULL;

  for( unsigned int  i = 0 ; i < _d->columns.size() ; ++i )
    _d->rows[rowIndex].items[ i ] = new Cell( _d->itemsArea, Rect( 0, 0, 1, 1 ) );

  _recalculateHeights();
  _recalculateCells();
  _recalculateScrollBars();
  return rowIndex;
}

void Table::removeRow(unsigned int  rowIndex)
{
	if ( rowIndex > _d->rows.size() )
		return;

  for( unsigned int  colNum=0; colNum < _d->columns.size(); colNum++ )
    removeElementFromCell( rowIndex, colNum );

  _d->eraseRow( rowIndex );

	if ( !(_selectedRow < int(_d->rows.size())) )
		_selectedRow = _d->rows.size() - 1;

  _recalculateHeights();
  _recalculateScrollBars();
}

//! adds an list item, returns id of item
void Table::setCellText(unsigned int  row, unsigned int  column, const std::string& text)
{
  Cell* cell = _getCell( row, column );
  if( cell )
    cell->setText( text );
}

void Table::setCellText(unsigned int  row, unsigned int  column, const std::string& text, NColor color)
{
  Cell* cell = _getCell( row, column );
  if( cell )
  {
    cell->setText( text );
    cell->setColor( color );
	}
}

void Table::setCellTextColor(unsigned int row, unsigned int column, NColor color)
{
  Cell* cell = _getCell( row, column );
  if( cell )
    cell->setColor( color );
}


void Table::setCellData(unsigned int row, unsigned int column,
                        const std::string& name, Variant data)
{
  Cell* cell = _getCell( row, column );
  if( cell )
    cell->addProperty( name, data );
}


std::string Table::getCellText(unsigned int row, unsigned int column ) const
{
  Cell* cell = _getCell( row, column );
  if( cell )
    return cell->text();

	return std::string();
}

Variant Table::getCellData(unsigned int row, unsigned int column, const std::string& name ) const
{
  Cell* cell = _getCell( row, column );
  if( cell )
    return cell->getProperty( name );

  return Variant();
}

//! clears the list
void Table::clear()
{
  _selectedRow = -1;

  clearRows();

  foreach( cit, _d->columns )
    (*cit)->deleteLater();

	_d->rows.clear();
	_d->columns.clear();

	if (_d->verticalScrollBar)
		_d->verticalScrollBar->setValue(0);

	if ( _d->horizontalScrollBar )
		_d->horizontalScrollBar->setValue(0);

  _recalculateHeights();
  _recalculateColumnsWidth();
}

void Table::clearContent()
{
  for( unsigned int  rowNum=0; rowNum < _d->rows.size(); rowNum++ )
      for( unsigned int  colNum=0; colNum < _d->columns.size(); colNum++ )
          removeElementFromCell( rowNum, colNum );

  _recalculateCells();
}

void Table::clearRows()
{
  _selectedRow = -1;

	const Widgets& tableAreaChilds = _d->itemsArea->children();
  foreach( wit, tableAreaChilds )
		(*wit)->deleteLater();

	_d->rows.clear();

	if (_d->verticalScrollBar)
		_d->verticalScrollBar->setValue(0);

  _recalculateHeights();
}

/*!
*/
int Table::selectedRow() const {return _selectedRow; }

//! set wich row is currently selected
void Table::setSelectedRow( int index )
{
	_selectedRow = -1;
	if ( index >= 0 && index < (int) _d->rows.size() )
		_selectedRow = index;
}

void Table::_recalculateColumnsWidth()
{
  _totalItemWidth=0;
  foreach( it, _d->columns )
  {
    (*it)->setLeft( _totalItemWidth );
    _totalItemWidth += (*it)->width();
  }
}

Cell* Table::_getCell( unsigned int row, unsigned int column) const
{
  if ( row < _d->rows.size() && column < _d->columns.size() )
  {
    return _d->rows[row].items[column];
  }

  return 0;
}

void Table::_getCellUnderMouse(int xpos, int ypos, int& column, int& row)
{
  row = -1;
  column = -1;
  if (_d->itemHeight!=0)
    row = ((ypos - screenTop() /*-_d->itemHeight - 1*/) + _d->verticalScrollBar->value()) / _d->itemHeight;

  column = _getCurrentColumn( xpos, ypos );
}

void Table::_recalculateHeights()
{
  _totalItemHeight = 0;
  Font curFont = _font.isValid() ? _font : Font::create( FONT_2 );
  if( _font != curFont )
  {
    _font = curFont;
    _d->itemHeight = 0;

    if( _font.isValid() )
      _d->itemHeight = _overItemHeight == 0 ? _font.getTextSize("A").height() + (_cellHeightPadding * 2) : _overItemHeight;
	}

  _totalItemHeight = _d->itemHeight * _d->rows.size();		//  header is not counted, because we only want items
}


// automatic enabled/disabling and resizing of scrollbars
void Table::_recalculateScrollBars()
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
  if( _totalItemWidth > tableRect.width() )
	{
    tableRect.rbottom() -= _d->horizontalScrollBar->height();
		_d->horizontalScrollBar->setVisible( true );
    _d->horizontalScrollBar->setMaxValue( math::max<int>(0,_totalItemWidth - tableRect.width() ) );
	}

	// needs vertical scroll be visible?
  if( _totalItemHeight > tableRect.height() )
	{
    tableRect.rright() -= _d->verticalScrollBar->width();
		_d->verticalScrollBar->setVisible( true );
    _d->verticalScrollBar->setMaxValue( math::max<int>(0,_totalItemHeight - tableRect.height() + 2 * _d->verticalScrollBar->absoluteRect().width()));

		// check horizontal again because we have now smaller clientClip
		if ( !_d->horizontalScrollBar->visible() )
		{
      if( _totalItemWidth > tableRect.width() )
			{
        tableRect.rbottom() -= _d->horizontalScrollBar->height();
				_d->horizontalScrollBar->setVisible(true);
        _d->horizontalScrollBar->setMaxValue( math::max<int>(0,_totalItemWidth - tableRect.width() ) );
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

    _recalculateColumnsWidth();
  _recalculateHeights();
    _recalculateScrollBars();
    _recalculateCells();
}

void Table::_recalculateCells()
{
		int yPos = -_d->verticalScrollBar->value();
	int xPos = -_d->horizontalScrollBar->value();
		RowIterator rit = _d->rows.begin();
		for( ; rit != _d->rows.end(); rit++ )
		{
				ColumnIterator cit = _d->columns.begin();
				for( int index=0; cit != _d->columns.end(); cit++, index++ )
				{
            Rect rectangle( (*cit)->left() + xPos, yPos, (*cit)->right() + xPos, yPos + _d->itemHeight );
            rit->items[ index ]->setGeometry( rectangle );
            rit->items[ index ]->sendToBack();
				}

        yPos += _d->itemHeight;
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
          _currentResizedColumn = -1;
          _selecting = false;
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

          if( _dragColumnStart( event.mouse.x, event.mouse.y ) )
					{
						setFocus();
						return true;
					}

          _getCellUnderMouse( event.mouse.x, event.mouse.y, _lastColumnIndex, _lastRowIndex );
          if ( _selectColumnHeader( event.mouse.x, event.mouse.y ) )
						return true;

          _selecting = true;
					setFocus();
					return true;
        break;

				case mouseLbtnRelease:
          _currentResizedColumn = -1;
          _selecting = false;          
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

          _selectNew( event.mouse.x, event.mouse.y, true );          
          if( _lastColumnIndex == _selectedColumn && _lastRowIndex == _selectedRow )
            emit _d->onCellClickedSignal( _selectedRow, _selectedColumn );
					return true;
        break;

				case mouseMoved:
          if ( _currentResizedColumn >= 0 && _dragColumnUpdate(event.mouse.x) )
					{
            return true;
					}
          if ( (_selecting || _moveOverSelect) && absoluteRect().isPointInside(p) )
					{
            _selectNew(event.mouse.x, event.mouse.y, false );
            return true;
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

bool Table::_dragColumnStart(int xpos, int ypos)
{
  if ( !_resizableColumns )
		return false;

  if ( ypos > ( screenTop() + _d->itemHeight ) )
		return false;

	const int CLICK_AREA = 12;	// to left and right of line which can be dragged
	int pos = screenLeft() + 1;

	if ( _d->horizontalScrollBar && _d->horizontalScrollBar->visible() )
		pos -= _d->horizontalScrollBar->value();

  pos += _totalItemWidth;

	// have to search from the right as otherwise lines could no longer be resized when a column width is 0
	for ( int i = (int)_d->columns.size()-1; i >= 0 ; --i )
	{
		unsigned int  colWidth = _d->columns[i]->width();

		if ( xpos >= (pos - CLICK_AREA) && xpos < ( pos + CLICK_AREA ) )
		{
      _currentResizedColumn = i;
      _resizeStart = xpos;
			return true;
		}

		pos -= colWidth;
	}

	return false;
}

bool Table::_dragColumnUpdate(int xpos)
{
  if ( !_resizableColumns || _currentResizedColumn < 0 || _currentResizedColumn >= int(_d->columns.size()) )
	{
    _currentResizedColumn = -1;
		return false;
	}

  int w = int(_d->columns[_currentResizedColumn]->width()) + (xpos-_resizeStart);
	if ( w < 0 )
		w = 0;

  setColumnWidth(_currentResizedColumn, w );
  _resizeStart = xpos;

  _recalculateCells();
	return false;
}

int Table::_getCurrentColumn( int xpos, int ypos )
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

bool Table::_selectColumnHeader(int xpos, int ypos)
{
  if ( ypos > ( screenTop() + _d->itemHeight ) )
		return false;

  _selectedColumn = _getCurrentColumn( xpos, ypos );
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
    columnIndex = activeColumn();
	if ( columnIndex < 0 )
		return;

	if ( mode == rowOrderingAscending )
	{
		for ( int i = 0 ; i < int(_d->rows.size()) - 1 ; ++i )
		{
			for ( int j = 0 ; j < int(_d->rows.size()) - i - 1 ; ++j )
			{
        if ( _d->rows[j+1].items[columnIndex]->text() < _d->rows[j].items[columnIndex]->text() )
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
        if ( _d->rows[j].items[columnIndex]->text() < _d->rows[j+1].items[columnIndex]->text() )
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

void Table::_selectNew( int xpos, int ypos, bool lmb, bool onlyHover)
{
  int oldSelectedRow = _selectedRow;
  int oldSelectedColumn = _selectedColumn;

  if ( ypos < screenTop() )
		return;

	// find new selected item.
  if (_d->itemHeight!=0)
    _selectedRow = ((ypos - screenTop() /*-_d->itemHeight - 1*/) + _d->verticalScrollBar->value()) / _d->itemHeight;

  _selectedColumn = _getCurrentColumn( xpos, ypos );

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
    emit _d->onCellSelectedSignal(_selectedRow,_selectedColumn);

    if( _selectedRow < 0 || _selectedColumn < 0 )
      return;
  }
}

void Table::beforeDraw(gfx::Engine &painter)
{
	if( _d->needRefreshCellsGeometry )
	{
    _recalculateCells();
		_d->needRefreshCellsGeometry = false;
	}

  Widget::beforeDraw( painter );
}

//! draws the element and its children
void Table::draw( gfx::Engine& painter )
{
	if ( !visible() )
		return;

  foreach( itRow, _d->rows )
	{
		// draw row seperator
		if( _d->isFlag( drawRowBackground ) )
		{
			//skin->DrawElement( this, cellStyle.Normal(), rowRect, &clientClip, 0 ) ;
		}

    if( _d->isFlag( drawRows ) )
    {
      Rect lineRect( itRow->items[ 0 ]->absoluteRect() );
      lineRect.setTop( lineRect.bottom() - 1 );
      lineRect.setRight (screenRight());
      painter.drawLine( 0xffc0c0c0, lineRect.lefttop(), lineRect.rightbottom() );
    }
  }

  //NColor lineColor = getResultColor( 0xffc0c0c0 );
  if( _d->isFlag( drawColumns ) )
  {
    foreach( itCol, _d->columns )
    {
      Rect columnSeparator( Point( (*itCol)->screenRight(), screenTop() + 1 ),
                            Size( 1, height() - 2 ) );

      // draw column seperator
      painter.drawLine( 0xffc0c0c0, columnSeparator.lefttop(), columnSeparator.rightbottom() );
    }
  }

  if( _d->isFlag( drawActiveCell ) )
  {
    Cell* cell = _getCell( _selectedRow, _selectedColumn );
    if( cell )
    {
      Rect cellRect = cell->absoluteRect();
      _d->spring.update();

      painter.drawLine( _d->spring, cellRect.lefttop(), cellRect.righttop() );
      painter.drawLine( _d->spring, cellRect.lefttop(), cellRect.leftbottom() );
      painter.drawLine( _d->spring, cellRect.leftbottom(), cellRect.rightbottom() );
      painter.drawLine( _d->spring, cellRect.rightbottom(), cellRect.righttop() );
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
bool Table::isFlag( DrawFlag flag ) const {	return _d->isFlag( flag ); }

void Table::setRowHeight( int height )
{
  _overItemHeight = height;
  _d->itemHeight = _overItemHeight == 0 ? _font.getTextSize("A").height() + (_cellHeightPadding * 2) : _overItemHeight;
}

int Table::selectedColumn() const { return _selectedColumn; }

void Table::removeChild( Widget* child)
{
  for( unsigned int  row = 0 ; row < _d->rows.size() ; ++row )
  {
    for ( unsigned int  column = 0 ; column < _d->columns.size() ; ++column )
    {
      Cell* cell = _getCell( row, column );
      if( cell && cell->element == child )
      {
        cell->element = NULL;
        break;
      }
    }
  }

  Widget::removeChild( child );
}

void Table::setItemFont(Font font)
{
  _font = font;
}

void Table::addElementToCell( unsigned int row, unsigned int column, Widget* elm )
{
  Cell* cell = _getCell( row, column );
  if( cell )
  {
    if( elm != element( row, column ) )
		{
      removeElementFromCell( row, column );

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

void Table::removeElementFromCell( unsigned int row, unsigned int column )
{  
  Cell* cell = _getCell( row, column );
  if ( cell )
  {
    if( cell->element )
        cell->element->remove();

    cell->element = 0;
  }
}

Widget* Table::element( unsigned int row, unsigned int column) const
{
  Cell* cell = _getCell( row, column );
  if( cell )
    return cell->element;

  return NULL;
}

ScrollBar* Table::getVerticalScrolBar() { return _d->verticalScrollBar; }
Signal2<int,int>& Table::onCellSelected() { return _d->onCellSelectedSignal; }
Signal2<int,int>& Table::onCellClicked() { return _d->onCellClickedSignal; }

}//end namespace gui
