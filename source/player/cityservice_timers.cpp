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

#include "cityservice_timers.hpp"
#include "core/time.hpp"
#include <vector>

typedef std::vector< TimerPtr > Timers;

class CityServiceTimers::Impl
{
public:
  Timers timers;
};

CityServiceTimers& CityServiceTimers::getInstance()
{
  static CityServiceTimers inst;
  return inst;
}

CityServiceTimers::CityServiceTimers() 
  : CityService( "timers" ), _d( new Impl )
{ 
}

void CityServiceTimers::update( const unsigned int time )
{
  Timers::iterator it=_d->timers.begin(); 
  while( it != _d->timers.end() )
  { 
    if( !(*it)->isActive() )
    {
      it = _d->timers.erase( it );      
    }
    else
    {
      (*it)->update( time );
      it++;
    }
  }
}

void CityServiceTimers::addTimer( TimerPtr timer )
{
  _d->timers.push_back( timer );
}

CityServiceTimers::~CityServiceTimers()
{
}
