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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_UNDO_EVENT_H_INCLUDE_
#define _CAESARIA_UNDO_EVENT_H_INCLUDE_

#include "event.hpp"
#include "objects/constants.hpp"
#include "gfx/tilepos.hpp"

namespace events
{

class UndoAction : public GameEvent
{
public:
  typedef enum { unknown=0, built, destroyed, finished, clear, revert } Type;
  static GameEventPtr create( Type event, object::Type type=object::unknown,
                              const TilePos& pos = TilePos::invalid(), int money=0 );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  TilePos _pos;
  int _money;
  object::Type _overlayType;
  Type _action;
};

} //end namespace events
#endif //_CAESARIA_UNDO_EVENT_H_INCLUDE_
