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
#include "oc3_pathway_helper.hpp"
#include "oc3_gettext.hpp"

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

  setName( _("##Animal##") );
}

void Animal::send2City(const TilePos &start )
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

void Animal::_findNewWay( const TilePos& start )
{
  int loopIndex = 0;
  bool foundPath = false;
  do
  {
    const Tilemap& tmap = _d->city->getTilemap();
    int range = 10;
    TilePos dest( std::rand() % range- range / 2, std::rand() % range - range / 2 );
    dest = (start+dest).fit( TilePos( 0, 0 ), TilePos( tmap.getSize()-1, tmap.getSize()-1 ) );

    if( tmap.at( dest ).getTerrain().isWalkable(true) )
    {
      PathWay pathway = PathwayHelper::create( _d->city, start, dest, PathwayHelper::allTerrain );

      if( pathway.isValid() )
      {
        foundPath = true;
        setPathWay( pathway );
        setIJ( start );
        go();
      }
    }
  }
  while( !foundPath && ++loopIndex < 20 );

  if( !foundPath )
  {
    deleteLater();
  }
}



Sheep::Sheep( CityPtr city ) : Animal( city )
{
  _setGraphic( WG_ANIMAL_SHEEP_WALK );
  _setType( WT_ANIMAL_SHEEP );
  setName( _("##Sheep##") );
}

WalkerPtr Sheep::create(CityPtr city)
{
  WalkerPtr ret( new Sheep( city ) );
  ret->drop();

  return ret;
}

void Sheep::onDestination()
{
  Walker::onDestination();

  _findNewWay( getIJ() );
}

void Sheep::onNewTile()
{
  Walker::onNewTile();
  _getAnimation().setFrameDelay( 3 );
}

void Sheep::send2City(const TilePos &start )
{
  _findNewWay( start );

  if( !isDeleted() )
  {
    _d->city->addWalker( this );
  }
}
