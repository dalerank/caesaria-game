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

#include "desirability_updater.hpp"
#include "game/game.hpp"
#include "helper.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"
#include "gfx/tilemap.hpp"

using namespace constants;

namespace city
{

class DesirabilityUpdater::Impl
{
public:
  PlayerCityPtr city;
  DateTime endTime;
  VariantMap events;
  bool isDeleted;
  int value;
  bool alsoInfluence;

  void update( bool positive );
};

SrvcPtr DesirabilityUpdater::create( PlayerCityPtr city )
{
  DesirabilityUpdater* e = new DesirabilityUpdater();
  e->_d->city = city;

  SrvcPtr ret( e );
  ret->drop();

  return ret;
}

void DesirabilityUpdater::update( const unsigned int time)
{
  if( time % GameDate::ticksInMonth() == 0 )
  {
    _d->isDeleted = (_d->endTime < GameDate::current());

    if( !_d->alsoInfluence )
    {      
      _d->alsoInfluence = true;
      _d->update( true );
    }

    events::Dispatcher::instance().load( _d->events );
  }
}

std::string DesirabilityUpdater::getDefaultName() { return "desirability_updater"; }
bool DesirabilityUpdater::isDeleted() const {  return _d->isDeleted; }

void DesirabilityUpdater::destroy()
{
  _d->update( false );
}

void DesirabilityUpdater::load(const VariantMap& stream)
{
  _d->endTime = stream.get( "endTime" ).toDateTime();
  _d->events = stream.get( "exec" ).toMap();
  _d->value = stream.get( "value" );
  _d->alsoInfluence = stream.get( "alsoInfluence", false );
}

VariantMap DesirabilityUpdater::save() const
{
  VariantMap ret;
  ret[ "endTime" ] = _d->endTime;
  ret[ "exec" ] = _d->events;
  ret[ "value" ] = _d->value;
  ret[ "alsoInfluence" ] = _d->alsoInfluence;

  return ret;
}

DesirabilityUpdater::DesirabilityUpdater() : Srvc( DesirabilityUpdater::getDefaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
  _d->alsoInfluence = false;
}

void DesirabilityUpdater::Impl::update(bool positive)
{
  if( city.isNull() )
    return;

  int size = city->tilemap().size();
  TilesArray tiles = city->tilemap().getArea( TilePos( 0, 0), Size( size ) );

  foreach( it, tiles )
  {
    (*it)->appendDesirability( positive ? value : -value);
  }
}

}//end namespace city
