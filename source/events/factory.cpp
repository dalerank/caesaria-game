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

#include "factory.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"

namespace events
{

class EFactory::Impl
{
public:
  typedef std::map< std::string, GameEventCreatorPtr > Creators;
  Creators creators;
};

GameEventPtr EFactory::create( const std::string& name )
{
  EFactory& inst = instance();
  Impl::Creators::iterator it = inst._d->creators.find( name );
  if( it != inst._d->creators.end() )
  {
    return it->second->create();
  }

  Logger::warning( "GameEventFactory: not found creator for event " + name );
  return GameEventPtr();
}

EFactory& EFactory::instance()
{
  static EFactory inst;
  return inst;
}

void EFactory::addCreator( const std::string& name, GameEventCreatorPtr creator )
{
  if( creator.isNull() )
    return;

  Impl::Creators::iterator it = _d->creators.find( name );

  if( it != _d->creators.end() )
  {
    Logger::warning( "GameEventFactory: Also have creator for event " + name );
    return;
  }

  _d->creators[ name ] = creator;
}

EFactory::EFactory() : _d( new Impl )
{
}

}//end namespace events
