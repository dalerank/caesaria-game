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
#include "objects/overlay.hpp"
#include "gfx/tilemap_config.hpp"
#include "core/direction.hpp"
#include "core/variant_map.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "core/variant_list.hpp"
#include "gfx/tilesarray.hpp"

using namespace gfx;

namespace {
static Tile invalidTile( TilePos(-1, -1) );
GAME_LITERALCONST(tiles)
}

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
  TilePos endPos;
  TilePos startPos;
  bool reverse;
  TilesArray tiles;
  unsigned int step;
};

Pathway::Pathway() : _d( new Impl )
{
  _d->startPos = TilePos( 0, 0 );
  _d->endPos = TilePos( 0, 0 );
  _d->step = 0;
  _d->reverse = false;
}

Pathway::~Pathway(){}

Pathway::Pathway(const Pathway &copy) : _d( new Impl )
{
  *this = copy;
}

void Pathway::init( const Tile& origin)
{
  _d->startPos = origin.epos();
  _d->reverse = false;
  _d->endPos = origin.epos();
  _d->tiles.clear();
  _d->tiles.push_back( &const_cast<Tile&>( origin ) );
}

unsigned int Pathway::length() const
{
  // TODO: various lands have various travel time (road easier to travel than open country)
  return _d->tiles.size();
}

const Tile& Pathway::front() const {  return _d->tiles.empty() ? invalidTile : *_d->tiles.front(); }
bool Pathway::isReverse()    const {  return _d->reverse; }
const TilesArray& Pathway::allTiles() const {  return _d->tiles; }
const Tile& Pathway::back()  const {  return _d->tiles.empty() ? invalidTile : *_d->tiles.back(); }
TilePos Pathway::startPos()  const {  return _d->reverse ? _d->endPos : _d->startPos;}
TilePos Pathway::stopPos()   const {  return _d->reverse ? _d->startPos : _d->endPos; }

const Tile& Pathway::current() const
{
  return _d->step < _d->tiles.size()
           ? *_d->tiles[ _d->step ]
           : invalidTile;
}

Direction Pathway::direction() const
{
  if( !_d->tiles.empty() )
  {
    if(_d->reverse )
    {
      if( _d->step > 0 )
        return _d->tiles[_d->step]->epos().directionTo( _d->tiles[ _d->step-1]->epos() );
    }
    else
    {
      if( _d->step < _d->tiles.size()-1 )
        return _d->tiles[_d->step]->epos().directionTo( _d->tiles[ _d->step+1]->epos() );
    }
  }

  return direction::none;
}

void Pathway::toggleDirection()
{
  _d->reverse = !_d->reverse;
}

void Pathway::next()
{
  if( _d->reverse )
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
  if (_d->reverse)
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
  case direction::north      : _d->endPos += TilePos( 0, 1 );  break;
  case direction::northEast  : _d->endPos += TilePos( 1, 1 );  break;
  case direction::east       : _d->endPos += TilePos( 1, 0 );  break;
  case direction::southEast  : _d->endPos += TilePos( 1, -1 ); break;
  case direction::south      : _d->endPos += TilePos( 0, -1 ); break;
  case direction::southWest  : _d->endPos += TilePos( -1, -1 );break;
  case direction::west       : _d->endPos += TilePos( -1, 0 ); break;
  case direction::northWest  : _d->endPos += TilePos( -1, 1 ); break;
  default:
    _d->endPos += TilePos( 0, 1 );  break;
    Logger::warning( "WARNING !!! Unexpected Direction:{}", direction);
  break;
  }

  if( tmap.isInside( _d->endPos ) )
  {
    _d->tiles.push_back( const_cast<Tile*>( &tmap.at( _d->endPos )) );
  }
}

void Pathway::setNextTile( const Tile& tile )
{
  _d->tiles.push_back( const_cast<Tile*>( &tile ) );
  _d->endPos = tile.epos();
}

void Pathway::append(const Pathway &other)
{
  _d->tiles.append( other._d->tiles );
  if( !_d->tiles.empty() )
    _d->endPos = _d->tiles.back()->epos();
}

bool Pathway::contains(const Tile& tile)
{
  // search in reverse direction, because usually the last tile matches
  bool res = false;
  for( auto itTile = _d->tiles.rbegin();
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
  Logger::warning( "pathWay from [{},{}] to [{},{}]",
                   _d->startPos.i(), _d->startPos.j(), _d->endPos.i(), _d->endPos.j() );

  std::string strDir = "";
  for( unsigned int k=0; k < _d->tiles.size()-1; k++ )
  {
    Direction direction = _d->tiles[k]->pos().directionTo( _d->tiles[k+1]->pos() );

    switch (direction)
    {
    case direction::north: strDir += "N";  break;
    case direction::northEast: strDir += "NE"; break;
    case direction::east: strDir += "E"; break;
    case direction::southEast: strDir += "SE"; break;
    case direction::south: strDir += "S";   break;
    case direction::southWest: strDir += "SW"; break;
    case direction::west: strDir += "W";  break;
    case direction::northWest: strDir += "NW"; break;
    default:
      //"Unexpected Direction:"
      _GAME_DEBUG_BREAK_IF( direction );
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
    return VariantMap();

  stream[ literals::tiles ] = _d->tiles.locations().save();
  VARIANT_SAVE_ANY_D( stream, _d, reverse )
  VARIANT_SAVE_ANY_D( stream, _d, step )
  VARIANT_SAVE_ANY_D( stream, _d, startPos )
  VARIANT_SAVE_ANY_D( stream, _d, endPos )

  return stream;
}

bool Pathway::isValid() const { return length() > 0; }

void Pathway::load(const Tilemap& tmap, const VariantMap& stream )
{
  if( stream.empty() )
  {
    return;
  }

  VARIANT_LOAD_ANY_D( _d, startPos, stream )
  VARIANT_LOAD_ANY_D( _d, endPos,   stream )
  VariantList vmTiles = stream.get( literals::tiles ).toList();
  for( const auto& it : vmTiles )
    _d->tiles.push_back( const_cast<Tile*>( &tmap.at( it.toTilePos() )) );

  VARIANT_LOAD_ANY_D( _d, reverse, stream )
  VARIANT_LOAD_ANY_D( _d, step, stream )
}

Pathway& Pathway::operator=( const Pathway& other )
{
  _d->startPos = other._d->startPos;
  _d->endPos   = other._d->endPos;
  _d->reverse  = other._d->reverse;
  _d->step     = other._d->step;
  _d->tiles    = other._d->tiles;

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
  if(_d->reverse)
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
    _d->reverse = false;
  }
  else
  {
    _d->step = _d->tiles.size() - 1;
    _d->reverse = true;
  }
}
