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

#include "pathway.hpp"
#include "gfx/helper.hpp"
#include "core/direction.hpp"
#include "core/variant_map.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;

namespace {
static Tile invalidTile( TilePos(-1, -1) );
}

class Directions : public std::vector<constants::Direction>
{
public:
  Directions& operator =(const Directions& a )
  {
    size_type newSize = a.size();
    resize( newSize );
    clear();
    insert(begin(), a.begin(), a.end());

    return *this;
  }
};

bool operator<(const Pathway& v1, const Pathway& v2)
{
  if( v1.length()!=v2.length() )
  {
    return v1.length() < v2.length();
  }

  // compare memory address
  return (&v1 < &v2);
}

class Pathway::Impl
{
public:
  TilePos destination;
  TilePos origin;
  bool isReverse;
  TilesArray tiles;
  unsigned int step;
};

Pathway::Pathway() : _d( new Impl )
{
  _d->origin = TilePos( 0, 0 );
  _d->destination = TilePos( 0, 0 );
  _d->step = 0;
  _d->isReverse = false;
}

Pathway::~Pathway(){}

Pathway::Pathway(const Pathway &copy) : _d( new Impl )
{
  *this = copy;
}

void Pathway::init( const Tile& origin)
{
  _d->origin = origin.pos();
  _d->isReverse = false;
  _d->destination = origin.pos();
  _d->tiles.clear();
  _d->tiles.push_back( &const_cast<Tile&>( origin ) );
}

unsigned int Pathway::length() const
{
  // TODO: various lands have various travel time (road easier to travel than open country)
  return _d->tiles.size();
}

const Tile& Pathway::front() const { return _d->tiles.empty() ? invalidTile : *_d->tiles.front(); }
bool Pathway::isReverse() const {  return _d->isReverse; }
const TilesArray& Pathway::allTiles() const {  return _d->tiles; }
const Tile& Pathway::back() const { return _d->tiles.empty() ? invalidTile : *_d->tiles.back(); }
TilePos Pathway::startPos() const {  return _d->isReverse ? _d->destination : _d->origin;}
TilePos Pathway::stopPos() const{  return _d->isReverse ? _d->origin : _d->destination; }

const Tile& Pathway::current() const
{
  return _d->step < _d->tiles.size()
           ? *_d->tiles[ _d->step ]
           : invalidTile;
}

constants::Direction Pathway::direction()
{
  if( !_d->tiles.empty() )
  {
    if(_d->isReverse )
    {
      if( _d->step > 0 )
        return tilemap::getDirection( _d->tiles[_d->step]->epos(),  _d->tiles[ _d->step-1]->epos() );
    }
    else
    {
      if( _d->step < _d->tiles.size()-1 )
        return tilemap::getDirection( _d->tiles[_d->step]->epos(),  _d->tiles[ _d->step+1]->epos() );
    }
  }

  return constants::noneDirection;
}

void Pathway::toggleDirection()
{
  _d->isReverse = !_d->isReverse;
}

void Pathway::next()
{
  if( _d->isReverse )
  {
    if( _d->step > 0 )
      _d->step--;
  }
  else
  {
    if( _d->step < _d->tiles.size()-1)
      _d->step++;
  }
}

bool Pathway::isDestination() const
{
  if (_d->isReverse)
  {
    return _d->step == 0;
  }
  else
  {
    return ( _d->step == _d->tiles.size()-1 );
  }
}

void Pathway::setNextDirection( const Tilemap& tmap, Direction direction)
{
  switch (direction)
  {
  case north      : _d->destination += TilePos( 0, 1 );  break;
  case northEast  : _d->destination += TilePos( 1, 1 );  break;
  case east       : _d->destination += TilePos( 1, 0 );  break;
  case southEast  : _d->destination += TilePos( 1, -1 ); break;
  case south      : _d->destination += TilePos( 0, -1 ); break;
  case southWest  : _d->destination += TilePos( -1, -1 );break;
  case west       : _d->destination += TilePos( -1, 0 ); break;
  case northWest  : _d->destination += TilePos( -1, 1 ); break;
  default:
    _d->destination += TilePos( 0, 1 );  break;
    Logger::warning( "Unexpected Direction:%d", direction);
  break;
  }

  if( !tmap.isInside( TilePos( _d->destination ) ) )
  {
    //Logger::warning( "Destination[%d, %d] out of map", _d->destination.i(), _d->destination.j() );
  }
  else
  {
    _d->tiles.push_back( const_cast<Tile*>( &tmap.at( _d->destination )) );
  }
}

