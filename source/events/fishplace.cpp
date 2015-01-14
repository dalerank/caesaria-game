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

using namespace constants;

namespace events
{

namespace {
}

class FishPlaceEvent::Impl
{
public:
  TilePos location;
  FishPlaceEvent::Mode mode;
};

GameEventPtr FishPlaceEvent::create( TilePos pos, Mode mode )
{
  FishPlaceEvent* fp = new FishPlaceEvent();
  fp->_d->location = pos;
  fp->_d->mode = mode;

  GameEventPtr ret( fp );
  ret->drop();

  return ret;
}

void FishPlaceEvent::_exec( Game& game, unsigned int time)
{
  city::FisheryPtr fishery;
  fishery << game.city()->findService( city::Fishery::defaultName() );
  if( fishery.isValid() )
  {
    switch( _d->mode )
    {
    case FishPlaceEvent::add: fishery->addLocation( _d->location ); break;

    case FishPlaceEvent::unknown:
    case FishPlaceEvent::remove:
      break;
    }
  }
}

bool FishPlaceEvent::_mayExec(Game&, unsigned int) const { return true; }

void FishPlaceEvent::load(const VariantMap& stream) {}
VariantMap FishPlaceEvent::save() const{ return VariantMap(); }

FishPlaceEvent::FishPlaceEvent() : _d( new Impl )
{
}

}//end namespace events
