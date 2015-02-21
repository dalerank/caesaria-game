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

#ifndef _CAESARIA_EVENT_REMOVECITIZENS_H_INCLUDE_
#define _CAESARIA_EVENT_REMOVECITIZENS_H_INCLUDE_

#include "event.hpp"
#include "game/citizen_group.hpp"

namespace events
{

class RemoveCitizens : public GameEvent
{
public:
  static GameEventPtr create(TilePos center, const CitizenGroup& group );

protected:
  virtual void _exec(Game& game, unsigned int time );
  virtual bool _mayExec(Game&, unsigned int ) const;

private:
  RemoveCitizens();

  TilePos _center;
  CitizenGroup _group;
};

}

#endif //_CAESARIA_EVENT_REMOVECITIZENS_H_INCLUDE_
