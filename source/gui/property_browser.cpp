// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "property_browser.hpp"
#include "core/event.hpp"
#include "scrollbar.hpp"
#include "property_attribute.hpp"
#include "widget_factory.hpp"
#include "core/utils.hpp"
#include "property_string.hpp"

namespace gui
{

PropertyBrowser::PropertyBrowser( Widget* parent, int id ) :
    Widget( parent, id, Rect(0, 0, 100, 100) ), _nameColumnWidth( 0.49f ),
    _valueColumnWidth( 0.49f )
{
  #ifdef _DEBUG
    setDebugName( "AttributeEditor");
  #endif

  // create attributes
  _attribTable = new Table( this, -1, Rect( 0, 0, 100, 100 ), false, false, false );
  _attribTable->setGeometry( RectF(0.0f, 0.0f, 1.0f, 1.0f));
  _attribTable->addColumn( "attribute", 0 );
  _attribTable->addColumn( "value", 1 );
  _attribTable->setItemFont( Font::create(FONT_1) );
  _attribTable->setSubElement( true );

  // refresh attrib list
  updateAbsolutePosition();
}

Signal0<>& PropertyBrowser::onAttributeChanged() { return _attributeChanged; }

PropertyBrowser::~PropertyBrowser()
{
  _attribList.clear();
}

bool PropertyBrowser::onEvent(const NEvent &event)
{
  if( event.EventType == AbstractAttribute::ATTRIBEDIT_ATTRIB_CHANGED )
  {
    emit _attributeChanged();
  }

  if( event.EventType == sEventGui
      && event.gui.caller == _attribTable
      && event.gui.type == guiTableCellDblclick )
  {
    AbstractAttribute* attr = safety_cast< AbstractAttribute* >( _attribTable->element( _attribTable->selectedRow(), 1 ) );
    if( attr )
    {
      attr->SetExpanded( !attr->IsExpanded() );
      updateTable_();
    }
  }

  return Widget::onEvent( event );
}

const VariantMap& PropertyBrowser::getAttribs()
{
  return _attribs;
}

void PropertyBrowser::setAttribs(const VariantMap &attribs)
{
  _attribs = attribs;
  _createTable();
}

void PropertyBrowser::_clearAttributesList()
{
  // clear the attribute list
  foreach( i, _attribList )
    (*i)->deleteLater();

  _attribList.clear();
}

void PropertyBrowser::updateTable_()
{
  int oldPos = _attribTable->getVerticalScrolBar()->value();
  _attribTable->clearRows();

  foreach( i, _attribList )
    addAttribute2Table_( *i );

  _attribTable->getVerticalScrolBar()->setValue( oldPos );
}

AbstractAttribute* PropertyBrowser::createAttributElm_(std::string typeStr, const std::string &attrName )
{
  typeStr = typeStr + AbstractAttribute::ATTRIBUTE_TYPENAME;

  WidgetFactory& wmgr = WidgetFactory::instance();
  AbstractAttribute* attr = (AbstractAttribute*)wmgr.create( typeStr, _attribTable);

  // if this doesn't exist, use a string editor
  if (!attr)
    attr = (AbstractAttribute*)wmgr.create( TEXT(StringAttribute), _attribTable);

  attr->setTitle( attrName );
  return attr;
}

void PropertyBrowser::_createTable()
{  
  foreach( it, _attribList )
    (*it)->deleteLater();

  _attribList.clear();
  _attribTable->clearRows();

  foreach( it, _attribs )
  {
    AbstractAttribute* n = createAttributElm_( it->second.typeName(), it->first );

    _attribList.push_back(n);
    n->setValue( it->second );
    n->setFont( Font::create( FONT_1 ) );

    _attribList.back()->setSubElement( true );
  }

  foreach( it, _attribList )
    addAttribute2Table_( *it );
}

void PropertyBrowser::addAttribute2Table_(AbstractAttribute* n, const std::string &offset )
{
  if( !n )
    return;

  unsigned int rowIndex = _attribTable->addRow( _attribTable->rowCount() );

  std::string prefix = "";
  if( n->childCount() > 0 )
      prefix = n->IsExpanded() ? "-" : "+";

  _attribTable->setCellText( rowIndex, 0, offset + prefix + n->title() );
  _attribTable->addElementToCell( rowIndex, 1, n );

  n->setVisible( true );

  for( unsigned int index=0; index < n->childCount(); index++ )
  {
     AbstractAttribute* childAttr = n->getChild( index );
     if( n->IsExpanded() )
       addAttribute2Table_( childAttr, offset + "\t" );

     childAttr->setVisible( n->IsExpanded() );
  }

  n->setParentID( ID() );
}

void PropertyBrowser::updateAttribs()
{
  for (unsigned int i=0; i<_attribList.size(); ++i)
    if( _attribList[i] )
      _attribList[i]->updateAttrib(false);
}

void PropertyBrowser::_finalizeResize()
{
  _attribTable->setGeometry( RectF( 0, 0, 1, 1 ));
  _attribTable->setColumnWidth( 0, _nameColumnWidth * width() );
  _attribTable->setColumnWidth( 1, _valueColumnWidth * width() );
}

void PropertyBrowser::SetColumnWidth(float nameColWidth, float valColWidth )
{
  _nameColumnWidth = nameColWidth;
  _valueColumnWidth = valColWidth;
}

}//end namespace gui
