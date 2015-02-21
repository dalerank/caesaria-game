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

#ifndef __CAESARIA_EVENT_FACTORY_H_INCLUDED__
#define __CAESARIA_EVENT_FACTORY_H_INCLUDED__

#include "event.hpp"
#include "predefinitions.hpp"
#include "core/scopedptr.hpp"

namespace events
{

class GameEventCreator : public ReferenceCounted
{
public:
  virtual GameEventPtr create() = 0;
};

typedef SmartPtr<GameEventCreator> GameEventCreatorPtr;

template< class T >
class BaseGameEventCreator : public GameEventCreator
{
public:
  virtual GameEventPtr create()
  {
    GameEventPtr ret( T::create() );
    return ret;
  }
};

class EFactory
{
public:
  static GameEventPtr create( const std::string& name );
  static EFactory& instance();
  void addCreator( const std::string& name, GameEventCreatorPtr creator );

  template<class T>
  void addCreator( const std::string& name )
  {
    GameEventCreatorPtr ret( new BaseGameEventCreator<T>() );
    addCreator( name, ret );
  }

private:
  EFactory();

  class Impl;
  ScopedPtr<Impl> _d;
};

#define REGISTER_EVENT_IN_FACTORY(type,a) \
namespace { \
struct Registrator_##type { Registrator_##type() { EFactory::instance().addCreator<type>( a ); }}; \
static Registrator_##type rtor_##type; \
}

}//end namespace events

#endif//__CAESARIA_EVENT_FACTORY_H_INCLUDED__
