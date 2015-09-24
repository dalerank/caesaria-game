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
// Copyright 2012-2015 Gregoire Athanase, gathanase@gmail.com

#ifndef __CAESARIA_FLOWLIST_H_INCLUDE__
#define __CAESARIA_FLOWLIST_H_INCLUDE__

#include "smartlist.hpp"

template< class T >
class FlowList : public SmartList<T>
{
public:
  void merge()
  {
    if( _willAdd.empty() )
      return;

    this->append( _willAdd );
    _willAdd.clear();
  }

  void postpone( SmartPtr<T> overlay ) { _willAdd.push_back(overlay); }

private:
  SmartList<T> _willAdd;
};

#endif
