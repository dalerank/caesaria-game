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

#ifndef _CAESARIA_WIDGET_FACTORY_H_INCLUDE_
#define _CAESARIA_WIDGET_FACTORY_H_INCLUDE_

#include "core/scopedptr.hpp"
#include <string>

namespace gui
{

class Widget;

class WidgetCreator
{
public:
  virtual Widget* create( Widget* parent ) = 0;
};

class WidgetFactory
{
public:
  WidgetFactory();
  ~WidgetFactory();

  Widget* create(const std::string& type, Widget* parent ) const;

  bool canCreate( const std::string& type ) const;

  void addCreator( const std::string& type, WidgetCreator* ctor );
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //_CAESARIA_WIDGET_FACTORY_H_INCLUDE_
