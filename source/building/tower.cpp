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

#include "tower.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "game/city.hpp"
#include "gfx/tilemap.hpp"
#include "fortification.hpp"
#include "core/direction.hpp"

using namespace constants;

class Tower::Impl
{
public:
};

Tower::Tower() : ServiceBuilding( Service::guard, building::tower, Size( 2 ) ), _d( new Impl )
{
  setMaxWorkers( 6 );
  setPicture( ResourceGroup::land2a, 149 );
  _fireIncrement = 0;
  _damageIncrement = 0;
}

void Tower::save(VariantMap& stream) const
{
  Building::save( stream );
}

void Tower::load(const VariantMap& stream)
{
  Building::load( stream );
}

bool Tower::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  Tilemap& tmap = city->getTilemap();

  bool freeMap[ countDirection ] = { 0 };
  freeMap[ noneDirection ] = tmap.at( pos ).getFlag( Tile::isConstructible );
  freeMap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getFlag( Tile::isConstructible );
  freeMap[ east ] = tmap.at( pos + TilePos( 1, 0 ) ).getFlag( Tile::isConstructible );
  freeMap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getFlag( Tile::isConstructible );

  bool frtMap[ countDirection ] = { 0 };
  frtMap[ noneDirection ] = tmap.at( pos ).getOverlay().is<Fortification>();
  frtMap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getOverlay().is<Fortification>();
  frtMap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getOverlay().is<Fortification>();
  frtMap[ east  ] = tmap.at( pos + TilePos( 1, 0 ) ).getOverlay().is<Fortification>();

  bool mayConstruct = ((frtMap[ noneDirection ] || freeMap[ noneDirection ]) &&
                       (frtMap[ north ] || freeMap[ north ]) &&
                       (frtMap[ east ] || freeMap[ east ]) &&
                       (frtMap[ northEast ] || freeMap[ northEast ]) );

  return mayConstruct;
}

void Tower::deliverService()
{

}
