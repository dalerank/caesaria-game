// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "corpse.hpp"
#include "core/variant.hpp"
#include "game/city.hpp"
#include "game/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "game/tilemap.hpp"
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

void Corpse::create(CityPtr city, TilePos pos,
                    const char* rcGroup, int startIndex, int stopIndex,
                    bool loop )
{
  Corpse* corpse = new Corpse( city );
  corpse->setIJ( pos );
  corpse->_d->startIndex = startIndex;
  corpse->_d->currentIndex = startIndex+1;
  corpse->_d->stopIndex = stopIndex;
  corpse->_d->rcGroup = rcGroup;
  corpse->_d->picture = Picture::load( rcGroup, startIndex );
  corpse->_d->time = 0;
  corpse->_d->delay = 1;
  corpse->_d->loop = loop;

  WalkerPtr ret( corpse );
  ret->drop();

  city->addWalker( ret );
}

Corpse::Corpse( CityPtr city ) : Walker( city ), _d( new Impl )
{
  _setType( walker::unknown );
  _setGraphic( WG_NONE );

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
}

void Corpse::load( const VariantMap& stream )
{
  Walker::load( stream );
}

const Picture& Corpse::getMainPicture()
{
  return _d->picture;
}
