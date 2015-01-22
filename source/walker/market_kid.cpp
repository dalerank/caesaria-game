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

#include "market_kid.hpp"
#include "market_buyer.hpp"
#include "objects/market.hpp"
#include "city/helper.hpp"
#include "pathway/pathway.hpp"
#include "good/goodstore.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant_map.hpp"
#include "name_generator.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "thinks.hpp"
#include "walkers_factory.hpp"

using namespace constants;

REGISTER_CLASS_IN_WALKERFACTORY(walker::marketKid, MarketKid)

class MarketKid::Impl
{
public:
  good::Stock basket;
  TilePos marketPos;
  unsigned int delay;
  unsigned int birthTime;
};

MarketKidPtr MarketKid::create(PlayerCityPtr city )
{
  MarketKidPtr ret( new MarketKid( city ) );
  ret->drop();

  return ret;
}

MarketKidPtr MarketKid::create(PlayerCityPtr city, MarketBuyerPtr lady )
{
  MarketKidPtr ret( new MarketKid( city ) );
  ret->setPos( lady->pos() );
  ret->_pathwayRef() = lady->pathway();

  ret->drop();

  return ret;
}

MarketKid::MarketKid(PlayerCityPtr city )
  : Human( city ), _d( new Impl )
{
  _d->delay = 0;
  _d->birthTime = 0;
  _d->basket.setCapacity( 100 );
  _setType( walker::marketKid );

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void MarketKid::setDelay( int delay ) {  _d->delay = delay; }

void MarketKid::send2City( MarketPtr destination )
{
  if( destination.isValid() )
  {
    _d->marketPos = destination->pos();
    _pathwayRef().move( Pathway::reverse );
    _centerTile();
    _city()->addWalker( this );
  }
  else
  {
    deleteLater();
  }
}

void MarketKid::save(VariantMap& stream) const
{
  Walker::save( stream );
  stream[ "basket" ] = _d->basket.save();
  VARIANT_SAVE_ANY_D( stream, _d, delay );
  VARIANT_SAVE_ANY_D( stream, _d, marketPos );
  VARIANT_SAVE_ANY_D( stream, _d, birthTime );
}

void MarketKid::load(const VariantMap& stream)
{
  Walker::load( stream );
  _d->basket.load( stream.get( "basket" ).toList() );
  VARIANT_LOAD_ANY_D( _d, delay, stream );
  VARIANT_LOAD_ANY_D( _d, marketPos, stream );
  VARIANT_LOAD_ANY_D( _d, birthTime, stream );
}

void MarketKid::timeStep( const unsigned long time )
{
  if( _d->birthTime == 0 )
  {
    _d->birthTime = time;
  }

  if( time - _d->birthTime < _d->delay )
  {
    return;
  }

  Walker::timeStep( time );
}

void MarketKid::_reachedPathway()
{
  Walker::_reachedPathway();

  deleteLater();

  MarketPtr market;
  market << _city()->getOverlay( _d->marketPos );
  if( market.isValid() )
  {
    market->goodStore().store( _d->basket, _d->basket.qty() );
  }
}

bool MarketKid::die()
{
  bool created = Walker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::carts, 465, 472 );
    return true;
  }

  return created;
}

void MarketKid::_updateThoughts()
{
  StringArray ownThinks;
  ownThinks << "##market_kid_say_1##";
  ownThinks << "##market_kid_say_2##";
  ownThinks << "##market_kid_say_3##";

  setThinks( WalkerThinks::check( this, _city(), ownThinks ) );
}

good::Stock& MarketKid::getBasket(){  return _d->basket;}
