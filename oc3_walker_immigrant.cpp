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

#include "oc3_walker_immigrant.hpp"
#include "oc3_positioni.hpp"
#include "oc3_scenario.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_house.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"
#include "oc3_city.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_name_generator.hpp"

class Immigrant::Impl
{
public:
  TilePos destination;
  Picture cartPicture;
  unsigned char peopleCount;
  CityPtr city;
};

Immigrant::Immigrant( CityPtr city ) : _d( new Impl )
{
  _setType( WT_IMMIGRANT );
  _setGraphic( WG_HOMELESS );
  _d->peopleCount = 0;
  _d->city = city;

  setName( NameGenerator::rand( NameGenerator::male ) );
}

HousePtr Immigrant::_findBlankHouse()
{
  CityHelper hlp( _d->city );
  std::list< HousePtr > houses = hlp.getBuildings< House >( B_HOUSE );
  HousePtr blankHouse;
  _d->destination = TilePos( -1, -1 );

  std::list< HousePtr >::iterator itHouse = houses.begin();
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

void Immigrant::_findPath2blankHouse( Tile& startPoint )
{
  HousePtr house = _findBlankHouse();

  PathWay pathWay;

  Tilemap& citymap = _d->city->getTilemap();
  Tile& destTile = house.isValid() ? house->getTile() : citymap.at( _d->city->getRoadExit() );
  Size arrivedArea( house.isValid() ? house->getSize() : 1 );

  bool pathFound = Pathfinder::getInstance().getPath( startPoint.getIJ(), destTile.getIJ(), pathWay, 
                                                      false, arrivedArea );
  if( pathFound )
  {
     setPathWay( pathWay );
     setIJ( startPoint.getIJ() );
     _setAction( WA_MOVE );
  }

  if( !pathFound )
  {
    deleteLater();
  }
}

void Immigrant::onDestination()
{  
  bool gooutCity = true;
  if( _d->destination.getI() > 0 && _d->destination.getJ() > 0 )  //have destination
  {
    const Tile& tile = _d->city->getTilemap().at( _d->destination );

    HousePtr house = tile.getTerrain().getOverlay().as<House>();
    if( house.isValid() )
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
    _findPath2blankHouse( _d->city->getTilemap().at( getIJ() ) );
  }
  else
  {
    deleteLater();
  }
}

ImmigrantPtr Immigrant::create( CityPtr city )
{
  ImmigrantPtr newImmigrant( new Immigrant( city ) );
  newImmigrant->drop(); //delete automatically
  return newImmigrant;
}

void Immigrant::send2City( Tile& startTile )
{
  _findPath2blankHouse( startTile );
  _d->city->addWalker( WalkerPtr( this ) );
}

Immigrant::~Immigrant()
{

}

void Immigrant::setCartPicture( const Picture& pic )
{
  _d->cartPicture = pic;
}

const Picture& Immigrant::getCartPicture()
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

void Immigrant::setCapacity( int value )
{
  _d->peopleCount = value;
} 

void Immigrant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "peopleCount" ] = _d->peopleCount;
  stream[ "destination" ] = _d->destination;
}

void Immigrant::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->peopleCount = stream.get( "peopleCount" ).toInt();
  _d->destination = TilePos( stream.get( "destination" ).toTilePos() );
}
