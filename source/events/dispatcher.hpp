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

#ifndef _CAESARIA_EVENT_DISPATCHER_H_INCLUDE_
#define _CAESARIA_EVENT_DISPATCHER_H_INCLUDE_

#include "predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "event.hpp"
#include "core/singleton.hpp"

class Game;

namespace events
{

class Dispatcher : public StaticSingleton<Dispatcher>
{
public:
  Dispatcher();
  ~Dispatcher();

  void append( GameEventPtr event );
  void update( Game& game, uint time );

  VariantMap save() const;
  void load( const VariantMap& stream );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr<Dispatcher> DispatcherPtr;

} //end namespace events

#endif //_CAESARIA_EVENT_DISPATCHER_H_INCLUDE_
