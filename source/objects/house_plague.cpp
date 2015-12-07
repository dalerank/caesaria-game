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

#include "house_plague.hpp"
#include "core/variant_map.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "walker/walkers_factory.hpp"
#include "house.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::house_plague,HousePlague)

class HousePlague::Impl
{
public:
  int counter;
  int time;
  Animation anim;
};

void HousePlague::create(PlayerCityPtr city, const TilePos& pos, int time)
{
  auto hpllist = city->walkers( pos ).select<HousePlague>();

  if( !hpllist.empty() )
  {
    hpllist.front()->_d->counter = 0;
    return;
  }

  HousePlague* hplague = new HousePlague( city );
  hplague->setPos( pos );
  hplague->_d->time = time;

  WalkerPtr ret( hplague );
  ret->drop();
  ret->attach();
}

HousePlague::HousePlague( PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::house_plague );

  _d->time = 0;
  _d->counter = 0;

  setName( _("##house_plague##") );
  _setHealth( 0 );

  setFlag( vividly, false );
  _d->anim.load( "illhouse", 1, 20 );
  _d->anim.setLoop( true );
  _d->anim.setDelay( 2 );
}

HousePlague::~HousePlague(){}

void HousePlague::timeStep(const unsigned long time)
{
  _d->counter++;

  _d->anim.update( time );

  if( game::Date::isDayChanged() )
  {
    auto house = _map().overlay<House>( pos() );
    int health_value = house.isValid() ? house->state( pr::health ) : 100;
    if( health_value > 20 )
    {
      deleteLater();
    }   
  }

  if( _d->counter > _d->time )
  {
    deleteLater();
  }
}

void HousePlague::save( VariantMap& stream ) const
{
  Walker::save( stream );

  VARIANT_SAVE_ANY_D( stream, _d, time )
  VARIANT_SAVE_ANY_D( stream, _d, counter )
}

void HousePlague::load( const VariantMap& stream )
{
  Walker::load( stream );

  VARIANT_LOAD_ANY_D( _d, time, stream )
  VARIANT_LOAD_ANY_D( _d, counter, stream )
}

const Picture& HousePlague::getMainPicture()
{
  return _d->anim.currentFrame();
}
