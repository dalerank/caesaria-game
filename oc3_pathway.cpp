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

#include "oc3_pathway.hpp"
#include "oc3_tile.hpp"
#include "oc3_tilemap.hpp"

bool operator<(const PathWay &v1, const PathWay &v2)
{
  if (v1.getLength()!=v2.getLength())
  {
    return v1.getLength() < v2.getLength();
  }

  // compare memory address
  return (&v1 < &v2);
}

class PathWay::Impl
{
public:
};

PathWay::PathWay()
{
  _origin = NULL;
  _destination = TilePos( 0, 0 );
  _isReverse = false;
}

PathWay::~PathWay()
{

}

PathWay::PathWay(const PathWay &copy)
{
  *this = copy;
}

void PathWay::init( const Tilemap &tilemap, const Tile &origin)
{
  _tilemap = &tilemap;
  _origin = &origin;
  _destination = origin.getIJ();
  _directionList.clear();
  _directionIt = _directionList.begin();
  _directionIt_reverse = _directionList.rbegin();
  _tileList.clear();
  _tileList.push_back(&origin);
}

int PathWay::getLength() const
{
  // TODO: various lands have various travel time (road easier to travel than open country)
  return _directionList.size();
}

const Tile& PathWay::getOrigin() const
{
  return *_origin;
}

const Tile& PathWay::getDestination() const
{
  const Tile& res = _tilemap->at( _destination );
  return res;
}

bool PathWay::isReverse() const
{
  return _isReverse;
}

void PathWay::begin()
{
  _directionIt = _directionList.begin();
  _isReverse = false;
}

void PathWay::rbegin()
{
  _directionIt_reverse = _directionList.rbegin();
  _isReverse = true;
}

void PathWay::toggleDirection()
{
  if( _isReverse )
  {
    _isReverse = false;
    _directionIt = _directionIt_reverse.base();
  }
  else
  {
    _isReverse = true;
    _directionIt_reverse = Directions::reverse_iterator( _directionIt );
  }
}

DirectionType PathWay::getNextDirection()
{
  DirectionType res;
  if (_isReverse)
  {
    if (_directionIt_reverse == _directionList.rend())
    {
      // end of path!
      return D_NONE;
    }
    int direction = (int) *_directionIt_reverse;
    if (direction != (int) D_NONE)
    {
      if (direction + 4 < (int) D_MAX)
      {
        res = (DirectionType) (direction+4);
      }
      else
      {
        res = (DirectionType) (direction-4);
      }
    }
    _directionIt_reverse ++;
  }
  else
  {
    if (_directionIt == _directionList.end())
    {
      // end of path!
      return D_NONE;
    }
    res = *_directionIt;
    _directionIt ++;
  }


  return res;
}

bool PathWay::isDestination() const
{
  bool res;
  if (_isReverse)
  {
#if defined(OC3_PLATFORM_WIN)
    std::vector<DirectionType>::const_reverse_iterator convItReverse = _directionIt_reverse;
    res = (convItReverse == _directionList.rend());
#elif defined(OC3_PLATFORM_UNIX)
    res = (_directionIt_reverse == _directionList.rend());
#endif //OC3_PLATFORM_UNIX
  }
  else
  {
    res = (_directionIt == _directionList.end());
  }

  return res;
}

void PathWay::setNextDirection(const DirectionType direction)
{
  switch (direction)
  {
  case D_NORTH:
    _destination += TilePos( 0, 1 );
    break;
  case D_NORTH_EAST:
    _destination += TilePos( 1, 1 );
    break;
  case D_EAST:
    _destination += TilePos( 1, 0 );
    break;
  case D_SOUTH_EAST:
    _destination += TilePos( 1, -1 );
    break;
  case D_SOUTH:
    _destination += TilePos( 0, -1 );
    break;
  case D_SOUTH_WEST:
    _destination += TilePos( -1, -1 );
    break;
  case D_WEST:
    _destination += TilePos( -1, 0 );
    break;
  case D_NORTH_WEST:
    _destination += TilePos( -1, 1 );
    break;
  default:
    _OC3_DEBUG_BREAK_IF( "Unexpected Direction:" || direction);
    break;
  }

  _OC3_DEBUG_BREAK_IF( !_tilemap->isInside( TilePos( _destination ) ) && "Destination is out of range");
  _tileList.push_back( &_tilemap->at( _destination ) );

  _directionList.push_back(direction);
}

