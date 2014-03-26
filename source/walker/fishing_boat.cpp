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
#include "game/gamedate.hpp"

using namespace constants;

class FishingBoat::Impl
{
public:  
  CoastalFactoryPtr base;
  int updateInterval;
  TilePos destination;
  GoodStock stock;
  FishingBoat::State mode;

  Pathway findFishingPlace(PlayerCityPtr city, TilePos pos);
};

void FishingBoat::save( VariantMap& stream ) const
{
  Ship::save( stream );

  stream[ "destination" ] = _d->destination;
  stream[ "stock" ] = _d->stock.save();
  stream[ "mode" ] = (int)_d->mode;
  stream[ "base" ] = _d->base.isValid() ? _d->base->pos() : TilePos( -1, -1 );
}

void FishingBoat::load( const VariantMap& stream )
{
  Ship::load( stream );
  _d->destination = stream.get( "destination" );
  _d->stock.load( stream.get( "stock" ).toList() );
  _d->mode = (State)stream.get( "mode", (int)wait ).toInt();

  _d->base = ptr_cast<CoastalFactory>(_getCity()->getOverlay( (TilePos)stream.get( "base" ) ) );
  if( _d->base.isValid() )
  {
    _d->base->assignBoat( this );
  }
}

void FishingBoat::timeStep(const unsigned long time)
{
  Ship::timeStep( time );

  if( time % _d->updateInterval == 1 )
  {
    switch( _d->mode )
    {
    case ready2Catch:
    {
      _animationRef().clear();
      _setAnimation( gfx::fishingBoatMove );
      Pathway way = _d->findFishingPlace( _getCity(), pos() );
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
      _animationRef().clear();
      _setAnimation( gfx::fishingBoatWork );

      city::Helper helper( _getCity() );
      FishPlaceList places = helper.find<FishPlace>( walker::fishPlace, pos() );

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
        Pathway way = Pathfinder::getInstance().getPath( pos(), _d->base->getLandingTile().pos(),
                                                         Pathfinder::waterOnly );

        if( way.isValid() )
        {
          _d->mode = back2base;
          setPathway( way );
          go();
        }

        _animationRef().clear();
        _setAnimation( gfx::fishingBoatMove );
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
int FishingBoat::getFishQty() const{  return _d->stock.qty(); }

void FishingBoat::die()
{
  _d->mode = wait;
  _d->base = 0;
  _d->updateInterval = GameDate::ticksInMonth() / 20;
  _animationRef().load( ResourceGroup::carts, 265, 8 );
  _animationRef().setDelay( 4 );

  Ship::die();
}

FishingBoat::FishingBoat( PlayerCityPtr city ) : Ship( city ), _d( new Impl )
{
  _setAnimation( gfx::fishingBoatMove );
  _setType( walker::fishingBoat );
  setName( _("##fishing_boat##") );
  _d->mode = wait;
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
  case go2fishplace: _d->mode = catchFish; break;
  case back2base: _d->mode = unloadFish; break;
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
  city::Helper helper( city );
  FishPlaceList places = helper.find<FishPlace>( walker::fishPlace, city::Helper::invalidPos );

  int minDistance = 999;
  FishPlacePtr nearest;
  foreach( it, places )
  {
    FishPlacePtr place = *it;
    int currentDistance = pos.distanceFrom( place->pos() );
    if( currentDistance < minDistance )
    {
      minDistance = currentDistance;
      nearest = place;
    }
  }

  if( nearest != 0 )
  {
    Pathway way = Pathfinder::getInstance().getPath( pos, nearest->pos(),
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
    setPos( start );
    _getCity()->addWalker( this );
  }
}
