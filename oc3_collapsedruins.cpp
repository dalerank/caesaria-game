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

#include "oc3_collapsedruins.h"
#include "pic_loader.hpp"
#include "oc3_resourcegroup.h"
#include "oc3_time.h"

CollapsedRuins::CollapsedRuins() : ServiceBuilding(S_COLLAPSED_RUINS)
{
    setType(B_COLLAPSED_RUINS);
    _size = 1;
    _damageLevel = 1;

    AnimLoader animLoader(PicLoader::instance());
    animLoader.fill_animation(_animation, ResourceGroup::sprites, 1, 8);
    animLoader.change_offset(_animation, 14, 26);
    _fgPictures.resize(1);           
}

CollapsedRuins* CollapsedRuins::clone() const
{
    return new CollapsedRuins(*this);
}

void CollapsedRuins::timeStep(const unsigned long time)
{
    if( time % 4 == 0 )
        ServiceBuilding::timeStep( time );    

    if( _animation.getCurrentIndex() == 7 )
    {
        _animation.init( std::vector<Picture*>() );
        _fgPictures.clear();
        getTile().get_terrain().setBuilding( true );
    }
}

void CollapsedRuins::deliverService()
{
    /*ServiceWalker walker(getService());
    walker.setServiceBuilding(*this);
    std::set<Building*> reachedBuildings = walker.getReachedBuildings(getTile().getI(), getTile().getJ());
    for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
    {
    Building &building = **itBuilding;
    building.applyService(walker);
    }*/
}

void CollapsedRuins::burn()
{

}

void CollapsedRuins::build( const int i, const int j )
{
    ServiceBuilding::build( i, j );
    //while burning can't remove it
    getTile().get_terrain().setTree( false );
    getTile().get_terrain().setBuilding( false );
    getTile().get_terrain().setRoad( false );
    setPicture(PicLoader::instance().get_picture( ResourceGroup::land2a, 111 + rand() % 8 ));
}   