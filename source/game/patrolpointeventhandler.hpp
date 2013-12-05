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

#ifndef _CAESARIA_HOUSE_HABITANT_INCLUDE_H_
#define _CAESARIA_HOUSE_HABITANT_INCLUDE_H_

#include "screen.hpp"
#include "core/scopedptr.hpp"

class Game;
class CityRenderer;

class PatrolPointEventHandler : public EventHandler
{
public:
  static EventHandlerPtr create( Game& game, CityRenderer& renderer );

  virtual void handleEvent(NEvent &event);
  virtual bool finished() const;

  virtual ~PatrolPointEventHandler();

private:
  PatrolPointEventHandler( Game& game, CityRenderer& renderer );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_HOUSE_HABITANT_INCLUDE_H_
