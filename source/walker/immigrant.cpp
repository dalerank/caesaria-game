// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "immigrant.hpp"
#include "core/position.hpp"
#include "core/safetycast.hpp"
#include "pathway/pathway_helper.hpp"
#include "objects/house.hpp"
#include "gfx/tile.hpp"
#include "core/variant.hpp"
#include "city/helper.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tilemap.hpp"
#include "name_generator.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "corpse.hpp"

using namespace constants;

class Immigrant::Impl
{
public:
  Picture cartPicture;
  CitizenGroup peoples;
  float stamina;
};

Immigrant::Immigrant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::immigrant );
  //_setAnimation( gfx::homelessMove );

  setName( NameGenerator::rand( NameGenerator::male ) );
  _d->stamina = rand() % 80 + 20;
}

HousePtr Immigrant::_findBlankHouse()
{
  city::Helper hlp( _city() );
  HouseList houses = hlp.find< House >( building::house );
  HousePtr blankHouse;

  HouseList::iterator itHouse = houses.begin();
  while( itHouse != houses.end() )
  {
    if( (*itHouse)->getAccessRoads().size() > 0 && 
        ( (*itHouse)->getHabitants().count() < (*itHouse)->getMaxHabitants() ) )
    {
      ++itHouse;
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
  }

  return blankHouse;
}

void Immigrant::_findPath2blankHouse( TilePos startPoint )
{
  HousePtr house = _findBlankHouse();  

  Pathway pathway;
  if( house.isValid() )
  {
    pathway = PathwayHelper::create( startPoint, ptr_cast<Construction>(house),
                                     PathwayHelper::roadFirst  );
  }

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::create( startPoint,
                                     _city()->borderInfo().roadExit,
                                     PathwayHelper::allTerrain );
  }

  if( pathway.isValid() )
  {
    setPos( startPoint );
    setPathway( pathway );
    go();
  }
  else
  {
    die();
  }
}

void Immigrant::_reachedPathway()
{
  bool gooutCity = true;
  Walker::_reachedPathway();

  if( pos() == _city()->borderInfo().roadExit )
  {
    deleteLater();
    return;
  }

  HousePtr house = ptr_cast<House>( _city()->getOverlay( pos() ) );
  if( house.isValid() )
  {
    int freeRoom = house->getMaxHabitants() - house->getHabitants().count();
    if( freeRoom > 0 )
    {
      house->addHabitants( _d->peoples );
      gooutCity = (_d->peoples.count() > 0);
    }
  }
  else
  {
    TilesArray area = _city()->tilemap().getArea( pos() - TilePos(1,1),
                                                         pos() + TilePos(1,1) );
    foreach( it, area )  //have destination
    {
      Tile* tile = *it;
      HousePtr house = ptr_cast<House>( tile->overlay() );
      if( !house.isValid() )
        continue;
      int freeRoom = house->getMaxHabitants() - house->getHabitants().count();
      if( freeRoom > 0 )
      {
        Tilemap& tmap = _city()->tilemap();
        Pathway pathway;
        pathway.init( tmap, tmap.at( pos() ) );
        pathway.setNextTile( *tile );

        gooutCity = false;
        _updatePathway( pathway );
        go();
        return;
      }
    }
  }

  if( gooutCity )
  {
    _findPath2blankHouse( pos() );
  }
  else
  {
    deleteLater();
  }
}

void Immigrant::_brokePathway(TilePos pos)
{
  /*TileOverlayPtr overlay = _getCity()->getTilemap().at( pos ).getOverlay();
  if( !overlay.is<House>() )
  {
    _d->destination = getIJ();
    _reachedPathway();
  }*/
}

ImmigrantPtr Immigrant::create(PlayerCityPtr city )
{
  ImmigrantPtr newImmigrant( new Immigrant( city ) );
  newImmigrant->drop(); //delete automatically
  return newImmigrant;
}

ImmigrantPtr Immigrant::send2city( PlayerCityPtr city, const CitizenGroup& peoples,
                                   const Tile& startTile, std::string thinks )
{
  if( peoples.count() > 0 )
  {
    ImmigrantPtr im = Immigrant::create( city );
    im->setPeoples( peoples );
    im->send2city( startTile );
    im->setThinks( thinks );
    return im;
  }

  return ImmigrantPtr();
}

void Immigrant::send2city( const Tile& startTile )
{
  _findPath2blankHouse( startTile.pos() );
  _city()->addWalker( this );
}

void Immigrant::leaveCity( const Tile& tile)
{
  setPos( tile.pos() );
  Pathway pathway = PathwayHelper::create( tile.pos(),
                                           _city()->borderInfo().roadExit,
                                           PathwayHelper::allTerrain );

  if( !pathway.isValid() )
  {
    die();
    return;
  }

  _city()->addWalker( this );
  setPathway( pathway );
  go();
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
    _d->stamina = math::clamp( _d->stamina-1, 0.f, 100.f );
    if( _d->stamina == 0 )
    {
      _setAction( Walker::acNone );
    }
  break;

  case Walker::acNone:
    _d->stamina = math::clamp( _d->stamina+1, 0.f, 100.f );
    if( _d->stamina >= 100 )
    {
      _setAction( Walker::acMove );
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
  stream[ "stamina" ] = _d->stamina;
}

void Immigrant::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->peoples.load( stream.get( "peoples" ).toList() );
  _d->stamina = stream.get( "stamina" );
}

void Immigrant::die()
{
  Walker::die();

  Corpse::create( _city(), pos(), ResourceGroup::citizen2, 1007, 1014 );
}
