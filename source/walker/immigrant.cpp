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

#include "immigrant.hpp"
#include "core/position.hpp"
#include "core/safetycast.hpp"
#include "game/astarpathfinding.hpp"
#include "building/house.hpp"
#include "gfx/tile.hpp"
#include "core/variant.hpp"
#include "game/city.hpp"
#include "game/path_finding.hpp"
#include "game/tilemap.hpp"
#include "game/name_generator.hpp"
#include "building/constants.hpp"
#include "game/resourcegroup.hpp"
#include "corpse.hpp"

using namespace constants;

class Immigrant::Impl
{
public:
  TilePos destination;
  Picture cartPicture;
  CitizenGroup peoples;
  int stamina;
};

Immigrant::Immigrant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::immigrant );
  _setAnimation( gfx::homeless );

  setName( NameGenerator::rand( NameGenerator::male ) );
  _d->stamina = rand() % 80 + 20;
}

HousePtr Immigrant::_findBlankHouse()
{
  CityHelper hlp( _getCity() );
  HouseList houses = hlp.find< House >( building::house );
  HousePtr blankHouse;
  _d->destination = TilePos( -1, -1 );

  HouseList::iterator itHouse = houses.begin();
  while( itHouse != houses.end() )
  {
    if( (*itHouse)->getAccessRoads().size() > 0 && 
        ( (*itHouse)->getHabitants().count() < (*itHouse)->getMaxHabitants() ) )
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

  Pathway pathWay;

  Tilemap& citymap = _getCity()->getTilemap();
  Tile& destTile = house.isValid() ? house->getTile() : citymap.at( _getCity()->getBorderInfo().roadExit );
  Size arrivedArea( house.isValid() ? house->getSize() : 1 );

  bool pathFound = Pathfinder::getInstance().getPath( startPoint.getIJ(), destTile.getIJ(), pathWay, 
                                                      false, arrivedArea );
  if( pathFound )
  {
     setIJ( startPoint.getIJ() );
     setPathway( pathWay );
     go();
  }

  if( !pathFound )
  {
    deleteLater();
  }
}

void Immigrant::_reachedPathway()
{  
  bool gooutCity = true;
  if( _d->destination.getI() > 0 && _d->destination.getJ() > 0 )  //have destination
  {
    const Tile& tile = _getCity()->getTilemap().at( _d->destination );

    HousePtr house = tile.getOverlay().as<House>();
    if( house.isValid() )
    {      
      int freeRoom = house->getMaxHabitants() - house->getHabitants().count();
      if( freeRoom > 0 )
      {
        house->addHabitants( _d->peoples );
        Walker::_reachedPathway();

        gooutCity = (_d->peoples.count() > 0);
      }
    }
  }

  if( gooutCity )
  {
    _findPath2blankHouse( _getCity()->getTilemap().at( getIJ() ) );
  }
  else
  {
    deleteLater();
  }
}

void Immigrant::_brokePathway(TilePos pos)
{
  TileOverlayPtr overlay = _getCity()->getTilemap().at( pos ).getOverlay();
  if( !overlay.is<House>() )
  {
    _d->destination = getIJ();
    _reachedPathway();
  }
}

ImmigrantPtr Immigrant::create(PlayerCityPtr city )
{
  ImmigrantPtr newImmigrant( new Immigrant( city ) );
  newImmigrant->drop(); //delete automatically
  return newImmigrant;
}

bool Immigrant::send2City( PlayerCityPtr city, const CitizenGroup& peoples, Tile& startTile )
{
  if( peoples.count() > 0 )
  {
    ImmigrantPtr im = Immigrant::create( city );
    im->setPeoples( peoples );
    im->send2City( startTile );
    return true;
  }

  return false;
}

void Immigrant::send2City( Tile& startTile )
{
  _findPath2blankHouse( startTile );
  _getCity()->addWalker( this );
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

const CitizenGroup& Immigrant::_getPeoples() const
{
  return _d->peoples;
}

void Immigrant::setPeoples( const CitizenGroup& peoples )
{
  _d->peoples = peoples;
}

void Immigrant::timeStep(const unsigned long time)
{
  Walker::timeStep( time );

  switch( getAction() )
  {
  case Walker::acMove:
    _d->stamina = math::clamp( _d->stamina-1, 0, 100 );
    if( _d->stamina == 0 )
    {
      _setAnimation( gfx::homelessSit );
      _setAction( Walker::acNone );
      _getAnimation().clear();
    }
  break;

  case Walker::acNone:
    _d->stamina = math::clamp( _d->stamina+5, 0, 100 );
    if( _d->stamina >= 100 )
    {
      _setAnimation( gfx::homeless );
      _setAction( Walker::acMove );
      _getAnimation().clear();
    }
  break;

  default:
  break;
  }
}

void Immigrant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "peoples" ] = _d->peoples.save();
  stream[ "destination" ] = _d->destination;
}

void Immigrant::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->peoples.load( stream.get( "peoples" ).toList() );
  _d->destination = TilePos( stream.get( "destination" ).toTilePos() );
}

void Immigrant::die()
{
  Walker::die();

  Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen2, 1007, 1014 );
}
