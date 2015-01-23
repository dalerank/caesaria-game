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
  friend class StaticSingleton;
public:
  virtual ~Dispatcher();

  void append( GameEventPtr event );
  void update( Game& game, unsigned int time );

  VariantMap save() const;
  void load( const VariantMap& stream );
  void load( vfs::Path filename, const std::string& section="" );

  void reset();

private:
  Dispatcher();

  class Impl;
  ScopedPtr< Impl > _d;
};

} //end namespace events

#endif //_CAESARIA_EVENT_DISPATCHER_H_INCLUDE_
