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

#include "postpone.hpp"
#include "game/gamedate.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "city/requestdispatcher.hpp"
#include "core/logger.hpp"
#include "city/cityservice_factory.hpp"
#include "factory.hpp"

namespace events
{

class PostponeEvent::Impl
{
public:
  DateTime date;
  int population;
  bool mayDelete;
  std::string name;
  std::string type;
  VariantMap options;
};

GameEventPtr PostponeEvent::create( const std::string& type, const VariantMap& stream )
{
  PostponeEvent* pe = new PostponeEvent();

  std::string::size_type reshPos = type.find( "#" );
  pe->_d->name = type;
  pe->_d->type = reshPos != std::string::npos
                      ? type.substr( reshPos+1 )
                      : type;

  pe->load( stream );
  Logger::warningIf( pe->_d->type.empty(), "Unknown postpone event" );

  GameEventPtr ret( pe );
  ret->drop();

  if( pe->_d->type.empty() )
    return GameEventPtr();

  Logger::warning( "Load postpone event " + type );
  return ret;
}

PostponeEvent::~PostponeEvent(){}

void PostponeEvent::_exec(Game& game, uint)
{
  Logger::warning( "Start event name=" + _d->name + " type=" + _d->type );
  PlayerCityPtr city = game.getCity();
  if( "city_request" == _d->type )
  {    
    CityServicePtr service = city->findService( CityRequestDispatcher::getDefaultName() );
    CityRequestDispatcherPtr dispatcher = ptr_cast<CityRequestDispatcher>( service );

    if( dispatcher.isValid() )
    {
      dispatcher->add( _d->options );
    }

    return;
  }

  GameEventPtr e = GameEventFactory::create( _d->type );
  if( e.isValid() )
  {
    e->load( _d->options );
    e->dispatch();
    return;
  }

  CityServicePtr srvc = CityServiceFactory::create( _d->type, city );
  if( srvc.isValid() )
  {
    srvc->load( _d->options );
    city->addService( srvc );
  }

  _d->mayDelete = true;
}

bool PostponeEvent::_mayExec( Game& game, uint time ) const
{
  if( time % 25 == 1 )
  {
    bool dateCondition = true;
    if( _d->date.year() != -1000 )
    {
      dateCondition = _d->date <= GameDate::current();
    }

    bool popCondition = true;
    if( _d->population > 0 )
    {
      popCondition = game.getCity()->getPopulation() > _d->population;
    }

    _d->mayDelete = dateCondition && popCondition;
    return _d->mayDelete;
  }

  return false;
}

bool PostponeEvent::isDeleted() const{  return _d->mayDelete; }
VariantMap PostponeEvent::save() const {  return _d->options; }

void PostponeEvent::load(const VariantMap& stream)
{  
  _d->date = stream.get( "date", DateTime( -1000, 1, 1 ) ).toDateTime();
  _d->population = (int)stream.get( "population", 0 );
  _d->type = stream.get( "eventType", Variant( _d->type ) ).toString();
  _d->name = stream.get( "eventName", Variant( _d->name ) ).toString();
  _d->options = stream;
}

PostponeEvent::PostponeEvent() : _d( new Impl )
{
  _d->mayDelete = false;
}

}
