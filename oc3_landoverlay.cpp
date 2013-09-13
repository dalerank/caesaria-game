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

#include "oc3_landoverlay.hpp"
#include "oc3_building_data.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"

class LandOverlay::Impl
{
public:
  BuildingType buildingType;
  BuildingClass buildingClass;
  Tile* masterTile;  // left-most tile if multi-tile, or "this" if single-tile
  std::string name;
  Picture picture;
  Size size;  // size in tiles
  Animation animation;  // basic animation (if any)
  bool isDeleted;
  CityPtr city;
};

LandOverlay::LandOverlay(const BuildingType type, const Size& size)
: _d( new Impl )
{
  _d->masterTile = 0;
  _d->size = size;
  _d->isDeleted = false;
  _d->name = "unknown";

  setType( type );
}

LandOverlay::~LandOverlay()
{
  // what we shall to do here?
}


BuildingType LandOverlay::getType() const
{
   return _d->buildingType;
}

void LandOverlay::setType(const BuildingType buildingType)
{
  const BuildingData& bd = BuildingDataHolder::instance().getData( buildingType );

   _d->buildingType = buildingType;
   _d->buildingClass = bd.getClass();
   _d->name = bd.getName();
}

void LandOverlay::timeStep(const unsigned long time) { }

void LandOverlay::setPicture(const Picture &picture)
{
  _d->picture = picture;

  if (_d->masterTile != NULL)
  {
     Tilemap &tilemap = _getCity()->getTilemap();
     // _master_tile == NULL is cloneable buildings
     TilePos pos = _d->masterTile->getIJ();

     for (int dj = 0; dj<_d->size.getWidth(); ++dj)
     {
        for (int di = 0; di<_d->size.getHeight(); ++di)
        {
           Tile &tile = tilemap.at( pos + TilePos( di, dj ) );
           tile.setPicture( &_d->picture );
        }
     }
  }
}

void LandOverlay::build( CityPtr city, const TilePos& pos )
{
  Tilemap &tilemap = city->getTilemap();

  _d->city = city;
  _d->masterTile = &tilemap.at( pos );

  for (int dj = 0; dj < _d->size.getWidth(); ++dj)
  {
    for (int di = 0; di < _d->size.getHeight(); ++di)
    {
      Tile& tile = tilemap.at( pos + TilePos( di, dj ) );
      tile.setMasterTile(_d->masterTile);
      tile.setPicture( &_d->picture);
      TerrainTile& terrain = tile.getTerrain();

      if( terrain.getOverlay().isValid() && terrain.getOverlay() != this)
      {
        terrain.getOverlay()->deleteLater();
      }

      terrain.setOverlay(this);
      setTerrain( terrain );
    }
  }
}

void LandOverlay::deleteLater()
{
  _d->isDeleted  = true;
}

void LandOverlay::destroy()
{
}

Tile& LandOverlay::getTile() const
{
  _OC3_DEBUG_BREAK_IF( !_d->masterTile && "master tile must be exists" );
  return *_d->masterTile;
}

Size LandOverlay::getSize() const
{
  return _d->size;
}

bool LandOverlay::isDeleted() const
{
  return _d->isDeleted;
}

const Picture &LandOverlay::getPicture() const
{
  return _d->picture;
}

std::vector<Picture>& LandOverlay::getForegroundPictures()
{
  return _fgPictures;
}

std::string LandOverlay::getName()
{
  return _d->name;
}

void LandOverlay::save( VariantMap& stream ) const
{
  stream[ "pos" ] = getTile().getIJ();
  stream[ "buildingTypeName" ] = Variant( BuildingDataHolder::instance().getData( _d->buildingType ).getName() );
  stream[ "buildingType" ] = (int)_d->buildingType;
  stream[ "picture" ] = Variant( _d->picture.getName() );
  stream[ "size" ] = _d->size;
  stream[ "isDeleted" ] = _d->isDeleted;
  stream[ "name" ] = Variant( _d->name );
}

void LandOverlay::load( const VariantMap& stream )
{
  _d->name = stream.get( "name" ).toString();
  _d->buildingType = (BuildingType)stream.get( "buildingType" ).toInt();
  _d->picture = Picture::load( stream.get( "picture" ).toString() + ".png" );
  _d->size = stream.get( "size" ).toSize();
  _d->isDeleted = stream.get( "isDeleted" ).toBool();
}

bool LandOverlay::isWalkable() const
{
  return false;
}

TilePos LandOverlay::getTilePos() const
{
  _OC3_DEBUG_BREAK_IF( !_d->masterTile && "master tile can't be null" );
  return _d->masterTile ? _d->masterTile->getIJ() : TilePos( -1, -1 );
}

void LandOverlay::setName( const std::string& name )
{
  _d->name = name;
}

void LandOverlay::setSize( const Size& size )
{
  _d->size = size;
}

Point LandOverlay::getOffset( const Point& subpos ) const
{
  return Point( 0, 0 );
}

Animation& LandOverlay::_getAnimation()
{
  return _d->animation;
}

Tile* LandOverlay::_getMasterTile()
{
  return _d->masterTile;
}

CityPtr LandOverlay::_getCity() const
{
  return _d->city;
}

BuildingClass LandOverlay::getClass() const
{
  return _d->buildingClass;
}
