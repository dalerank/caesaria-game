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

#include "oc3_scenario_event_resolver.hpp"
#include "oc3_foreach.hpp"
#include "oc3_city.hpp"
#include <vector>

class ScenarioEventResolver::Impl
{
public:
  typedef std::vector< ScenarioEventPtr > Events;

  Events events;
  CityPtr city;
};

ScenarioEventResolver::ScenarioEventResolver(): _d( new Impl )
{
}

void ScenarioEventResolver::addEvent( ScenarioEventPtr event)
{
  _d->events.push_back( event );
}

void ScenarioEventResolver::update(unsigned int time)
{
  Impl::Events events = _d->events;
  _d->events.clear();

  foreach( ScenarioEventPtr event, events )
  {
    event->exec( _d->city );
  }
}


ScenarioEventResolverPtr ScenarioEventResolver::create(CityPtr city)
{
  ScenarioEventResolverPtr ret( new ScenarioEventResolver() );
  ret->_d->city = city;

  return ret;
}
