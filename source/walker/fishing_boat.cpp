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

#include "fishing_boat.hpp"
#include "core/gettext.hpp"
#include "city/helper.hpp"
#include "objects/wharf.hpp"
#include "good/good.hpp"
#include "walker/fish_place.hpp"
#include "pathway/astarpathfinding.hpp"
#include "core/stringhelper.hpp"
#include "pathway/pathway.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "objects/predefinitions.hpp"
#include "walker/fish_place.hpp"
#include "gfx/tilesarray.hpp"

using namespace constants;

class FishingBoat::Impl
{
public:
  typedef enum { go2fishplace, catchFish, back2base, finishCatch, unloadFish, ready2Catch, wait } Mode;
  CoastalFactoryPtr base;
  TilePos destination;
  GoodStock stock;
  Mode mode;

  Pathway findFishingPlace(PlayerCityPtr city, TilePos pos);
};

void FishingBoat::save( VariantMap& stream ) const
{
  Ship::save( stream );

  stream[ "destination" ] = _d->destination;
  stream[ "stock" ] = _d->stock.save();
  stream[ "mode" ] = (int)_d->mode;
  stream[ "base" ] = _d->base.isValid() ? _d->base->getTilePos() : TilePos( -1, -1 );
}

void FishingBoat::load( const VariantMap& stream )
{
  Ship::load( stream );
  _d->destination = stream.get( "destination" );
  _d->stock.load( stream.get( "stock" ).toList() );
  _d->mode = (Impl::Mode)stream.get( "mode", (int)Impl::wait ).toInt();

  _d->base = ptr_cast<CoastalFactory>(_getCity()->getOverlay( (TilePos)stream.get( "base" ) ) );
  if( _d->base.isValid() )
  {
    _d->base->assignBoat( this );
  }
}

void FishingBoat::timeStep(const unsigned long time)
{
  Ship::timeStep( time );

  if( time % 16 == 1 )
  {
    switch( _d->mode )
    {
    case Impl::ready2Catch:
    {
      _animationRef().clear();
      _setAnimation( gfx::fishingBoat );
      Pathway way = _d->findFishingPlace( _getCity(), getIJ() );
      if( way.isValid() )
      {
        setPathway( way );
        go();

        _d->mode = Impl::go2fishplace;
      }
    }
    break;

    case Impl::catchFish:
    {
      _animationRef().clear();
      _setAnimation( gfx::fishingBoatWork );

      CityHelper helper( _getCity() );
      FishPlaceList places = helper.find<FishPlace>( walker::fishPlace, getIJ() );

      if( !places.empty() )
      {
        FishPlacePtr fishplace = places.front();
        _d->stock.setQty( math::clamp( _d->stock.qty()+10, 0, _d->stock.capacity() ) );
      }
      else
      {
        _d->mode = Impl::ready2Catch;
      }

      if( _d->stock.qty() == _d->stock.capacity() )
      {
        _d->mode = Impl::finishCatch;
      }
    }
    break;

    case Impl::finishCatch:
    {
      if( _d->base != 0 )
      {
        Pathway way = Pathfinder::getInstance().getPath( getIJ(), _d->base->getLandingTile().getIJ(),
                                                         Pathfinder::waterOnly );

        if( way.isValid() )
        {
          _d->mode = Impl::back2base;
          setPathway( way );
          go();
        }

        _animationRef().clear();
        _setAnimation( gfx::fishingBoat );
      }
      else
      {
        Logger::warning( "Broken fishing boat" );
        deleteLater();
      }
    }
    break;

    case Impl::go2fishplace: break;
    case Impl::back2base: break;
    case Impl::wait: break;

    case Impl::unloadFish:
      if( _d->stock.qty() > 0 && _d->base != 0 )
      {
        _d->stock.setQty( math::clamp( _d->stock.qty()-10, 0, 100 ) );
        _d->base->updateProgress( 10 );
      }

      if( _d->stock.qty() == 0 )
      {
        _d->mode = Impl::ready2Catch;
      }
    break;
    }
  }
}

void FishingBoat::startCatch()
{
  _d->mode = Impl::ready2Catch;
}

void FishingBoat::back2base()
{
  _d->mode = Impl::finishCatch;
}

void FishingBoat::setBase(CoastalFactoryPtr base)
{
  _d->base = base;
}

bool FishingBoat::isBusy() const
{
  return _d->mode != Impl::wait;
}

void FishingBoat::die()
{
  _d->mode = Impl::wait;
  _d->base = 0;
  _animationRef().load( ResourceGroup::carts, 265, 8 );
  _animationRef().setDelay( 4 );

  Ship::die();
}

FishingBoat::FishingBoat( PlayerCityPtr city ) : Ship( city ), _d( new Impl )
{
  _setAnimation( gfx::fishingBoat );
  _setType( walker::fishingBoat );
  setName( _("##fishing_boat##") );
  _d->mode = Impl::wait;
  _d->stock.setType( Good::fish );
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
  case Impl::go2fishplace: _d->mode = Impl::catchFish; break;
  case Impl::back2base: _d->mode = Impl::unloadFish; break;
  default: break;
  }
}

void FishingBoat::_changeTile()
{
  Walker::_changeTile();
  _animationRef().setDelay( 3 );
}

Pathway FishingBoat::Impl::findFishingPlace(PlayerCityPtr city, TilePos pos )
{
  CityHelper helper( city );
  FishPlaceList places = helper.find<FishPlace>( walker::fishPlace, CityHelper::invalidPos );

  int minDistance = 999;
  FishPlacePtr nearest;
  foreach( it, places )
  {
    FishPlacePtr place = *it;
    int currentDistance = pos.distanceFrom( place->getIJ() );
    if( currentDistance < minDistance )
    {
      minDistance = currentDistance;
      nearest = place;
    }
  }

  if( nearest != 0 )
  {
    Pathway way = Pathfinder::getInstance().getPath( pos, nearest->getIJ(),
                                                     Pathfinder::waterOnly );

    return way;
  }

  return Pathway();
}

void FishingBoat::send2city( CoastalFactoryPtr base, TilePos start )
{
  _d->base = base;
  if( !isDeleted() )
  {
    setIJ( start );
    _getCity()->addWalker( this );
  }
}
