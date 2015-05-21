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

#ifndef _CAESARIA_PROPERTYBROWSER_H_INCLUDE_
#define _CAESARIA_PROPERTYBROWSER_H_INCLUDE_

#include "editbox.hpp"
#include "table.hpp"
#include "core/variant_map.hpp"

namespace gui
{

class AbstractAttribute;

class PropertyBrowser : public Widget
{
public:

  //! constructor
  PropertyBrowser( Widget* parent, int id=-1);

  //! destructor
  ~PropertyBrowser();

  // gets the current attributes list
  virtual const VariantMap &getAttribs();

  void SetColumnWidth( float nameColWidth, float valColWidth );

  // update the attribute list after making a change
  void refreshAttribs();

  // save the attributes
  void updateAttribs();

  bool onEvent(const NEvent &event);
public signals:
  Signal0<>& onAttributeChanged();

protected:
  void resizeEvent_();

private:
  void _ClearAttributesList();
  void createTable_();
  void updateTable_();
  AbstractAttribute* createAttributElm_( std::string typeStr, const std::string& attrName );
  void addAttribute2Table_( AbstractAttribute* n, const std::string& offset="" );
  std::vector< AbstractAttribute* >	_attribList;	// attributes editing controls
  VariantMap	_attribs;	// current attributes
  Table* _attribTable;

  float _nameColumnWidth, _valueColumnWidth;

private signals:
  Signal0<> _attributeChanged;
};

}

#endif // _CAESARIA_PROPERTYBROWSER_H_INCLUDE_
