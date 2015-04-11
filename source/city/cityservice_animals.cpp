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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "cityservice_animals.hpp"
#include "city.hpp"
#include "gfx/tile.hpp"
#include "game/gamedate.hpp"
#include "gfx/tilemap.hpp"
#include "walker/animals.hpp"
#include "core/variant_map.hpp"
#include "walker/constants.hpp"
#include "walker/helper.hpp"
#include "walker/walkers_factory.hpp"
#include "cityservice_factory.hpp"
#include "config.hpp"

using namespace gfx;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Animals,animals)

class Animals::Impl
{
public:
  std::map< walker::Type, unsigned int > maxAnimal;
};

SrvcPtr Animals::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Animals( city ) );
  ret->drop();

  return ret;
}

std::string Animals::defaultName() { return CAESARIA_STR_EXT(Animals); }

void Animals::timeStep(const unsigned int time)
{
  if( !game::Date::isMonthChanged() )
    return;

  if( _d->maxAnimal.empty() )
  {
    walker::Type currentTerrainAnimal = _city()->climate() == game::climate::desert
                                          ? walker::zebra
                                          : walker::sheep;
    _d->maxAnimal[ currentTerrainAnimal ] = config::animals::defaultNumber;
  }

  TilesArray border = _city()->tilemap().border();
  border = border.walkables( true );

  foreach( winfo, _d->maxAnimal )
  {
    walker::Type walkerType = winfo->first;
    unsigned int maxAnimalInCity = winfo->second;

    if( maxAnimalInCity > 0 )
    {
      WalkerList animals = _city()->walkers( walkerType );
      if( animals.size() < maxAnimalInCity )
      {
        AnimalPtr animal = WalkerManager::instance().create<Animal>( walkerType, _city() );
        if( animal.isValid() )
        {
          Tile* rndTile = border.random();
          animal->send2City( rndTile->epos() );
        }
      }
    }
  }
}

void Animals::setAnimalsNumber( walker::Type animal_type, unsigned int number)
{
  _d->maxAnimal[ animal_type ] = number;
}

VariantMap Animals::save() const
{
  VariantMap ret = Srvc::save();

  VariantMap animalsVm;
  foreach( winfo, _d->maxAnimal )
    animalsVm[ WalkerHelper::getTypename( winfo->first ) ] = winfo->second;

  ret[ "animals" ] = animalsVm;

  return ret;
}

void Animals::load(const VariantMap& stream)
{
  Srvc::load( stream );

  VariantMap animalsVm = stream.get( "animals" ).toMap();
  foreach( info, animalsVm )
  {
    walker::Type wtype = WalkerHelper::getType( info->first );
    _d->maxAnimal[ wtype ] = info->second;
  }
}

Animals::Animals(PlayerCityPtr city)
  : Srvc( city, Animals::defaultName() ), _d( new Impl )
{
}

}//end namespace city
