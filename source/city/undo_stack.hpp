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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_UNDO_STACK_INCLUDE_H_
#define _CAESARIA_UNDO_STACK_INCLUDE_H_

#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/smartlist.hpp"
#include "objects/constants.hpp"
#include "city/predefinitions.hpp"
#include "core/predefinitions.hpp"
#include "core/signals.hpp"

namespace undo
{

class UStack
{
public:
  UStack();
  ~UStack();

  void init( PlayerCityPtr city );

  void build(object::Type what, const TilePos& where , int money);
  void destroy( const TilePos& where );

  void clear();

  void undo();
  bool isAvailableUndo();

public signals:
  Signal1<bool>& onUndoChange();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace undo

#endif  //_CAESARIA_UNDO_STACK_INCLUDE_H_
