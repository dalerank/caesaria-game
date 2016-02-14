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

#include "empiremap.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "core/variant_map.hpp"
#include "gfx/tilepos.hpp"
#include "core/tilepos_array.hpp"
#include "routefinder.hpp"

namespace world
{

struct EmTile
{  
  TilePos pos;
  EmpireMap::TerrainType terrain = EmpireMap::trUnknown;

  EmTile() {}
  EmTile( const TilePos& p ) : pos (p) {}
};

static EmTile invalidTile = EmTile( TilePos( -1, -1 ) );
typedef std::vector< EmTile > EmRow;

class EmTileGrid : public std::vector< EmRow >
{
};

class EmpireMap::Impl : public EmTileGrid
{
public:
  EmTile& at( const TilePos& p )
  {
    if( isInside( p ) )
    {
      return (*this)[p.j()][p.i()];
    }

    //Logger::warning( "Need inside point current=[%d, %d]", i, j );
    return invalidTile;
  }

  bool isInside( const TilePos& pos )
  {
    return( pos.i() >= 0 && pos.j()>=0 && pos.i() < size.width() && pos.j() < size.height());
  }

  void resize( Size s )
  {
    size = s;

    // resize the tile array
    EmTileGrid::resize( size.height() );
    for( int j = 0; j < size.height(); ++j )
    {
      (*this)[j].resize( size.width() );

      for (int i = 0; i < size.width(); ++i)
      {
        TilePos p( i, j );
        at( p ).pos = p;
      }
    }
  }

  TilePos pnt2tp( Point pos ) { return TilePos( pos.x() / tilesize.width(), pos.y() / tilesize.height() ); }
  Point tp2pnt( TilePos tp ) { return Point( tp.i() * tilesize.width(), tp.j() * tilesize.height() ); }

  Size size;
  Size tilesize;
  ScopedPtr< TraderouteFinder > routefinder;
};

EmpireMap::TerrainType EmpireMap::getTerrainType( const TilePos& ij ) const
{
  return _d->at( ij ).terrain;
}

TilePos EmpireMap::point2location(const Point& point) const
{
   return _d->pnt2tp( point );
}

void EmpireMap::setTerrainType(const TilePos& ij, EmpireMap::TerrainType type)
{
  _d->at( ij ).terrain = type;
}

Rect EmpireMap::area(const TilePos& ij) const
{
  return Rect( Point( ij.i() * _d->tilesize.width(),ij.j() * _d->tilesize.height() ),
               _d->tilesize );
}

Route EmpireMap::findRoute(const Point& start, const Point& stop, int flags ) const
{
  Route way;
  Locations tiles;

  _d->routefinder->findRoute( _d->pnt2tp( start ), _d->pnt2tp( stop ), tiles, flags);

  for( const auto& pos : tiles)
    way.push_back( _d->tp2pnt( pos ) );

  return way;
}

EmpireMap::EmpireMap() : _d( new Impl )
{

}

EmpireMap::~EmpireMap() {}

void EmpireMap::load(const VariantMap& stream)
{
  if( stream.empty() )
  {
    Logger::warning( "EmpireMap: cann't initialize empiremap" );
    return;
  }

  _d->resize( stream.get( "size" ).toSize() );
  _d->tilesize = stream.get( "tilesize" ).toSize();  

  VariantList tiles = stream.get( "tiles" ).toList();
  int index = 0;
  for( const auto& v : tiles )
  {
    EmTile& tile = _d->at( TilePos( index % _d->size.width(), index / _d->size.width() ) );
    tile.terrain = (TerrainType)v.toInt();
    index++;
  }

  _d->routefinder.createInstance( *this );
}

VariantMap EmpireMap::save() const
{
  VariantMap ret;

  VariantList vTiles;
  for( int j=0; j < _d->size.height(); j++ )
    for( int i=0; i < _d->size.width(); i++ )
    {
      EmTile& tile = _d->at( TilePos( i, j ) );
      vTiles.push_back( tile.terrain );
    }

  ret[ "tiles" ] = vTiles;
  ret[ "size"  ] = _d->size;
  ret[ "tilesize" ] = _d->tilesize;

  return ret;
}

void EmpireMap::setCity(const Point& rpos)
{
  TilePos pos = _d->pnt2tp( rpos );

  if( _d->isInside( pos ) )
  {
    EmTile& tile = _d->at( pos );
    tile.terrain = EmpireMap::trCity;
    _d->routefinder->setTerrainType( pos, EmpireMap::trCity );
  }
}

Size EmpireMap::size() const { return _d->size; }

}