void PathWay::setNextTile( const Tile& tile)
{
  int dI = tile.getI() - _destination.getI();
  int dJ = tile.getJ() - _destination.getJ();

  DirectionType direction;

  if (dI==0 && dJ==0)
  {
    direction = D_NONE;
  }
  else if (dI==0 && dJ==1)
  {
    direction = D_NORTH;
  }
  else if (dI==1 && dJ==1)
  {
    direction = D_NORTH_EAST;
  }
  else if (dI==1 && dJ==0)
  {
    direction = D_EAST;
  }
  else if (dI==1 && dJ==-1)
  {
    direction = D_SOUTH_EAST;
  }
  else if (dI==0 && dJ==-1)
  {
    direction = D_SOUTH;
  }
  else if (dI==-1 && dJ==-1)
  {
    direction = D_SOUTH_WEST;
  }
  else if (dI==-1 && dJ==0)
  {
    direction = D_WEST;
  }
  else if (dI==-1 && dJ==1)
  {
    direction = D_NORTH_WEST;
  }
  else
  {
    _OC3_DEBUG_BREAK_IF( "Unexpected tile, deltaI:" );
  }

  setNextDirection(direction);
}

bool PathWay::contains(Tile &tile)
{
  // search in reverse direction, because usually the last tile matches
  bool res = false;
  for( ConstPtrTilesList::reverse_iterator itTile = _tileList.rbegin(); itTile != _tileList.rend(); ++itTile)
  {
    if (*itTile == &tile)
    {
      res = true;
      break;
    }
  }

  return res;
}

ConstPtrTilesList& PathWay::getAllTiles()
{
  return _tileList;
}

void PathWay::prettyPrint() const
{
  if (_origin == NULL)
  {
    std::cout << "pathWay is NULL" << std::endl;
  }
  else
  {
    std::cout << "pathWay from (" << _origin->getI() << ", " << _origin->getJ() 
      << ") to (" << _destination.getI() << ", " << _destination.getJ() << "): ";
    for (std::vector<DirectionType>::const_iterator itDir = _directionList.begin(); itDir != _directionList.end(); ++itDir)
    {
      DirectionType direction = *itDir;
      std::string strDir = "";
      switch (direction)
      {
      case D_NORTH:
        strDir = "N";
        break;
      case D_NORTH_EAST:
        strDir = "NE";
        break;
      case D_EAST:
        strDir = "E";
        break;
      case D_SOUTH_EAST:
        strDir = "SE";
        break;
      case D_SOUTH:
        strDir = "S";
        break;
      case D_SOUTH_WEST:
        strDir = "SW";
        break;
      case D_WEST:
        strDir = "W";
        break;
      case D_NORTH_WEST:
        strDir = "NW";
        break;
      default:
        _OC3_DEBUG_BREAK_IF( "Unexpected Direction:" || direction);
        break;
      }
      std::cout << strDir << " ";
    }
    std::cout << std::endl;
  }
}

VariantMap PathWay::save() const
{
  VariantMap stream;
  if( getLength() == 0 ) //not save empty way
  {
    return VariantMap();
  }

  stream[ "startPos" ] = _origin->getIJ();
  stream[ "stopPos" ] = _destination;

  VariantList directions;
  for( Directions::const_iterator itDir = _directionList.begin(); itDir != _directionList.end(); ++itDir)
  {
    directions.push_back( (int)*itDir);
  }

  stream[ "directions" ] = directions;
  stream[ "reverse" ] = _isReverse;
  stream[ "step" ] = getStep();

  return stream;
}

bool PathWay::isValid() const
{
  return getLength() != 0;
}

void PathWay::load( const VariantMap& stream )
{
  if( stream.size() == 0 )
  {
    return;
  }

  _origin = &_tilemap->at( stream.get( "startPos" ).toTilePos() );
  _destination = _origin->getIJ();//stream.get( "stopPos" ).toTilePos();
  VariantList directions = stream.get( "directions" ).toList();
  for( VariantList::iterator it = directions.begin(); it != directions.end(); it++ )
  {
    DirectionType dir = (DirectionType)(*it).toInt();
    setNextDirection( dir );
  }
  _isReverse = stream.get( "reverse" ).toBool();
  int off = stream.get( "step" ).toInt();
  _directionIt = _directionList.begin();
  _directionIt_reverse = _directionList.rbegin();
  std::advance(_directionIt_reverse, off);
  std::advance(_directionIt, off);
}

PathWay& PathWay::operator=( const PathWay& other )
{
  _tilemap             = other._tilemap;
  _origin              = other._origin;
  _destination         = other._destination;
  _directionList       = other._directionList;
  _directionIt         = _directionList.begin();
  _directionIt_reverse = _directionList.rbegin();
  _tileList            = other._tileList;

  return *this;
}

unsigned int PathWay::getStep() const
{
  if (_isReverse)
  {
    size_t pos = std::distance<Directions::const_reverse_iterator>(_directionList.rbegin(), _directionIt_reverse);
    return static_cast<unsigned int>(pos);
  }
  else
  {
    size_t pos = std::distance<Directions::const_iterator>(_directionList.begin(), _directionIt);
    return static_cast<unsigned int>(pos);
  }
}
