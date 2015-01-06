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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "tilemap.hpp"

#include "gfx/helper.hpp"
#include "objects/building.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"

using namespace constants;

namespace gfx
{

static Tile invalidTile( TilePos( -1, -1 ) );

class TileRow : public TilesArray
{
public:
  ~TileRow()
  {
    foreach( it, *this )
    {
      delete *it;
    }
  }
};

class TileGrid : public std::vector< TileRow >
{
};

class Tilemap::Impl : public TileGrid
{
public:
  struct TurnInfo {
    Tile* tile;
    Picture pic;
    TileOverlayPtr overlay;
  };

  typedef std::map<Tile*, TurnInfo> MasterTiles;
  TilesArray border;

  int size;  
  Direction direction;
  int virtWidth;

  Tile* ate(const TilePos& pos);
  Tile* ate( const int i, const int j );

  Tile& at( const int i, const int j );

  bool isInside( const TilePos& pos );
  void resize( const int s );
  void set( int i, int j, Tile* v );
  void saveMasterTiles( MasterTiles& mtiles );
  void checkCoastAfterTurn();
};

Tilemap::Tilemap() : _d( new Impl )
{
  _d->size = 0;
  _d->direction = north;
  _d->virtWidth = tilemap::cellSize().width() * 2;
}

void Tilemap::resize( const unsigned int size )
{
  _d->resize( size );
}

bool Tilemap::isInside(const TilePos& pos ) const
{
  return _d->isInside( pos );
}

TilePos Tilemap::fit( const TilePos& pos ) const
{
  TilePos ret;
  ret.setI( math::clamp( pos.i(), 0, _d->size ) );
  ret.setJ( math::clamp( pos.j(), 0, _d->size ) );
  return ret;
}

Tile* Tilemap::at( const Point& pos, bool overborder)
{
  // x relative to the left most pixel of the tilemap
  int i = (pos.x() + 2 * pos.y()) / _d->virtWidth;
  int j = (pos.x() - 2 * pos.y()) / _d->virtWidth;

  if( overborder )
  {
      i = math::clamp( i, 0, size() - 1 );
      j = math::clamp( j, 0, size() - 1 );
  }
  // std::cout << "ij ("<<i<<","<<j<<")"<<std::endl;

  if (i>=0 && j>=0 && i < size() && j < size())
  {
    // valid coordinate
    return &at( TilePos( i, j ) );
  }
  else // the pixel is outside the tilemap => no tile here
  {
    return NULL;
  }
}

TilePos Tilemap::p2tp(const Point &pos)
{
  return TilePos( (pos.x() + 2 * pos.y()) / _d->virtWidth,
                  (pos.x() - 2 * pos.y()) / _d->virtWidth );
}

Tile& Tilemap::at(const int i, const int j) {  return _d->at( i, j );}
const Tile& Tilemap::at(const int i, const int j) const  {  return _d->at( i, j ); }
Tile& Tilemap::at( const TilePos& ij ){  return _d->at( ij.i(), ij.j() ); }

const Tile& Tilemap::at( const TilePos& ij ) const
{
  return const_cast<Tilemap*>( this )->at( ij.i(), ij.j() );
}

TilesArray Tilemap::allTiles() const
{
  TilesArray ret;
  TileGrid& tg = *_d;
  for( int row=0; row < _d->size; row++ )
  {
    ret.append( tg[row] );
  }

  return ret;
}

const TilesArray& Tilemap::borderTiles() const
{
  return _d->border;
}

void Tilemap::addBorder()
{
  if( !_d->border.empty() )
    return;

  Rect r;
  r.addInternalPoint( Tile( TilePos(-1, -1) ).mappos() );
  r.addInternalPoint( Tile( TilePos(0, _d->size+1) ).mappos() );
  r.addInternalPoint( Tile( TilePos(_d->size+1, _d->size+1) ).mappos() );
  r.addInternalPoint( Tile( TilePos(_d->size+1, 0) ).mappos() );

  for( int u=0; u < _d->size/2; u++ )
  {
    for( int i=0; i < _d->size; i++ )
    {
      TilePos tpos[4] = { TilePos( -_d->size/2 + u, _d->size-i ), TilePos( i, -u),
                          TilePos( i, _d->size + _d->size/2 - 1 - u ), TilePos( _d->size + u, _d->size-1-i) };
      Picture pics[4] = { at( 0, _d->size-i ).picture(), at( i, 0 ).picture(),
                          at( i, _d->size -1 ).picture(), at( _d->size-1,  _d->size-1-i).picture() };

      for( int idx=0; idx < 4; idx++ )
      {
        Tile t( tpos[idx] );

        if( r.isPointInside( t.mappos() ) )
        {
          _d->border.push_back( new Tile( tpos[idx] ) );
          _d->border.back()->setPicture( pics[idx] );
        }
      }
    }
  }
}

int Tilemap::size() const {  return _d->size; }

TilesArray Tilemap::getNeighbors(TilePos pos, TileNeighbors type)
{
  TilePos offset(1,1);
  switch (type){
    case AllNeighbors:
      return getRectangle(pos - offset, pos + offset, checkCorners);
    case FourNeighbors:
      return getRectangle(pos - offset, pos + offset, !checkCorners);
  }

  Logger::warning( "CRITICAL: Unexpected type %d in Tilemap::getNeighbors", type );
  return TilesArray();
}

TilesArray Tilemap::getRectangle( TilePos start, TilePos stop, const bool corners /*= true*/ )
{
  TilesArray res;

  int mini = math::min( start.i(), stop.i() );
  int minj = math::min( start.j(), stop.j() );
  int maxi = math::max( start.i(), stop.i() );
  int maxj = math::max( start.j(), stop.j() );
  start = TilePos( mini, minj );
  stop = TilePos( maxi, maxj );

  size_t expected = 2 * ((maxi - mini) + (maxj - minj)) + corners;
  res.reserve(expected);

  int delta_corners = 0;
  if(!corners)
  {
    delta_corners = 1;
  }

  int tmpij = start.i();
  //west side
  for(int j = start.j() + delta_corners; j <= stop.j() - delta_corners; ++j)
  {
    if( isInside( TilePos( tmpij, j  ) ) )
      res.push_back( &at( tmpij, j ));
  }

  tmpij = stop.j();
  for(int i = start.i() + 1; i <= stop.i() - delta_corners; ++i)
  {
    if( isInside( TilePos( i, tmpij  ) ) )
      res.push_back( &at(i, tmpij ));
  }

  tmpij = stop.i();
  for (int j = stop.j() - 1; j >= start.j() + delta_corners; --j)  // corners have been handled already
  {
    if( isInside( TilePos( tmpij, j ) ))
      res.push_back(&at( tmpij, j));
  }

  tmpij = start.j();
  for( int i = stop.i() - 1; i >= start.i() + 1; --i)  // corners have been handled already
  {
    if( isInside( TilePos( i, tmpij )) )
      res.push_back(&at( i, tmpij));
  }

  return res;
}

TilesArray Tilemap::getRectangle( TilePos pos, Size size, const bool corners /*= true */ )
{
  return getRectangle( pos, pos + TilePos( size.width()-1, size.height()-1), corners );
}

TilesArray Tilemap::getRectangle(unsigned int range, TilePos center)
{
  TilePos offset( range, range );
  return getRectangle( center - offset, center + offset );
}

// Get tiles inside of rectangle
TilesArray Tilemap::getArea(const TilePos& start, const TilePos& stop ) const
{
  TilesArray res;
  int expected = math::min((abs(stop.i() - start.i()) + 1) * (abs(stop.j() - start.j()) + 1), 100);
  res.reserve(expected);

  Rect r( start.i(), start.j(), stop.i(), stop.j() );
  r.repair();

  for (int i = r.left(); i <= r.right(); ++i)
  {
    for (int j = r.top(); j <= r.bottom(); ++j)
    {
      if( isInside( TilePos( i, j ) ))
      {
        res.push_back( &( const_cast<Tilemap*>( this )->at( TilePos( i, j ) )) );
      }
    }
  }

  return res;
}

TilesArray Tilemap::getArea(const TilePos& start, const Size& size ) const
{
  return getArea( start, start + TilePos( size.width()-1, size.height()-1 ) );
}

TilesArray Tilemap::getArea(int range, const TilePos& center) const
{
  TilePos offset(range,range);
  return getArea( center - offset, center + offset );
}

void Tilemap::save( VariantMap& stream ) const
{
  // saves the graphics map
  std::vector<long> bitsetInfo;
  std::vector<short> desInfo;
  std::vector<short> idInfo;

  TilesArray tiles = getArea( TilePos( 0, 0 ), Size( _d->size ) );
  foreach( it, tiles )
  {
    Tile* tile = *it;    
    bitsetInfo.push_back( tile::encode( *tile ) );
    desInfo.push_back( tile->param( Tile::pDesirability ) );
    idInfo.push_back( tile->originalImgId() );
  }

  ByteArray baBitset;
  ByteArray baDes;
  ByteArray baId;
  baBitset.resize( bitsetInfo.size() * sizeof(long) );
  baDes.resize( desInfo.size() * sizeof(short) );
  baId.resize( idInfo.size() * sizeof(short) );

  memcpy( baBitset.data(), &bitsetInfo[0], baBitset.size() );
  memcpy( baDes.data(), &desInfo[0], baDes.size() );
  memcpy( baId.data(), &idInfo[0], baId.size() );

  stream[ "bitset" ]       = Variant( baBitset.base64() );
  stream[ "desirability" ] = Variant( baDes.base64() );
  stream[ "imgId" ]        = Variant( baId.base64() );
  VARIANT_SAVE_ANY_D( stream, _d, size );
}

void Tilemap::load( const VariantMap& stream )
{
  std::string bitsetInfo = stream.get( "bitset" ).toString();
  std::string desInfo    = stream.get( "desirability" ).toString();
  std::string idInfo     = stream.get( "imgId" ).toString();

  int size;
  VARIANT_LOAD_ANY( size, stream );

  resize( size );

  ByteArray baImgId = ByteArray::fromBase64( idInfo );
  ByteArray baBitset = ByteArray::fromBase64( bitsetInfo );
  ByteArray baDes = ByteArray::fromBase64( desInfo );

  const long* bitsetAr = (long*)baBitset.data();
  const short* imgIdAr = (short*)baImgId.data();
  const short* desAr = (short*)baDes.data();

  if( baBitset.empty() || baImgId.empty() || baDes.empty() )
  {
    Logger::warning( "!!! Tilemap::load data's array is null %d/%d/%d", baBitset.size(), baImgId.size(), baDes.size() );
    return;
  }

  TilesArray tiles = getArea( TilePos( 0, 0 ), Size( _d->size ) );
  int index = 0;
  foreach( it, tiles )
  {
    Tile* tile = *it;

    tile::decode( *tile, bitsetAr[index] );
    tile->setParam( Tile::pDesirability, desAr[index] );

    int imgId = imgIdAr[index];
    if( !tile->masterTile() && imgId != 0 )
    {
      Picture& pic = imgid::toPicture( imgId );

      tile->setOriginalImgId( imgId );

      int tile_size = (pic.width()+2) / _d->virtWidth;  // size of the multi-tile. the multi-tile is a square.

      tile_size = math::clamp<int>( tile_size, 1, 10 );

      // master is the left-most subtile
      Tile* master = (tile_size == 1) ? NULL : tile;

      for ( int di = 0; di<tile_size; ++di )
      {
        // for each subrow of the multi-tile
        for (int dj = 0; dj<tile_size; ++dj)
        {
          // for each subcol of the multi-tile
          Tile &sub_tile = at( tile->pos() + TilePos( di, dj ) );
          sub_tile.setMasterTile( master );
          sub_tile.setPicture( pic );
        }
      }
    }
    index++;
  }
}

void Tilemap::turnRight()
{
  switch( _d->direction )
  {
  case north: _d->direction = west; break;
  case west: _d->direction = south; break;
  case south: _d->direction = east; break;
  case east: _d->direction = north; break;

  default:
    Logger::warning( "Tilemap::turnRight wrong direction %d", _d->direction );
  }

  Impl::MasterTiles masterTiles;
  _d->saveMasterTiles( masterTiles );

  const int& msize = _d->size;
  Tile* tmp;
  for( int i=0; i < msize/2;i++)
  {
    for( int j=i; j < msize-1-i;j++)
    {
      tmp = _d->ate( i, j );
      _d->set( i, j, _d->ate( msize -j-1, i ) );
      _d->set( msize-j-1, i, _d->ate( msize-i-1, msize-j-1 ) );
      _d->set( msize-i-1, msize-j-1, _d->ate( j, msize-i-1 ) );
      _d->set( j, msize-i-1, tmp );
    }
  }  

  foreach( it, masterTiles )
  {    
    const Impl::TurnInfo& ti = it->second;

    Picture pic = ti.overlay.isValid() ? ti.overlay->picture() : ti.pic;
    int pSize = (pic.width() + 2) / _d->virtWidth;

    pSize = math::clamp<int>( pSize, 1, 10 );

    TilePos mTilePos = ti.tile->epos() - TilePos( 0, pSize - 1 );
    Tile* mTile = _d->ate( mTilePos );
    for( int i=0; i < pSize; i++ )
    {
      for( int j=0; j < pSize; j++ )
      {
        Tile* apTile = _d->ate( mTilePos + TilePos( i, j ) );
        apTile->setMasterTile( mTile );
      }
    }

    mTile->setPicture( ti.pic );
    ti.tile->changeDirection( mTile, _d->direction );
  }

  _d->checkCoastAfterTurn();
}

void Tilemap::turnLeft()
{
  switch( _d->direction )
  {
  case north: _d->direction = east; break;
  case east: _d->direction = south; break;
  case south: _d->direction = west; break;
  case west: _d->direction = north; break;

  default:
    Logger::warning( "Tilemap::turnLeft wrong direction %d", _d->direction );
  }

  Impl::MasterTiles masterTiles;
  _d->saveMasterTiles( masterTiles );

  unsigned int size = _d->size;
  Tile* tmp;
  for( unsigned int i=0;i<size/2;i++)
  {
    for( unsigned int j=i;j<size-1-i;j++)
    {
      tmp = _d->ate( i, j );
      _d->set( i, j, _d->ate( j, size-1-i ) );
      _d->set( j, size-1-i, _d->ate( size-1-i, size-1-j ) );
      _d->set( size-1-i, size-1-j, _d->ate( size-1-j, i ) );
      _d->set( size-1-j, i, tmp );
    }
  }

  foreach( it, masterTiles )
  {
    const Impl::TurnInfo& ti = it->second;

    Picture pic = ti.overlay.isValid() ? ti.overlay->picture() : ti.pic;
    int pSize = (pic.width() + 2) / _d->virtWidth;

    pSize = math::clamp<int>( pSize, 1, 10);

    TilePos mTilePos = ti.tile->epos() - TilePos( pSize - 1, 0 );
    Tile* mTile = _d->ate( mTilePos );
    for( int i=0; i < pSize; i++ )
    {
      for( int j=0; j < pSize; j++ )
      {
        Tile* apTile = _d->ate( mTilePos + TilePos( i, j ) );
        apTile->setMasterTile( mTile );
      }
    }

    mTile->setPicture( ti.pic );
    ti.tile->changeDirection( mTile, _d->direction );
  }

  _d->checkCoastAfterTurn();
}

Direction Tilemap::direction() const { return _d->direction; }

Tilemap::~Tilemap(){}

Tile* Tilemap::Impl::ate(const TilePos& pos )
{
  return ate( pos.i(), pos.j() );
}

Tile* Tilemap::Impl::ate(const int i, const int j)
{
  if( isInside( TilePos( i, j ) ) )
  {
    return (*this)[i][j];
  }

  return 0;
}

Tile& Tilemap::Impl::at(const int i, const int j)
{
  if( isInside( TilePos( i, j ) ) )
  {
    return *(*this)[i][j];
  }

  //Logger::warning( "Need inside point current=[%d, %d]", i, j );
  return invalidTile;
}

bool Tilemap::Impl::isInside(const TilePos& pos)
{
  return( pos.i() >= 0 && pos.j()>=0 && pos.i() < size && pos.j() < size);
}

void Tilemap::Impl::resize(const int s)
{
  size = s;

  // resize the tile array
  TileGrid::resize( size );

  for( int i = 0; i < size; ++i )
  {
    (*this)[i].reserve( size );

    for (int j = 0; j < size; ++j)
    {
      (*this)[i].push_back( new Tile( TilePos( i, j ) ));
    }
  }
}

void Tilemap::Impl::set(int i, int j, Tile* v)
{
  v->setEPos( TilePos( i, j ) );
  (*this)[i][j] = v;
}

void Tilemap::Impl::saveMasterTiles(Tilemap::Impl::MasterTiles &mtiles)
{
  Tile* tmp;

  for( int i=0; i < size; i++ )
  {
    for( int j=0; j < size; j++ )
    {
      tmp = ate( i, j );
      Tile* masterTile = tmp->masterTile();

      if( masterTile )
      {        
        Impl::MasterTiles::iterator mtFound = mtiles.find( masterTile );

        if( mtFound == mtiles.end() )
        {
          Impl::TurnInfo ti;
          ti.tile = masterTile;
          ti.pic = ti.tile ->picture();
          ti.overlay = tmp->overlay();

          mtiles[ masterTile ] = ti;

          int pSize = (ti.pic.width() + 2) / virtWidth;

          pSize = math::clamp<int>(  pSize, 1, 10 );

          for( int i=0; i < pSize; i++ )
          {
            for( int j=0; j < pSize; j++ )
            {
              Tile* apTile = ate( ti.tile->epos() + TilePos( i, j ) );
              apTile->setMasterTile( 0 );
              apTile->setPicture( Picture::getInvalid() );
            }
          }
        }
      }
    }
  }
}

void Tilemap::Impl::checkCoastAfterTurn()
{
  for( int i=0; i < size; i++ )
  {
    for( int j=0; j < size; j++ )
    {
      Tile* tmp = ate( i, j );
      if( tmp->getFlag( Tile::tlWater ) )
        tmp->changeDirection( 0, direction );
    }
  }
}

}//end namespace gfx
