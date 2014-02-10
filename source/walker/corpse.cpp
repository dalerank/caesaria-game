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

#include "corpse.hpp"
#include "core/variant.hpp"
#include "city/city.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"

using namespace constants;

class Corpse::Impl
{
public:
  std::string rcGroup;
  int startIndex;
  int currentIndex;
  int stopIndex;
  int time;
  int delay;
  bool loop;
  Picture picture;
};

WalkerPtr Corpse::create(PlayerCityPtr city)
{
  Corpse* corpse = new Corpse( city );

  WalkerPtr ret( corpse );
  ret->drop();

  return ret;
}

void Corpse::create(PlayerCityPtr city, TilePos pos,
                    std::string rcGroup, int startIndex, int stopIndex,
                    bool loop )
{
  Corpse* corpse = new Corpse( city );
  corpse->setPos( pos );
  corpse->_d->startIndex = startIndex;
  corpse->_d->currentIndex = startIndex+1;
  corpse->_d->stopIndex = stopIndex;
  corpse->_d->rcGroup = rcGroup;
  corpse->_d->time = 0;
  corpse->_d->delay = 1;
  corpse->_d->loop = loop;

  if( !rcGroup.empty() )
  {
    corpse->_d->picture = Picture::load( rcGroup, startIndex );
  }

  WalkerPtr ret( corpse );
  ret->drop();

  city->addWalker( ret );
}

Corpse::Corpse( PlayerCityPtr city ) : Walker( city ), _d( new Impl )
{
  _setType( walker::corpse );
  _setAnimation( gfx::unknown );

  _d->startIndex = 0;
  _d->currentIndex = 0;
  _d->stopIndex = 0;
  _d->rcGroup = "";
  _d->time = 0;
  _d->delay = 0;
  _d->loop = false;

  setName( _("##corpse##") );
  _setHealth( 0 );
}

Corpse::~Corpse()
{
}

void Corpse::timeStep(const unsigned long time)
{
  if( _d->time >= _d->delay  )
  {
    _d->time = 0;

    if( _d->currentIndex < _d->stopIndex )
    {
      _d->picture = Picture::load( _d->rcGroup, _d->currentIndex );
      _d->currentIndex++;
      _d->delay *= 2;
    }
    else if( _d->loop )
    {
      _d->currentIndex = _d->startIndex;
    }
  }

  _d->time++;

  if( ( time % 8 == 1 ) && getHealth() <= 0 )
  {
    updateHealth( -1 );

    if( getHealth() <= -100 )
    {
      deleteLater();
    }
  }
}

void Corpse::save( VariantMap& stream ) const
{
  Walker::save( stream );

  stream[ "rc" ] = Variant( _d->rcGroup );
  stream[ "start" ] = _d->startIndex;
  stream[ "index" ] = _d->currentIndex;
  stream[ "stop" ] = _d->stopIndex;
  stream[ "time" ] = _d->time;
  stream[ "delay" ] = _d->delay;
  stream[ "loop" ] = _d->loop;
}

void Corpse::load( const VariantMap& stream )
{
  Walker::load( stream );

  _d->rcGroup = stream.get( "rc" ).toString();
  _d->startIndex = stream.get( "start" );
  _d->currentIndex = stream.get( "index" );
  _d->stopIndex = stream.get( "stop" );
  _d->time = stream.get( "time" );
  _d->delay = stream.get( "delay" );
  _d->loop = stream.get( "loop" );

  _d->picture = Picture::load( _d->rcGroup, _d->currentIndex );
}

const Picture& Corpse::getMainPicture()
{
  return _d->picture;
}
