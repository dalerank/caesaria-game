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

#include "fishing_boat.hpp"
#include "core/gettext.hpp"
#include "city/statistic.hpp"
#include "objects/wharf.hpp"
#include "good/stock.hpp"
#include "walker/fish_place.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/utils.hpp"
#include "pathway/pathway.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "objects/predefinitions.hpp"
#include "walker/fish_place.hpp"
#include "gfx/tilesarray.hpp"
#include "game/gamedate.hpp"
#include "gfx/tilemap.hpp"
#include "walkers_factory.hpp"
#include "gfx/helper.hpp"

REGISTER_CLASS_IN_WALKERFACTORY(walker::fishingBoat, FishingBoat)

class FishingBoat::Impl
{
public:  
  CoastalFactoryPtr base;
  DateTime dateUpdate;
  TilePos destination;
  good::Stock stock;
  FishingBoat::State mode;

  Pathway findFishingPlace(PlayerCityPtr city, TilePos pos);
};

void FishingBoat::save( VariantMap& stream ) const
{
  Ship::save( stream );

  VARIANT_SAVE_ANY_D( stream, _d, destination )
  VARIANT_SAVE_CLASS_D( stream, _d, stock )
  stream[ "mode" ] = (int)_d->mode;
  stream[ "base" ] = _d->base.isValid() ? _d->base->pos() : gfx::tilemap::invalidLocation();
}

void FishingBoat::load( const VariantMap& stream )
{
  Ship::load( stream );
  VARIANT_LOAD_ANY_D( _d, destination, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, stock, stream )
  _d->mode = (State)stream.get( "mode", (int)wait ).toInt();

  TilePos basepos = stream.get( "base" );
  _d->base = _map().overlay( basepos ).as<CoastalFactory>();
  if( _d->base.isValid() )
  {
    _d->base->assignBoat( this );
  }
}

void FishingBoat::timeStep(const unsigned long time)
{
  Ship::timeStep( time );

  if( _d->dateUpdate.daysTo( game::Date::current() ) > 0 )
  {
    _d->dateUpdate = game::Date::current();

    switch( _d->mode )
    {
    case ready2Catch:
    {
      _animation().clear();
      Pathway way = _d->findFishingPlace( _city(), pos() );
      if( way.isValid() )
      {
        setPathway( way );
        go();

        _d->mode = go2fishplace;
      }
    }
    break;

    case catchFish:
    {
      _animation().clear();
      _setAction( acWork );

      FishPlaceList places = _city()->statistic().walkers.find<FishPlace>( walker::fishPlace, pos() );

      if( !places.empty() )
      {
        FishPlacePtr fishplace = places.front();
        _d->stock.setQty( math::clamp( _d->stock.qty()+10, 0, _d->stock.capacity() ) );
      }
      else
      {
        _d->mode = ready2Catch;
      }

      if( _d->stock.qty() == _d->stock.capacity() )
      {
        _d->mode = finishCatch;
      }
    }
    break;

    case finishCatch:
    {
      if( _d->base != 0 )
      {
        Pathway way = PathwayHelper::create( pos(), _d->base->landingTile().pos(),
                                             PathwayHelper::deepWater );

        if( way.isValid() )
        {
          _d->mode = back2base;
          setPathway( way );
          go();
        }

        _setAction( acMove );
      }
      else
      {
        Logger::warning( "Broken fishing boat" );
        deleteLater();
      }
    }
    break;

    case go2fishplace: break;
    case back2base: break;
    case wait: break;

    case unloadFish:
      if( _d->stock.qty() > 0 && _d->base != 0 )
      {
        _d->stock.setQty( math::clamp( _d->stock.qty()-10, 0, 100 ) );
        _d->base->updateProgress( 10 );
      }

      if( _d->stock.qty() == 0 )
      {
        _d->mode = ready2Catch;
      }
    break;
    }
  }
}

void FishingBoat::startCatch() {  _d->mode = ready2Catch; }
void FishingBoat::return2base(){  _d->mode = finishCatch; }
void FishingBoat::setBase(CoastalFactoryPtr base){  _d->base = base;}
FishingBoat::State FishingBoat::state() const{  return _d->mode;}
bool FishingBoat::isBusy() const{  return _d->mode != wait; }
int FishingBoat::fishQty() const{  return _d->stock.qty(); }
int FishingBoat::fishMax() const{  return _d->stock.capacity(); }
void FishingBoat::addFish(int qty) { _d->stock.push(qty); }

bool FishingBoat::die()
{
  _d->mode = wait;
  _d->base = 0;
  _animation().load( ResourceGroup::carts, 265, 8 );
  _animation().setDelay( gfx::Animation::slow );

  bool created = Ship::die();
  return created;
}

FishingBoat::FishingBoat( PlayerCityPtr city ) : Ship( city ), _d( new Impl )
{
  _setType( walker::fishingBoat );
  setName( _("##fishing_boat##") );
  _d->mode = wait;
  _d->stock.setType( good::fish );
  _d->stock.setCapacity( 100 );
}

FishingBoatPtr FishingBoat::create(PlayerCityPtr city)
{
  FishingBoatPtr ret( new FishingBoat( city ) );
  ret->drop();

  return ret;
}

void FishingBoat::_reachedPathway()
{
  Walker::_reachedPathway();

  switch( _d->mode )
  {
  case go2fishplace: _d->mode = catchFish; break;
  case back2base: _d->mode = unloadFish; break;
  default: break;
  }
}

Pathway FishingBoat::Impl::findFishingPlace(PlayerCityPtr city, TilePos pos )
{
  FishPlaceList places = city->statistic().walkers.find<FishPlace>( walker::fishPlace, gfx::tilemap::invalidLocation() );

  int minDistance = 999;
  FishPlacePtr nearest;
  for( auto place : places )
  {
    int currentDistance = pos.distanceFrom( place->pos() );
    if( currentDistance < minDistance )
    {
      minDistance = currentDistance;
      nearest = place;
    }
  }

  if( nearest != 0 )
  {
    Pathway way = PathwayHelper::create( pos, nearest->pos(), PathwayHelper::deepWater );

    return way;
  }

  return Pathway();
}

void FishingBoat::send2city( CoastalFactoryPtr base, TilePos start )
{
  _d->base = base;
  setPos( start );
  attach();
}
