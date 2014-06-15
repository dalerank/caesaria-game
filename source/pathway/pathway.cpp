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
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "core/direction.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;

class Directions : public std::vector<constants::Direction>
{
public:
   Directions& operator =(const Directions& a )
   {
     size_type newSize = a.size();
     resize( newSize );
     std::memcpy( &front(), &a.front(), sizeof(Direction) * newSize );

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
  bool isReverse;
  Tilemap* tilemap;
  Directions directionList;
  Directions::iterator directionIt;
  Directions::reverse_iterator directionIt_reverse;
  TilesArray tileList;
  Tile* origin;
};

Pathway::Pathway() : _d( new Impl )
{
  _d->origin = NULL;
  _d->destination = TilePos( 0, 0 );
  _d->isReverse = false;
}

Pathway::~Pathway(){}

Pathway::Pathway(const Pathway &copy) : _d( new Impl )
{
  *this = copy;
}

void Pathway::init( Tilemap& tilemap, Tile &origin)
{
  _d->tilemap = &tilemap;
  _d->origin = &origin;
  _d->destination = origin.pos();
  _d->directionList.clear();
  _d->directionIt = _d->directionList.begin();
  _d->directionIt_reverse = _d->directionList.rbegin();
  _d->tileList.clear();
  _d->tileList.push_back(&origin);
}

unsigned int Pathway::length() const
{
  // TODO: various lands have various travel time (road easier to travel than open country)
  return _d->directionList.size();
}

const Tile& Pathway::front() const {  return *_d->origin; }
TilePos Pathway::getStartPos() const { return _d->origin ? _d->origin->pos() : TilePos( -1, -1); }
bool Pathway::isReverse() const {  return _d->isReverse; }
const TilesArray& Pathway::allTiles() const {  return _d->tileList; }

const Tile& Pathway::destination() const
{
  const Tile& res = _d->tilemap->at( _d->destination );
  return res;
}

void Pathway::begin()
{
  _d->directionIt = _d->directionList.begin();
  _d->isReverse = false;
}

void Pathway::rbegin()
{
  _d->directionIt_reverse = _d->directionList.rbegin();
  _d->isReverse = true;
}

constants::Direction Pathway::direction()
{
  if(_d->isReverse )
  {
    if( _d->directionIt_reverse != _d->directionList.rend() )
    {
     constants::Direction inverseDir[] = { noneDirection,
                                           south, southEast,
                                           east,  northEast,
                                           north, northWest,
                                           west,  southWest,
                                           countDirection };
     return inverseDir[ *_d->directionIt_reverse ];
    }
  }
  else
  {
    if( _d->directionIt != _d->directionList.end() )
     return *_d->directionIt;
  }

  return constants::noneDirection;
}

void Pathway::toggleDirection()
{
  if( _d->isReverse )
  {
    _d->isReverse = false;
    _d->directionIt = _d->directionIt_reverse.base();
  }
  else
  {
    _d->isReverse = true;
    _d->directionIt_reverse = Directions::reverse_iterator( _d->directionIt );
  }
}

void Pathway::next()
{
  if( _d->isReverse )
  {
    if (_d->directionIt_reverse == _d->directionList.rend())
    {
      // end of path!
      return;
    }

    /*int direction = (int) *_d->directionIt_reverse;
    if( direction != (int) noneDirection )
    {
      if (direction + 4 < (int) countDirection)
      {
        res = (Direction) (direction+4);
      }
      else
      {
        res = (Direction) (direction-4);
      }
    }*/
    _d->directionIt_reverse++;
  }
  else
  {
    if( _d->directionIt == _d->directionList.end())
    {
      // end of path!
      return;
    }

    _d->directionIt++;
  }
}

bool Pathway::isDestination() const
{
  bool res = false;
  if (_d->isReverse)
  {
#if defined(CAESARIA_PLATFORM_WIN)
    Directions::const_reverse_iterator convItReverse = _d->directionIt_reverse;
    res = (convItReverse == _d->directionList.rend());
#elif defined(CAESARIA_PLATFORM_UNIX)
    res = (_d->directionIt_reverse == _d->directionList.rend());
#endif //CAESARIA_PLATFORM_UNIX
  }
  else
  {
    res = ( _d->directionIt == _d->directionList.end() );
  }

  return res;
}

void Pathway::setNextDirection(Direction direction)
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


