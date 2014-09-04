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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

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
using namespace gfx;

namespace  {
CAESARIA_LITERALCONST(peoples)
CAESARIA_LITERALCONST(stamina)
}

class Emigrant::Impl
{
public:
  Picture cartPicture;
  CitizenGroup peoples;
  int failedWayCount;
  float stamina;

  void mayWalk( const Tile* tile, bool& ret )
  {
    HousePtr f = ptr_cast<House>( tile->overlay() );
    ret = ( tile->isWalkable( true ) || f.isValid() );
  }
};

Emigrant::Emigrant(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::emigrant );

  setName( NameGenerator::rand( NameGenerator::male ) );
  _d->stamina = math::random( 80 ) + 20;
  _d->failedWayCount = 0;
}

HousePtr Emigrant::_findBlankHouse()
{
  city::Helper hlp( _city() );

  HousePtr blankHouse;

  TilePos offset( 5, 5 );
  HouseList houses = hlp.find<House>( building::house, pos() - offset, pos() + offset );
  _checkAvailabelHouses( houses );
  if( houses.empty() )
  {
    houses = hlp.find<House>( building::house );
    _checkAvailabelHouses( houses );
  }


  if( houses.size() > 0 )
  {
    blankHouse = houses.random();
  }

  return blankHouse;
}

Pathway Emigrant::_findSomeWay( TilePos startPoint )
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

  return pathway;
}

void Emigrant::_reachedPathway()
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
    _append2house( house );
    gooutCity = (_d->peoples.count() > 0);
  }
  else
  {
    if( _checkNearestHouse() )
    {
      return;
    }
  }

  if( gooutCity )
  {
    Pathway way = _findSomeWay( pos() );
    if( way.isValid() )
    {
      _updatePathway( way );
      go();
    }
    else
    {
      die();
    }
  }
  else
  {
    deleteLater();
  }
}

void Emigrant::_append2house( HousePtr house )
{
  int freeRoom = house->maxHabitants() - house->habitants().count();
  if( freeRoom > 0 )
  {
    house->addHabitants( _d->peoples );
  }
}

bool Emigrant::_checkNearestHouse()
{
  city::Helper helper( _city() );
  TilePos offset( 2, 2 );
  HouseList houses = helper.find<House>( building::house, pos()-offset, pos() + offset );
  foreach( it, houses )  //have destination
  {
    HousePtr house = *it;

    int freeRoom = house->maxHabitants() - house->habitants().count();
    if( freeRoom > 0 )
    {
      Pathway pathway = PathwayHelper::create( pos(), house->pos(), makeDelegate( _d.data(), &Impl::mayWalk ) );

      _updatePathway( pathway );
      go();
      return true;
    }
  }

  return false;
}

void Emigrant::_brokePathway(TilePos p)
{
  _reachedPathway();

  if( isDeleted() )
    return;

  Pathway way = _findSomeWay( pos() );
  if( way.isValid() )
  {
    setPathway( way );
    go();
  }
  else
  {
    die();
  }
}

void Emigrant::_noWay()
{
  Pathway someway = PathwayHelper::randomWay( _city(), pos(), 5 );
  if( !someway.isValid() )
  {
    _d->failedWayCount++;
    if( _d->failedWayCount > 10 )
    {
      die();
    }
  }
  else
  {
    _d->failedWayCount = 0;
    setPathway( someway );
    go();
    }
}

void Emigrant::_checkAvailabelHouses(HouseList &hlist)
{
  HouseList::iterator itHouse = hlist.begin();
  bool bigcity = _city()->population() > 300;
  while( itHouse != hlist.end() )
  {
    HousePtr house = *itHouse;
    bool haveRoad = !house->getAccessRoads().empty();
    bool haveVacantRoom = (house->habitants().count() < house->maxHabitants());
    bool normalDesirability = true;
    if( bigcity )
    {
      normalDesirability = (house->tile().param( Tile::pDesirability ) > -10);
    }

    if( haveRoad && haveVacantRoom && normalDesirability) { ++itHouse; }
    else { itHouse = hlist.erase( itHouse ); }
  }
}

EmigrantPtr Emigrant::create(PlayerCityPtr city )
{
  EmigrantPtr newImmigrant( new Emigrant( city ) );
  newImmigrant->drop(); //delete automatically
  return newImmigrant;
}

EmigrantPtr Emigrant::send2city( PlayerCityPtr city, const CitizenGroup& peoples,
                                   const Tile& startTile, std::string thinks )
{
  if( peoples.count() > 0 )
  {
    EmigrantPtr im = Emigrant::create( city );
    im->setPeoples( peoples );
    im->send2city( startTile );
    im->setThinks( thinks );
    return im;
  }

  return EmigrantPtr();
}

bool Emigrant::send2city( const Tile& startTile )
{    
  setPos( startTile.pos() );
  Pathway way = _findSomeWay( startTile.pos() );

  if( way.isValid() )
  {
    setPathway( way );
    _city()->addWalker( this );
    return true;
  }
  else
  {
    deleteLater();
    return false;
  }
}

void Emigrant::leaveCity( const Tile& tile)
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


Emigrant::~Emigrant(){}

void Emigrant::_setCartPicture( const Picture& pic ){  _d->cartPicture = pic;}
const Picture& Emigrant::_cartPicture(){  return _d->cartPicture;}
const CitizenGroup& Emigrant::_getPeoples() const{  return _d->peoples;}
void Emigrant::setPeoples( const CitizenGroup& peoples ){  _d->peoples = peoples;}

void Emigrant::timeStep(const unsigned long time)
{
  Walker::timeStep( time );

  switch( action() )
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
      Pathway way = _findSomeWay( pos() );
      if( way.isValid() )
      {
        _updatePathway( way );
      }
      go();
    }
  break;

  default:
  break;
  }
}

void Emigrant::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ lc_peoples ] = _d->peoples.save();
  stream[ lc_stamina ] = _d->stamina;
}

void Emigrant::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->peoples.load( stream.get( lc_peoples ).toList() );
  _d->stamina = stream.get( lc_stamina );
}

bool Emigrant::die()
{
  bool created = Walker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen2, 1007, 1014 );
    return true;
  }

  return created;
}
