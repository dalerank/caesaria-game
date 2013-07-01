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

#include "oc3_collapsedruins.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_positioni.hpp"
#include "oc3_time.hpp"
#include "oc3_tile.hpp"

CollapsedRuins::CollapsedRuins() : Building(B_COLLAPSED_RUINS, Size(1) )
{
    _damageLevel = 1;   

    _getAnimation().load( ResourceGroup::sprites, 1, 8 );
    _getAnimation().setOffset( Point( 14, 26 ) );
    _getAnimation().setFrameDelay( 4 );
    _getAnimation().setLoop( false );
    _fgPictures.resize(1); 
}

void CollapsedRuins::burn()
{

}

void CollapsedRuins::build( const TilePos& pos )
{
    Building::build( pos );
    //while burning can't remove it
    getTile().getTerrain().setTree( false );
    getTile().getTerrain().setBuilding( true );
    getTile().getTerrain().setRoad( false );
    setPicture( Picture::load( ResourceGroup::land2a, 111 + rand() % 8 ) );
}   

bool CollapsedRuins::isWalkable() const
{
  return true;
}

bool CollapsedRuins::isNeedRoadAccess() const
{
  return false;
}