  if( !_d->tilemap->isInside( TilePos( _d->destination ) ) )
  {
    Logger::warning( "Destination[%d, %d] out of map", _d->destination.i(), _d->destination.j() );
  }
  else
  {
    _d->tileList.push_back( &_d->tilemap->at( _d->destination ) );
    _d->directionList.push_back(direction);
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
  else if (dI==-1 && dJ==1){direction = northWest; }
  else
  {
    Logger::warning( "Destination[%d, %d] out of map", dI, dJ );
    direction = noneDirection;
  }

  setNextDirection(direction);
}

bool Pathway::contains(Tile &tile)
{
  // search in reverse direction, because usually the last tile matches
  bool res = false;
  for( TilesArray::reverse_iterator itTile = _d->tileList.rbegin();
       itTile != _d->tileList.rend(); ++itTile)
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
  if (_d->origin == NULL)
  {
    Logger::warning( "pathWay is NULL" );
  }
  else
  {
    Logger::warning( "pathWay from [%d,%d] to [%d,%d]",
                     _d->origin->i(), _d->origin->j(), _d->destination.i(), _d->destination.j() );

    std::string strDir = "";
    for( Directions::const_iterator itDir = _d->directionList.begin();
         itDir != _d->directionList.end(); ++itDir)
    {
      Direction direction = *itDir;

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

    Logger::warning( strDir.c_str() );
  }
}

VariantMap Pathway::save() const
{
  VariantMap stream;
  if( length() == 0 ) //not save empty way
  {
    return VariantMap();
  }

  stream[ "startPos" ] = _d->origin->pos();
  stream[ "stopPos" ] = _d->destination;

  VariantList directions;
  for( Directions::const_iterator itDir = _d->directionList.begin();
       itDir != _d->directionList.end(); ++itDir)
  {
    directions.push_back( (int)*itDir );
  }

  stream[ "directions" ] = directions;
  stream[ "reverse" ] = _d->isReverse;
  stream[ "step" ] = curStep();

  return stream;
}

bool Pathway::isValid() const { return length() > 0; }

void Pathway::load( const VariantMap& stream )
{
  if( stream.size() == 0 )
  {
    return;
  }

  _d->origin = &_d->tilemap->at( stream.get( "startPos" ).toTilePos() );
  _d->destination = _d->origin->pos(); //stream.get( "stopPos" ).toTilePos();
  VariantList directions = stream.get( "directions" ).toList();
  foreach( it, directions )
  {
    Direction dir = (Direction)(*it).toInt();
    setNextDirection( dir );
  }

  _d->isReverse = stream.get( "reverse" ).toBool();
  int off = stream.get( "step" ).toInt();
  _d->directionIt = _d->directionList.begin();
  _d->directionIt_reverse = _d->directionList.rbegin();
  std::advance(_d->directionIt_reverse, off);
  std::advance(_d->directionIt, off);
}

Pathway& Pathway::operator=( const Pathway& other )
{
  _d->tilemap             = other._d->tilemap;
  _d->origin              = other._d->origin;
  _d->destination      = other._d->destination;
  _d->directionList       = other._d->directionList;
  _d->directionIt         = _d->directionList.begin();
  _d->directionIt_reverse = _d->directionList.rbegin();
  _d->tileList            = other._d->tileList;

  return *this;
}

Pathway Pathway::copy(unsigned int start, int stop) const
{
  Pathway ret;
  if( start >= length() )
  {
    return ret;
  }

  ret.init( *_d->tilemap, *_d->tileList[ start ] );
  stop = (stop == -1 ? _d->tileList.size() : stop );
  for( int i=start+1; i < stop; i++ )
  {
    ret.setNextTile( *_d->tileList[ i ] );
  }

  return ret;
}

unsigned int Pathway::curStep() const
{
  if(_d->isReverse)
  {
    size_t pos = std::distance<Directions::const_reverse_iterator>(_d->directionList.rbegin(), _d->directionIt_reverse);
    return static_cast<unsigned int>(pos);
  }
  else
  {
    size_t pos = std::distance<Directions::const_iterator>(_d->directionList.begin(), _d->directionIt);
    return static_cast<unsigned int>(pos);
  }
}
