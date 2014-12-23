#ifndef __CAESRAIA_TABLE_INCLUDE__
#define __CAESARIA_TABLE_INCLUDE__

#include "widget.hpp"

namespace gui
{

class AbstractFont;
class AbstractSkin;
class ScrollBar;

//! modes for ordering used when a column header is clicked
enum TableColumnOrderingMode
{
    //! Do not use ordering
    columnOrderingNone,

    //! Send a EGET_TABLE_HEADER_CHANGED message when a column header is clicked.
    columnOrderingCustom,

    //! Sort it ascending by it's ascii value like: a,b,c,...
    columnOrderingAscending,

    //! Sort it descending by it's ascii value like: z,x,y,...
    columnOrderingDescending,

    //! Sort it ascending on first click, descending on next, etc
    columnOrderingAscendingDescending,

    //! Not used as mode, only to get maximum value for this enum
    columnOrderingCount
};

//! Names for EGUI_COLUMN_ORDERING types
const std::string TableColumnOrderingNames[] =
{
  "none",
  "custom",
  "ascend",
  "descend",
  "ascend_descend",
  ""
};

enum TableRowOrderingMode
{
    //! No element ordering
    rowOrderingNone,

    //! Elements are ordered from the smallest to the largest.
    rowOrderingAscending,

    //! Elements are ordered from the largest to the smallest.
    rowOrderingDescending,

    //! this value is not used, it only specifies the amount of default ordering types
    //! available.
    rowOrderingCount
};

const std::string TableOrderingModeNames[] =
{
  "none",
  "ascending",
  "descending",
  ""
};

class Table : public Widget
{
public:
	typedef enum
	{
		drawRows = (1 << 0),
		drawColumns = (1 << 1),
		drawActiveRow = (1 << 2),
		drawRowBackground = (1 << 3),
		drawBorder = ( 1 << 4 ),
		drawCount
	} DrawFlag;
	//! constructor
	Table( Widget* parent,
			 int id, const Rect& rectangle, bool clip=true,
			 bool drawBack=false, bool moveOverSelect=true);

	//! destructor
	~Table();

	//! Adds a column
	//! If columnIndex is outside the current range, do push new colum at the end
	virtual void addColumn(const std::string& caption, unsigned int columnIndex=0xffffffff);

	//! remove a column from the table
	virtual void removeColumn(unsigned int columnIndex);

	//! Returns the number of columns in the table control
	virtual int getColumnCount() const;

	//! Makes a column active. This will trigger an ordering process.
	/** \param idx: The id of the column to make active.
	\return True if successful. */
	virtual bool setActiveColumn(int columnIndex, bool doOrder=false);

	//! Returns which header is currently active
	virtual int getActiveColumn() const;

	//! Returns the ordering used by the currently active column
	virtual TableRowOrderingMode getActiveColumnOrdering() const;

	//! set a column width
	virtual void setColumnWidth(unsigned int columnIndex, unsigned int width);

    //! returns column width
    virtual unsigned int getColumnWidth(unsigned int columnIndex) const;

	//! columns can be resized by drag 'n drop
	virtual void setResizableColumns(bool resizable);

	//! can columns be resized by dran 'n drop?
	virtual bool hasResizableColumns() const;

	//! This tells the table control which ordering mode should be used when
	//! a column header is clicked.
	/** \param columnIndex: The index of the column header.
	\param state: If true, a EGET_TABLE_HEADER_CHANGED message will be sent and you can order the table data as you whish.*/
	//! \param mode: One of the modes defined in EGUI_COLUMN_ORDERING
	virtual void setColumnOrdering(unsigned int columnIndex, TableColumnOrderingMode mode);

	//! Returns which row is currently selected
	virtual int getSelected() const;

	//! set wich row is currently selected
	virtual void setSelected( int index );

		virtual ScrollBar* getVerticalScrolBar();

		virtual int getSelectedColumn() const;

	//! Returns amount of rows in the tabcontrol
	virtual int getRowCount() const;

