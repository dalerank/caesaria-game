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
  _attribTable->setAlignment( align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight);
  _attribTable->addColumn( "attribute", 0 );
  _attribTable->addColumn( "value", 1 );
  _attribTable->setSubElement( true );

  // refresh attrib list
  updateAbsolutePosition();
  refreshAttribs();
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
        AbstractAttribute* attr = safety_cast< AbstractAttribute* >( _attribTable->element( _attribTable->getSelected(), 1 ) );
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

void PropertyBrowser::_ClearAttributesList()
{
    // clear the attribute list
    unsigned int i;
    for (i=0; i<_attribList.size(); ++i)
    {
        if( _attribList[ i ] )
            _attribList[ i ]->deleteLater();
    }
    _attribList.clear();
}

void PropertyBrowser::refreshAttribs()
{
    _ClearAttributesList();

    _nameColumnWidth = _attribTable->getColumnWidth( 0 ) / float( _attribTable->width() );
    _valueColumnWidth = _attribTable->getColumnWidth( 1 ) / float( _attribTable->width() );
  // add attribute elements

  createTable_();
}

void PropertyBrowser::updateTable_()
{
    int oldPos = _attribTable->getVerticalScrolBar()->value();
    _attribTable->clearRows();

    unsigned int c = _attribs.size();
    //s32 rowCount=0;
    for (unsigned int i=0; i < c; ++i )
    {
        AbstractAttribute* n = _attribList[i];

        addAttribute2Table_( n );
    }
    _attribTable->getVerticalScrolBar()->setValue( oldPos );
}

AbstractAttribute* PropertyBrowser::createAttributElm_(std::string typeStr, const std::string &attrName )
{
    typeStr += "_attribute";

    WidgetFactory& wmgr = WidgetFactory::instance();
    AbstractAttribute* attr = (AbstractAttribute*)wmgr.create( typeStr, _attribTable);

    // if this doesn't exist, use a string editor
    if (!attr)
        attr = (AbstractAttribute*)wmgr.create( "string_attribute", _attribTable);

    attr->setAttributeName( attrName );
    return attr;
}

void PropertyBrowser::createTable_()
{
    _attribTable->clearRows();

    for( unsigned int i=0; i < _attribs.size(); ++i )
    {
        // try to create attribute
        /*AbstractAttribute* n = NULL;

        std::string startSection = _attribs->getAttributeName(i);
        if( startSection == SerializeHelper::subSectionStartProp )
        {
             n = new SubSectionAttribute( _attribTable, -1 );
             //n->drop();

             _attribList.push_back(n);
             n->setAttributeName( _attribs->getAttributeAsString(i) );

             _attribList[i]->setSubElement( true );
             _attribList[i]->setAttrib(_attribs, i);

             while( true )
             {
                 String endSection = _attribs->getAttributeName(++i);
                 if( endSection == SerializeHelper::subSectionEndProp )
                 {
                     _attribList.push_back( NULL );
                     break;
                 }

                 AbstractAttribute* subAttr = createAttributElm_( _attribs->getPropertyTypeString(i),
                                                                  _attribs->getAttributeName(i) );

                 n->addChild( subAttr );
                 subAttr->setAttrib( _attribs, i );
                 subAttr->setVisible( false );
                 subAttr->setParentID( getID() );
                 _attribList.push_back( NULL );
             }
        }
        else
        {
            n = createAttributElm_( _attribs->getPropertyTypeString(i), _attribs->getAttributeName(i) );

            _attribList.push_back(n);

            _attribList[i]->setSubElement( true );
            _attribList[i]->setAttrib( _attribs, i );
        }*/
    }

  for( unsigned int i=0; i < _attribList.size(); i++ )
        addAttribute2Table_( _attribList[ i ] );
}

void PropertyBrowser::addAttribute2Table_(AbstractAttribute* n, const std::string &offset )
{
    if( !n )
        return;

    unsigned int rowIndex = _attribTable->addRow( _attribTable->rowCount() );

    std::string prefix = "";
    if( n->GetChildAttributesCount() > 0 )
        prefix = n->IsExpanded() ? "-" : "+";

    _attribTable->setCellText( rowIndex, 0, offset + prefix + n->GetAttributeName() );
    _attribTable->addElementToCell( rowIndex, 1, n );
    n->setGeometry( RectF( 0, 0, 1, 1 ) );
    n->setAlignment(align::upperLeft, align::lowerRight, align::upperLeft, align::lowerRight);
    n->setVisible( true );

    for( unsigned int index=0; index < n->GetChildAttributesCount(); index++ )
    {
       AbstractAttribute* childAttr = n->GetChildAttribute( index );
       if( n->IsExpanded() )
         addAttribute2Table_( childAttr, offset + "\t" );

       childAttr->setVisible( n->IsExpanded() );
    }

    n->setParentID( ID() );
    n->grab();
}

void PropertyBrowser::updateAttribs()
{
  for (unsigned int i=0; i<_attribList.size(); ++i)
        if( _attribList[i] )
  _attribList[i]->updateAttrib(false);
}

void PropertyBrowser::resizeEvent_()
{
  _attribTable->setColumnWidth( 0, _nameColumnWidth * width() );
  _attribTable->setColumnWidth( 1, _valueColumnWidth * width() );
}

void PropertyBrowser::SetColumnWidth(float nameColWidth, float valColWidth )
{
    _nameColumnWidth = nameColWidth;
    _valueColumnWidth = valColWidth;
}

}//end namespace gui
