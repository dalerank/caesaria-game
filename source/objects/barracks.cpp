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
#include "city/city.hpp"
#include "walker/cart_supplier.hpp"

using namespace constants;

class Barracks::Impl
{
public:
  SimpleGoodStore store;
};

Barracks::Barracks() : TrainingBuilding( building::barracks, Size( 3 ) ),
  _d( new Impl )
{
  setMaxWorkers(5);
  setPicture( ResourceGroup::security, 17 );

  _d->store.setCapacity( 1000 );
  _d->store.setCapacity( Good::weapon, 1000 );
}

void Barracks::deliverTrainee()
{ 
  if( getWalkers().size() == 0 && _d->store.freeQty() > 0 )
  {
    CartSupplierPtr walker = CartSupplier::create( _getCity() );
    walker->send2city( this, Good::weapon, 100 );

    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
  }

  if( _d->store.qty( Good::weapon ) >= 100 )
  {
    TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::soldier );
    trainee->send2City( this, false );

    if( !trainee->isDeleted() )
    {
      GoodStock delStock( Good::weapon, 100 );
      _d->store.retrieve( delStock, 100 );
      addWalker( trainee.object() );
    }
  }
}

void Barracks::timeStep(const unsigned long time)
{
  TrainingBuilding::timeStep( time );

  if( numberWorkers() > 0 )
  {
    if( _animationRef().isStopped() )
    {
      _animationRef().start();
    }
  }
  else if( _animationRef().isRunning() )
  {
    _animationRef().stop();
  }
}

bool Barracks::isNeedWeapons() const
{
  return _d->store.freeQty() >= 100;
}

void Barracks::storeGoods(GoodStock& stock, const int amount)
{
  _d->store.store(stock, amount == -1 ? stock.qty() : amount );
}

void Barracks::save(VariantMap& stream) const
{
  TrainingBuilding::save( stream );

  stream[ "store" ] = _d->store.save();
}

void Barracks::load(const VariantMap& stream)
{
  TrainingBuilding::load( stream );

  _d->store.load( stream.get( "store" ).toMap() );
}
