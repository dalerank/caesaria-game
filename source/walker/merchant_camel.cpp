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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "merchant_camel.hpp"
#include "core/position.hpp"
#include "merchant_land.hpp"
#include "city/city.hpp"
#include "walkers_factory.hpp"
#include "city/statistic.hpp"
#include "objects/construction.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/logger.hpp"

REGISTER_CLASS_IN_WALKERFACTORY(walker::merchantCamel, MerchantCamel)

class MerchantCamel::Impl
{
public:
  bool inCaravan;
  TilePos headLocation;
  Walker::UniqueId headId;
};

void MerchantCamel::timeStep(const unsigned long time)
{
  if( !_d->inCaravan )
  {
    LandMerchantPtr head = _city()->statistic().walkers.find<LandMerchant>( walker::merchant, _d->headId );
    if( !head.isValid() )
    {
      die();
      Logger::warning( "!!!Warning: MerchantCamel havenot headID caravan {0}", _d->headId );
    }
    else
    {
      head->addCamel( this );
      _d->inCaravan = true;
    }
  }

  if( !gfx::tilemap::isValidLocation( _d->headLocation ) )
  {
    return;
  }

  if( _d->headLocation.distanceFrom( pos() ) < 1 * (uniqueId() - _d->headId) )
  {
    return;
  }

  Human::timeStep( time );
}

void MerchantCamel::save(VariantMap &stream) const
{
  Human::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, headId )
}

void MerchantCamel::load(const VariantMap &stream)
{
  Human::load( stream );
  VARIANT_LOAD_ANY_D( _d, headId, stream )

  if( _d->headId == 0  )
  {
    Logger::warning( "WARNING !!! MerchantCamel can't have headID." );
    deleteLater();
  }
}

void MerchantCamel::updateHeadLocation(const TilePos &pos)
{
  _d->headLocation = pos;
}

void MerchantCamel::_centerTile()
{
  Human::_centerTile();

  if( pos().distanceFrom( _d->headLocation ) > 1 )
  {
    MerchantPtr merchant = _city()->statistic().walkers.find<Merchant>( walker::merchant, _d->headId );
    if( merchant.isValid() )
    {
      setSpeed( merchant->speed() );
      setSpeedMultiplier( 1.5f );
    }
  }
  else
  {
    setSpeedMultiplier( 1.f );
  }
}

MerchantCamel::MerchantCamel(PlayerCityPtr city, MerchantPtr merchant, int delay)
  : Human( city, walker::merchantCamel ), _d( new Impl )
{
  _d->headId = 0;
  _d->inCaravan = false;
  _d->headLocation = gfx::tilemap::invalidLocation();

  if( merchant.isValid() )
  {
    _d->headId = merchant->uniqueId();
    setPos( merchant->pos() );
    setPathway( merchant->pathway() );
    go();
    wait( delay );
  }
}
