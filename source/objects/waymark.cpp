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

#include "waymark.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/helper.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::waymark, Waymark)

Waymark::Waymark()
  : TileOverlay( constants::objects::waymark, Size(1) )
{
}

void Waymark::timeStep( const unsigned long time )
{
  TileOverlay::timeStep( time );
}

bool Waymark::isFlat() const { return _isFlat; }
bool Waymark::isDestructible() const{  return false; }

void Waymark::initTerrain(Tile& terrain)
{

}

bool Waymark::build( const CityAreaInfo& info )
{  
  bool isEntryMark = false;

  Tilemap& tmap = info.city->tilemap();
  TilesArray around = tmap.getNeighbors( info.pos );
  TilePos entryPos = info.city->borderInfo().roadEntry;
  foreach( it, around )
  {
    if( (*it)->pos() == entryPos )
    {
      isEntryMark = true;
      break;
    }
  }

  unsigned int picIndex = isEntryMark ? 89 : 85;
  const TilePos& pos = info.pos;
  if( pos.i() == 0 || pos.i() == tmap.size() - 1 )
  {
    picIndex += (pos.i() == 0 ? 1 : 3 );
  }
  else if( pos.j() == 0 || pos.j() == tmap.size() - 1 )
  {
    picIndex += (pos.j() == 0 ? 2 : 0 );
  }
  else
  {
    Picture pic = MetaDataHolder::randomPicture( objects::terrain, Size(1) );
    Tile& oTile = tmap.at( info.pos );
    oTile.setPicture( pic );
    oTile.setOriginalImgId( imgid::fromResource( pic.name() ) );
    deleteLater();
  }

  setPicture( ResourceGroup::land3a, picIndex );
  _isFlat = picture().height() <= tilemap::cellPicSize().height();

  return TileOverlay::build( info );
}
