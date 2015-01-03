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

#include "locust.hpp"
#include "core/variant_map.hpp"
#include "city/helper.hpp"
#include "core/gettext.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "objects/farm.hpp"

using namespace constants;
using namespace gfx;

class Locust::Impl
{
public:
  int counter;
  int time;
  bool loop;
  Picture picture;
};

WalkerPtr Locust::create(PlayerCityPtr city)
{
  Locust* locust = new Locust( city );

  WalkerPtr ret( locust );
  ret->drop();

  return ret;
}

void Locust::create(PlayerCityPtr city, TilePos pos, int time)
{
  Locust* locust = new Locust( city );
  locust->setPos( pos );

  WalkerPtr ret( locust );
  ret->drop();

  city->addWalker( ret );
}

Locust::Locust( PlayerCityPtr city ) : Walker( city ), _d( new Impl )
{
  _setType( walker::locust );

  _d->time = 0;

  setName( _("##locust##") );
  _setHealth( 0 );

  setFlag( vividly, false );
}

Locust::~Locust()
{
}

void Locust::timeStep(const unsigned long time)
{
  _d->counter++;

  if( game::Date::isWeekChanged() )
  {
    FarmPtr farm;
    farm << _city()->getOverlay( pos() );
    if( farm.isValid() && farm->type() != objects::meat_farm )
    {
      farm->updateProgress( -50 );
    }
  }

  if( _d->counter > _d->time )
  {
    deleteLater();
  }
}

void Locust::save( VariantMap& stream ) const
{
  Walker::save( stream );

  stream[ "time" ] = _d->time;
}

void Locust::load( const VariantMap& stream )
{
  Walker::load( stream );

  _d->time = stream.get( "time" );

  //_d->picture = Picture::load( _d->rcGroup, _d->currentIndex );
}

const Picture& Locust::getMainPicture()
{
  return _d->picture;
}
