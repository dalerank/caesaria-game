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

#include "oc3_walker_animals.hpp"
#include "oc3_variant.hpp"
#include "oc3_city.hpp"
#include "oc3_astarpathfinding.hpp"

class Animal::Impl
{
public:
  TilePos destination;
  CityPtr city;
};

Animal::Animal( CityPtr city ) : _d( new Impl )
{
  _setType( WT_NONE );
  _setGraphic( WG_NONE );
  _d->city = city;

  setName( "Animal" );
}

void Animal::send2City( const Tile& startTile )
{
  _d->city->addWalker( WalkerPtr( this ) );
}

Animal::~Animal()
{

}

void Animal::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destination" ] = _d->destination;
}

void Animal::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->destination = stream.get( "destination" ).toTilePos();
}


Sheep::Sheep( CityPtr city ) : Animal( city )
{
  _setGraphic( WG_ANIMAL_SHEEP );
  _setType( WT_ANIMAL_SHEEP );
}

void Sheep::send2City(const Tile &startTile)
{
  int loopIndex = 0;
  bool foundPath = false;
  do
  {
    const Tilemap& tmap = _d->city->getTilemap();
    int range = 10;
    TilePos dest( std::rand() % range, std::rand() % range );


  }
  while( !foundPath && loopIndex < 20 );

  if( !foundPath )
  {
    deleteLater();

  }
}
