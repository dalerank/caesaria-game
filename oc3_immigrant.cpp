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

#include "oc3_immigrant.hpp"
#include "oc3_positioni.hpp"
#include "oc3_scenario.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_astarpathfinding.hpp"

class Immigrant::Impl
{
public:
  TilePos destination;
  Picture* cartPicture;
  City* city;
};

Immigrant::Immigrant( City& city ) : _d( new Impl )
{
  _walkerType = WT_IMMIGRANT;
  _walkerGraphic = WG_HOMELESS;
  _d->cartPicture = 0;
  _d->city = &city;
}

Immigrant* Immigrant::clone() const
{
  return 0;
}

void Immigrant::assignPath( Tile& startPoint )
{
  House* blankHouse = _findBlankHouse();
  
  _checkPath( startPoint, blankHouse );
}

House* Immigrant::_findBlankHouse()
{
  std::list<LandOverlay*> houses = _d->city->getBuildingList(B_HOUSE);
  House* blankHouse = 0;
  _d->destination = TilePos( -1, -1 );
  for( std::list<LandOverlay*>::iterator itHouse = houses.begin(); itHouse != houses.end(); ++itHouse )
  {
    if( House* house = dynamic_cast<House*>(*itHouse) )
    {
      if( house->getAccessRoads().size() > 0 && 
          ( house->getNbHabitants() < house->getMaxHabitants() ) )
      {
        blankHouse = house;
        _d->destination = house->getTile().getIJ();
        break;
      }
    }
  }

  return blankHouse;
}

void Immigrant::_checkPath( Tile& startPoint, Building* house )
{
  Propagator pathfinder;
  PathWay pathWay;
  pathfinder.init( startPoint );

  Tilemap& citymap = _d->city->getTilemap();
  Tile& destTile = house ? house->getTile() : citymap.at( _d->city->getRoadExitIJ() );

  bool pathFound = Pathfinder::getInstance().getPath( startPoint.getIJ(), destTile.getIJ(), pathWay, false );
  if( pathFound )
  {
     setPathWay( pathWay );
     setIJ( startPoint.getIJ() );
  }

  _isDeleted = !pathFound;  
}

void Immigrant::onDestination()
{  
  _isDeleted = true;
  bool gooutCity = true;
  if( _d->destination.getI() > 0 && _d->destination.getJ() > 0 )  //have destination
  {
    const Tile& tile = _d->city->getTilemap().at( _d->destination );

    LandOverlay* overlay = tile.get_terrain().getOverlay();
    if( House* house = dynamic_cast<House*>( overlay ) )
    {
      if( house->getNbHabitants() < house->getMaxHabitants() )
      {
        house->addHabitants( 1 );
        Walker::onDestination();
        gooutCity = false;
      }
    }
  }

  if( gooutCity )
  {
    House* blankHouse = _findBlankHouse();
    _checkPath( _d->city->getTilemap().at( getIJ() ), blankHouse );
  }
}

Immigrant* Immigrant::create( City& city, const Building& startPoint )
{
  Immigrant* newImmigrant = new Immigrant( city );
  newImmigrant->assignPath( startPoint.getTile() );
  city.addWalker( *newImmigrant );
  return newImmigrant;
}

Immigrant::~Immigrant()
{

}

void Immigrant::setCartPicture( Picture* pic )
{
  _d->cartPicture = pic;
}

Picture* Immigrant::getCartPicture()
{
  return _d->cartPicture;
}