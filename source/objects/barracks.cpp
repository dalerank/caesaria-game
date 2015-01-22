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

#include "barracks.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "walker/trainee.hpp"
#include "good/goodstore_simple.hpp"
#include "core/variant_map.hpp"
#include "city/city.hpp"
#include "walker/cart_supplier.hpp"
#include "objects_factory.hpp"

using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::barracks, Barracks)

class Barracks::Impl
{
public:
  good::SimpleStore store;
  bool notNeedSoldiers;
};

Barracks::Barracks() : TrainingBuilding( objects::barracks, Size( 3 ) ),
  _d( new Impl )
{
  setMaximumWorkers(5);
  setPicture( ResourceGroup::security, 17 );

  _d->store.setCapacity( 1000 );
  _d->store.setCapacity( good::weapon, 1000 );
  _d->notNeedSoldiers = false;
}

void Barracks::deliverTrainee()
{ 
  if( walkers().size() == 0 && _d->store.freeQty() > 0 )
  {
    CartSupplierPtr walker = CartSupplier::create( _city() );
    walker->send2city( this, good::weapon, 100 );

    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
  }

  if( _d->store.qty( good::weapon ) >= 100 )
  {
    TraineeWalkerPtr trainee = TraineeWalker::create( _city(), walker::soldier );
    trainee->send2City( this, false );

    if( !trainee->isDeleted() )
    {
      good::Stock delStock( good::weapon, 100 );
      _d->store.retrieve( delStock, 100 );
      addWalker( trainee.object() );
      _d->notNeedSoldiers = false;
    }
    else
    {
      _d->notNeedSoldiers = true;
    }
  }
}

void Barracks::timeStep(const unsigned long time)
{
  TrainingBuilding::timeStep( time );
}

bool Barracks::isNeedWeapons() const {  return _d->store.freeQty() >= 100; }
int Barracks::goodQty(good::Product type) const{  return _d->store.qty( type ); }

void Barracks::storeGoods(good::Stock& stock, const int amount)
{
  _d->store.store(stock, amount == -1 ? stock.qty() : amount );
}

std::string Barracks::workersProblemDesc() const
{
  unsigned int pp = productivity();
  unsigned int haveWeapon = _d->store.qty() >= _d->store.capacity() / 2;
  if( pp > 0  )
  {
    if( _d->notNeedSoldiers )
      return "##barracks_city_not_need_soldiers##";

    if( _d->store.empty() )
      return "##barracks_no_weapons##";

    if( pp < 25 )
    {
      return ( haveWeapon )
                ? "##barracks_have_weapons_bad_workers##"
                : "##barracks_bad_weapons_bad_workers##";
    }
    else if( pp < 50 )
    {
      return ( haveWeapon )
                ? "##barracks_have_weapons_slow_workers##"
                : "##barracks_bad_weapons_slow_workers##";
    }
    else if( pp < 75 )
    {
      return ( haveWeapon )
                ? "##barracks_have_weapons_patrly_workers##"
                : "##barracks_bad_weapons_patrly_workers##";
    }
    else if( pp < 90 )
    {
      return ( haveWeapon )
                ? "##barracks_have_weapons_need_some_workers##"
                : "##barracks_bad_weapons_need_some_workers##";
    }
  }

  return WorkingBuilding::workersProblemDesc();
}

void Barracks::save(VariantMap& stream) const
{
  TrainingBuilding::save( stream );

  stream[ "store" ] = _d->store.save();
  VARIANT_SAVE_ANY_D( stream, _d, notNeedSoldiers );
}

void Barracks::load(const VariantMap& stream)
{
  TrainingBuilding::load( stream );

  _d->store.load( stream.get( "store" ).toMap() );
  VARIANT_LOAD_ANY_D( _d, notNeedSoldiers, stream );
}
