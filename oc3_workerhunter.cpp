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

#include "oc3_workerhunter.h"
#include "house.hpp"
#include "oc3_safetycast.h"
#include "oc3_positioni.h"

WorkersHunter::WorkersHunter( ServiceBuilding& building )
 : ServiceWalker(S_WORKERS_HUNTER)
{    
    setServiceBuilding( building );
}

int WorkersHunter::start( const int workersNeeded )
{
    _workersNeeded = workersNeeded;
    ServiceWalker walker( getService() );
    walker.setServiceBuilding( getServiceBuilding() );
    std::set<Building*> reachedBuildings = walker.getReachedBuildings( getServiceBuilding().getTile().getIJ() );
    for( std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
    {
        if( House* house = safety_cast< House* >( *itBuilding ) )
            house->applyService(walker);
    }

    return 0;
}

void WorkersHunter::hireWorkers( const int workers )
{
    getServiceBuilding().addWorkers( workers );
}

int WorkersHunter::getWorkersNeeded() const
{
    return _workersNeeded;
}