void Pathway::setNextTile( const Tile& tile )
{
  int dI = tile.i() - _d->destination.i();
  int dJ = tile.j() - _d->destination.j();

  Direction direction;

  if (dI==0 && dJ==0) {  direction = noneDirection; }
  else if (dI==0 && dJ==1) { direction = north; }
  else if (dI==1 && dJ==1) { direction = northEast; }
  else if (dI==1 && dJ==0) { direction = east; }
  else if (dI==1 && dJ==-1){ direction = southEast; }
  else if (dI==0 && dJ==-1){ direction = south; }
  else if (dI==-1 && dJ==-1){ direction = southWest;}
  else if (dI==-1 && dJ==0) {direction = west;}
  else if (dI==-1 && dJ==1){ direction = northWest; }
  else
  {
    Logger::warning( "WARNING!!! Pathway::setNextTile() destination[%d, %d] out of map", dI, dJ );
    direction = noneDirection;
  }

  _d->tiles.push_back( const_cast<Tile*>( &tile ) );
}

bool Pathway::contains(Tile &tile)
{
  // search in reverse direction, because usually the last tile matches
  bool res = false;
  for( TilesArray::reverse_iterator itTile = _d->tiles.rbegin();
       itTile != _d->tiles.rend(); ++itTile)
  {
    if (*itTile == &tile)
    {
      res = true;
      break;
    }
  }

  return res;
}

void Pathway::prettyPrint() const
{
  Logger::warning( "pathWay from [%d,%d] to [%d,%d]",
                   _d->origin.i(), _d->origin.j(), _d->destination.i(), _d->destination.j() );

  std::string strDir = "";
  for( unsigned int k=0; k < _d->tiles.size()-1; k++ )
  {
    Direction direction = tilemap::getDirection( _d->tiles[k]->pos(), _d->tiles[k+1]->pos() );

    switch (direction)
    {
    case north: strDir += "N";  break;
    case northEast: strDir += "NE"; break;
    case east: strDir += "E"; break;
    case southEast: strDir += "SE"; break;
    case south: strDir += "S";   break;
    case southWest: strDir += "SW"; break;
    case west: strDir += "W";  break;
    case northWest: strDir += "NW"; break;
    default:
      //"Unexpected Direction:"
      _CAESARIA_DEBUG_BREAK_IF( direction );
    break;
    }

    strDir += " ";
  }

  Logger::warning( strDir );
}

VariantMap Pathway::save() const
{
  VariantMap stream;
  if( length() == 0 ) //not save empty way
  {
    return VariantMap();
  }

  VariantList directions;
  foreach( it, _d->tiles )
  {
    directions.push_back( (*it)->pos() );
  }

  stream[ "tiles" ] = directions;
  stream[ "reverse" ] = _d->isReverse;
  stream[ "step" ] = curStep();
  stream[ "startPos" ] = _d->origin;
  stream[ "endPos"] = _d->destination;

  return stream;
}

bool Pathway::isValid() const { return length() > 0; }

void Pathway::load(const Tilemap& tmap, const VariantMap& stream )
{
  if( stream.size() == 0 )
  {
    return;
  }

  _d->origin = stream.get( "startPos" );
  _d->destination = stream.get( "endPos" ); //stream.get( "stopPos" ).toTilePos();
  VariantList vmTiles = stream.get( "tiles" ).toList();
  foreach( it, vmTiles )
  {
    _d->tiles.push_back( const_cast<Tile*>( &tmap.at( it->toTilePos() )) );
  }

  _d->isReverse = stream.get( "reverse" ).toBool();
  _d->step = stream.get( "step" ).toInt();
}

Pathway& Pathway::operator=( const Pathway& other )
{
  _d->origin              = other._d->origin;
  _d->destination         = other._d->destination;
  _d->isReverse           = other._d->isReverse;
  _d->step                = other._d->step;
  _d->tiles               = other._d->tiles;

  return *this;
}

Pathway Pathway::copy(unsigned int start, int stop) const
{
  Pathway ret;
  if( start >= length() )
  {
    return ret;
  }

  ret.init( *_d->tiles[ start ] );
  stop = (stop == -1 ? _d->tiles.size() : stop );
  for( int i=start+1; i < stop; i++ )
  {
    ret.setNextTile( *_d->tiles[ i ] );
  }

  return ret;
}

unsigned int Pathway::curStep() const
{
  if(_d->isReverse)
  {
    return _d->tiles.size() - _d->step;
  }
  else
  {
    return _d->step;
  }
}

void Pathway::move( Pathway::DirectionType type )
{
  if( type == forward )
  {
    _d->step = 0;
    _d->isReverse = false;
  }
  else
  {
    _d->step = _d->tiles.size() - 1;
    _d->isReverse = true;
  }
}
