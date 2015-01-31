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

#include "random_animals.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "city/cityservice_animals.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "walker/animals.hpp"
#include "walker/helper.hpp"
#include "core/variant_map.hpp"
#include "walker/walkers_factory.hpp"
#include "gfx/tilemap.hpp"
#include "factory.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

REGISTER_EVENT_IN_FACTORY(RandomAnimals, "random_animals")

class RandomAnimals::Impl
{
public:
  bool isDeleted;
  unsigned int count;
  walker::Type animalType;
  int maxAnimals;
};

GameEventPtr RandomAnimals::create()
{
  GameEventPtr ret( new RandomAnimals() );
  ret->drop();

  return ret;
}

GameEventPtr RandomAnimals::create( walker::Type type, unsigned int wolvesNumber)
{
  RandomAnimals* r = new RandomAnimals();
  r->_d->count = wolvesNumber;
  r->_d->animalType = type;

  GameEventPtr ret( r );
  ret->drop();

  return ret;
}

void RandomAnimals::_exec( Game& game, unsigned int time)
{
  Logger::warning( "Execute random animals event" );
  if( _d->count > 0 )
  {
    Tilemap& tmap = game.city()->tilemap();
    TilesArray border = tmap.getRectangle( TilePos( 0, 0 ), Size( tmap.size() ) );
    border = border.walkableTiles( true );

    Tile* randomTile = border.random();

    for( unsigned int k=0; k < _d->count; k++ )
    {
      AnimalPtr animal = ptr_cast<Animal>( WalkerManager::instance().create( _d->animalType, game.city() ) );
      if( animal.isValid() )
      {
        animal->send2City( randomTile->pos() );
        animal->wait( k * 20 );
      }
    }

    _d->count = 0;
  }

  if( _d->maxAnimals >= 0 )
  {
    city::AnimalsPtr srvc;
    srvc << game.city()->findService( city::Animals::defaultName() );

    if( srvc.isValid() )
    {
      srvc->setAnimalsNumber( _d->animalType, _d->maxAnimals );
    }
  }
}

bool RandomAnimals::_mayExec(Game&, unsigned int) const { return true; }
bool RandomAnimals::isDeleted() const {  return _d->isDeleted; }

void RandomAnimals::load(const VariantMap& stream)
{
  VARIANT_LOAD_ANY_D( _d, count, stream );
  VARIANT_LOAD_ANY_D( _d, maxAnimals, stream );

  Variant vAnimalType = stream.get( "animalType" );
  if( vAnimalType.type() == Variant::String )
  {
    _d->animalType = WalkerHelper::getType( vAnimalType.toString() );
  }
  else
  {
    _d->animalType = (walker::Type)stream.get( "animalType" ).toInt();
  }
}

VariantMap RandomAnimals::save() const
{
  VariantMap ret;

  VARIANT_SAVE_ANY_D( ret, _d, count );
  VARIANT_SAVE_ANY_D( ret, _d, maxAnimals );
  ret[ "animalType" ] = (int)_d->animalType;
  return ret;
}

RandomAnimals::RandomAnimals() : _d( new Impl )
{
  _d->isDeleted = true;
  _d->count = 0;
  _d->maxAnimals = 0;
}

}//end namespace events
