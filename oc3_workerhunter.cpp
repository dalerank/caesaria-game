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

#include "oc3_workerhunter.hpp"
#include "oc3_house.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_positioni.hpp"

WorkersHunter::WorkersHunter( ServiceBuilding& building, const int workersNeeded  )
 : ServiceWalker(S_WORKERS_HUNTER)
{    
    setServiceBuilding( building );
    _workersNeeded = workersNeeded;
    _walkerGraphic = WG_LIBRARIAN;
    _walkerType = WT_WORKERS_HUNTER;
}

void WorkersHunter::hireWorkers( const int workers )
{
    getServiceBuilding().addWorkers( workers );
}

int WorkersHunter::getWorkersNeeded() const
{
    return _workersNeeded;
}

void WorkersHunter::onNewTile()
{
  Walker::onNewTile();

  std::set<Building*> reachedBuildings = getReachedBuildings( getIJ() );
  for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
  {
    if( House* house = safety_cast< House* >( *itBuilding ) )
      (*itBuilding)->applyService(*this);
  }
}