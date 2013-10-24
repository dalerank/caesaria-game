// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _OPENCAESAR3_SCENARIO_EVENT_RESOLVER_H_INCLUDE_
#define _OPENCAESAR3_SCENARIO_EVENT_RESOLVER_H_INCLUDE_

#include "core/predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "event.hpp"
#include "core/singleton.hpp"
#include "core/signals.hpp"

namespace events
{

class Dispatcher : public StaticSingleton<Dispatcher>
{
public:
  Dispatcher();
  ~Dispatcher();

  static void append( GameEventPtr event );
  static void update( unsigned int time );

public oc3_signals:
  Signal1<GameEventPtr>& onEvent();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr<Dispatcher> DispatcherPtr;

} //end namespace events

#endif //_OPENCAESAR3_SCENARIO_EVENT_RESOLVER_H_INCLUDE_
