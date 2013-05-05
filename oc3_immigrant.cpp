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
  unsigned char peopleCount;
  City* city;
};

Immigrant::Immigrant( City& city, const unsigned char peoples ) : _d( new Impl )
{
  _walkerType = WT_IMMIGRANT;
  _walkerGraphic = WG_HOMELESS;
  _d->cartPicture = 0;
  _d->peopleCount = peoples;
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
  CityHelper hlp( *_d->city );
  std::list< House* > houses = hlp.getBuildings< House* >( B_HOUSE );
  House* blankHouse = 0;
  _d->destination = TilePos( -1, -1 );

  std::list< House* >::iterator itHouse = houses.begin();
  while( itHouse != houses.end() )
  {
    if( (*itHouse)->getAccessRoads().size() > 0 && 
        ( (*itHouse)->getNbHabitants() < (*itHouse)->getMaxHabitants() ) )
    {
      itHouse++;
    }
    else
    {
      itHouse = houses.erase( itHouse );
    }
  }

  if( houses.size() > 0 )
  {
    itHouse = houses.begin();
    std::advance(itHouse, rand() % houses.size() );
    blankHouse = *itHouse;
    _d->destination = blankHouse->getTilePos();
  }

  return blankHouse;
}

void Immigrant::_checkPath( Tile& startPoint, Building* house )
{
  PathWay pathWay;

  Tilemap& citymap = _d->city->getTilemap();
  Tile& destTile = house ? house->getTile() : citymap.at( _d->city->getRoadExitIJ() );
  Size arrivedArea( house ? house->getSize() : 1 );

  bool pathFound = Pathfinder::getInstance().getPath( startPoint.getIJ(), destTile.getIJ(), pathWay, 
                                                      false, arrivedArea );
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
        int saveHbCount = house->getNbHabitants();
        house->addHabitants( _getPeoplesCount() );
        _d->peopleCount -= ( house->getNbHabitants() - saveHbCount ); 
        Walker::onDestination();

        gooutCity = (_d->peopleCount > 0);
      }
    }
  }

  if( gooutCity )
  {
    House* blankHouse = _findBlankHouse();
    _checkPath( _d->city->getTilemap().at( getIJ() ), blankHouse );
  }
}

Immigrant* Immigrant::create( City& city, const Building& startPoint,
                              const unsigned char peoples )
{
  Immigrant* newImmigrant = new Immigrant( city, peoples );
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

void Immigrant::_setPeoplesCount( const unsigned char num )
{
  _d->peopleCount = num;
}

unsigned char Immigrant::_getPeoplesCount() const
{
  return _d->peopleCount;
}