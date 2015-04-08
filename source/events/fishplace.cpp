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

#include "fishplace.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"
#include "core/priorities.hpp"
#include "core/logger.hpp"
#include "city/cityservice_fishplace.hpp"
#include "city/statistic.hpp"

using namespace constants;

namespace events
{

class ChangeFishery::Impl
{
public:
  TilePos location;
  ChangeFishery::Mode mode;
};

GameEventPtr ChangeFishery::create( TilePos pos, Mode mode )
{
  ChangeFishery* fp = new ChangeFishery();
  fp->_d->location = pos;
  fp->_d->mode = mode;

  GameEventPtr ret( fp );
  ret->drop();

  return ret;
}

void ChangeFishery::_exec( Game& game, unsigned int time)
{
  city::FisheryPtr fishery = city::statistic::finds<city::Fishery>( game.city() );

  if( fishery.isValid() )
  {
    switch( _d->mode )
    {
    case ChangeFishery::add: fishery->addLocation( _d->location ); break;

    case ChangeFishery::unknown:
    case ChangeFishery::remove:
      break;
    }
  }
}

bool ChangeFishery::_mayExec(Game&, unsigned int) const { return true; }

void ChangeFishery::load(const VariantMap& stream) {}
VariantMap ChangeFishery::save() const{ return VariantMap(); }

ChangeFishery::ChangeFishery() : _d( new Impl )
{
}

}//end namespace events
