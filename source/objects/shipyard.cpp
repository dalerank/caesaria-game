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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "shipyard.hpp"

#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"
#include "good/goodstore.hpp"
#include "wharf.hpp"
#include "pathway/pathway.hpp"
#include "city/helper.hpp"
#include "walker/fishing_boat.hpp"
#include "objects_factory.hpp"

using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::shipyard, Shipyard)

class Shipyard::Impl
{
public:
  enum { northPic=1, eastPic=2, southPic=3, westPic=4 };

  bool isNeedCreateBoat(PlayerCityPtr city);
  bool creatingBoat;

  FishingBoatPtr boat;

  WharfPtr findFreeWharf( PlayerCityPtr city );
};

Shipyard::Shipyard() : CoastalFactory(good::timber, good::none, objects::shipyard, Size(2)),
  _d( new Impl )
{
  // transport 1 2 3 4
  setPicture( ResourceGroup::shipyard, Impl::northPic );

  inStockRef().setCapacity( 1200 );
  store().setCapacity( 1200 );
  _d->creatingBoat = false;
}

void Shipyard::destroy()
{
  CoastalFactory::destroy();

  if( _d->boat.isValid() )
  {
    _d->boat->die();
  }
}

void Shipyard::timeStep(const unsigned long time)
{
  //try get good from storage building for us
  if( game::Date::isWeekChanged() )
  {
    if( numberWorkers() > 0 && walkers().size() == 0 )
    {
      receiveGood();
    }

    //try send boat to catch fish
    if( _d->boat.isValid() )
    {
      WharfPtr wharf = _d->findFreeWharf( _city() );

      if( wharf.isValid() )
      {
        wharf->assignBoat( _d->boat.object() );
        _d->boat->return2base();
        _d->boat = FishingBoatPtr();
      }
    }

    if( !_d->isNeedCreateBoat( _city() ) )
      return;
  }

  CoastalFactory::timeStep(time);

  if( progress() >= 100.0 )
  {
    if( store().qty( produceGoodType() ) < store().capacity( produceGoodType() )
        && _d->boat.isNull() )
    {
      updateProgress( -100.f );

      _d->boat = FishingBoat::create( _city() );
      _d->boat->send2city( this, landingTile().pos() );
    }
  }
}

bool Shipyard::mayWork() const
{
  bool factoryMayWork = CoastalFactory::mayWork();
  return (_d->boat.isNull() && factoryMayWork);
}

unsigned int Shipyard::getConsumeQty() const {  return 1000; }

std::string Shipyard::workersProblemDesc() const
{
  std::string ret = CoastalFactory::workersProblemDesc();

  if( ret.empty() && !_d->creatingBoat )
  {
    ret = "##shipyard_notneed_ours_boat##";
  }

  return ret;
}

void Shipyard::_updatePicture(Direction direction)
{
  switch( direction )
  {
  case south: setPicture( ResourceGroup::shipyard, Impl::southPic ); break;
  case north: setPicture( ResourceGroup::shipyard, Impl::northPic ); break;
  case west: setPicture( ResourceGroup::shipyard, Impl::westPic ); break;
  case east: setPicture( ResourceGroup::shipyard, Impl::eastPic ); break;

  default: break;
  }
}

bool Shipyard::Impl::isNeedCreateBoat(PlayerCityPtr city )
{
  if( creatingBoat )
    return true;

  WharfPtr wharf = findFreeWharf( city );
  return wharf.isNull();
}

WharfPtr Shipyard::Impl::findFreeWharf( PlayerCityPtr city )
{
  city::Helper helper( city );

  WharfList wharfs = helper.find<Wharf>( objects::wharf );
  foreach( wharf, wharfs )
  {
    if( (*wharf)->getBoat().isNull() )
      return *wharf;
  }

  return WharfPtr();
}