	//! adds a row to the table
	/** \param rowIndex: zero based index of rows. The row will be
		inserted at this position. If a row already exists
		there, it will be placed after it. If the row is larger
		than the actual number of rows by more than one, it
		won't be created. Note that if you create a row that is
		not at the end, there might be performance issues*/
	virtual unsigned int addRow(unsigned int rowIndex);

	//! Remove a row from the table
	virtual void removeRow(unsigned int rowIndex);

	//! clear the table rows, but keep the columns intact
	virtual void clearRows();

	//! Swap two row positions. This is useful for a custom ordering algo.
	virtual void swapRows(unsigned int rowIndexA, unsigned int rowIndexB);

	//! This tells the table to start ordering all the rows. You
	//! need to explicitly tell the table to reorder the rows when
	//! a new row is added or the cells data is changed. This makes
	//! the system more flexible and doesn't make you pay the cost
	//! of ordering when adding a lot of rows.
	//! \param columnIndex: When set to -1 the active column is used.
	virtual void orderRows(int columnIndex=-1, TableRowOrderingMode mode=rowOrderingNone);


	//! Set the text of a cell
	virtual void setCellText(unsigned int rowIndex, unsigned int columnIndex, const std::string& text);

  //! Set element of a cell
  virtual void setCellElement(unsigned int rowIndex, unsigned int columnIndex, Widget* elm );

  //! Remove element from cell
  virtual void removeCellElement(unsigned int rowIndex, unsigned int columnIndex);

  //! Get element from cell
  virtual Widget* getCellElement(unsigned int rowIndex, unsigned int columnIndex) const;

	//! Set the text of a cell, and set a color of this cell.
	virtual void setCellText(unsigned int rowIndex, unsigned int columnIndex, const std::string& text, NColor color);

	//! Set the data of a cell
	//! data will not be serialized.
	virtual void setCellData(unsigned int rowIndex, unsigned int columnIndex, void *data);

	//! Set the color of a cell text
	virtual void setCellTextColor(unsigned int rowIndex, unsigned int columnIndex, NColor color);

	//! Get the text of a cell
	virtual std::string getCellText(unsigned int rowIndex, unsigned int columnIndex ) const;

	//! Get the data of a cell
	virtual void* getCellData(unsigned int rowIndex, unsigned int columnIndex ) const;

	//! clears the table, deletes all items in the table
	virtual void clear();

  //! clears the contents int table, deletes all cell elements in the table
  virtual void clearContent();

	//! called if an event happened.
	virtual bool onEvent(const NEvent &event);

	//! draws the element and its children
	virtual void draw( gfx::Engine& painter );

	//! Set flags, as defined in EGUI_TABLE_DRAW_FLAGS, which influence the layout
	virtual void setDrawFlag( DrawFlag flag, bool enabled=true );

	//! Get the flags, as defined in EGUI_TABLE_DRAW_FLAGS, which influence the layout
	virtual bool isFlag( DrawFlag flag ) const;

	//!
	virtual void setItemHeight( int height );

  //!
  virtual void beforeDraw( gfx::Engine& painter );

  virtual void removeChild(Widget* child);

protected:
	virtual void refreshControls();
	virtual void recalculateScrollBars_();

private:
	void selectNew( int xpos, int ypos, bool lmb, bool onlyHover=false);
	bool selectColumnHeader(int xpos, int ypos);
	bool dragColumnStart(int xpos, int ypos);
	bool dragColumnUpdate(int xpos);
	void recalculateHeights();
	void recalculateColumnsWidth_();

  int getCurrentColumn_( int xpos, int ypos );
  void recalculateCells_();
  Font font_;
  bool Clip;
  bool MoveOverSelect;
  bool Selecting;
  int  CurrentResizedColumn;
  int  ResizeStart;
  bool ResizableColumns;
  std::string _headerStyle;

	int ItemHeight;
	int overItemHeight_;
	int TotalItemHeight;
	int TotalItemWidth;
	int _selectedRow, _selectedColumn;
	int CellHeightPadding;
	int CellWidthPadding;
	int ActiveTab;
	TableRowOrderingMode CurrentOrdering;

	class Impl;
	Impl* _d;
};

} //end namespace gui

#endif
