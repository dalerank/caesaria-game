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

#include "lion_tamer.hpp"
#include "city/city.hpp"
#include "lion.hpp"
#include "pathway/pathway.hpp"

using namespace constants;

class LionTamer::Impl
{
public:
  int delay;
};

LionTamerPtr LionTamer::create(PlayerCityPtr city)
{
  LionTamerPtr ret( new LionTamer( city ) );
  ret->drop();

  return ret;
}

void LionTamer::timeStep(const unsigned long time)
{
  if( _d->delay > 0 )
  {
    _d->delay--;
    return;
  }

  TraineeWalker::timeStep( time );
}

void LionTamer::send2City(BuildingPtr base, bool roadOnly)
{
  TraineeWalker::send2City( base, roadOnly );

  if( !isDeleted() )
  {
    LionPtr lion = Lion::create( _getCity() );
    lion->setIJ( getIJ() );
    lion->setPathway( _pathwayRef() );
    lion->go();
    _d->delay = 12;

    _getCity()->addWalker( csDynamicCast<Walker>( lion ) );
  }
}

LionTamer::~LionTamer()
{

}

LionTamer::LionTamer(PlayerCityPtr city)
  : TraineeWalker( city, walker::lionTamer ), _d( new Impl )
{
  _d->delay = 0;
}
