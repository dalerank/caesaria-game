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

#ifndef _CAESARIA_TILESARRAY_INCLUDE_H_
#define _CAESARIA_TILESARRAY_INCLUDE_H_

#include "tile.hpp"

#include <vector>
#include <cstring>

namespace gfx
{

class TilesArray
{
private:
	std::vector<Tile*> tiles;
	TilePos _leftUpCorner;
	TilePos _rightDownCorner;
	bool isCornersUpToDate;
public:
	bool isRectangular;

	typedef std::vector<Tile*>::const_iterator const_iterator;
	typedef std::vector<Tile*>::const_reverse_iterator const_reverse_iterator;
	typedef std::vector<Tile*>::iterator iterator;
	typedef std::vector<Tile*>::reverse_iterator reverse_iterator;
	typedef std::vector<Tile*>::size_type size_type;
	typedef std::vector<Tile*>::const_reference const_reference;
	typedef std::vector<Tile*>::reference reference;

	const_iterator begin() const { return tiles.begin(); }
	const_iterator end() const { return tiles.end(); }

	iterator begin() { return tiles.begin(); }
	iterator end() { return tiles.end(); }

	const_reverse_iterator rbegin() const { return tiles.rbegin(); }
	const_reverse_iterator rend() const { return tiles.rend(); }

	reverse_iterator rbegin() { return tiles.rbegin(); }
	reverse_iterator rend() { return tiles.rend(); }

	bool empty() const { return tiles.empty(); }
	void clear() { tiles.clear(); recreateCorners(); }
	void reserve(size_type newSize) { tiles.reserve(newSize); }
	void pop_back() { tiles.pop_back(); isCornersUpToDate = false; }
	void insert(const_iterator where, Tile* tile) { tiles.insert(where, tile); }
	iterator erase(const_iterator it){
		isCornersUpToDate = false;
		return tiles.erase(it);
	}
	size_type size() const { return tiles.size(); }
	const_reference front() const { return tiles.front(); }
	const_reference back() const { return tiles.back(); }
	const_reference operator[] (size_type ind) const { return tiles[ind]; }

  bool contain( TilePos& tilePos )
  {
		const int k = 1000;
		if (isRectangular)
		{
			if (tilePos.i() >= leftUpCorner().i() && tilePos.i() <= rightDownCorner().i()
				&& tilePos.j() <= leftUpCorner().j() && tilePos.j() >= rightDownCorner().j())
			{
				return true;
			}
		}
		else
		{
			if (tiles.size() > k){
				bool b = true;
			}
			foreach(it, tiles)
			{
				if ((*it)->pos() == tilePos)
					return true;
			}
		}



		return false;
  }

	TilesArray()
	{
	  isRectangular = false;
		_leftUpCorner = TilePos(-1, -1);
		_rightDownCorner = TilePos(-1, -1);
		isCornersUpToDate = true;
  }

	TilesArray(size_type size) : TilesArray()
	{
		tiles.reserve(size);
	}

  TilesArray( const TilesArray& a )
  {		
		tiles = a.tiles;
		isRectangular = a.isRectangular;
		isCornersUpToDate = a.isCornersUpToDate;
		_leftUpCorner = a._leftUpCorner;
		_rightDownCorner = a._rightDownCorner;
  }

  TilePos leftUpCorner()
  {
		if (!isCornersUpToDate)
		{
			recreateCorners();
		}

		return _leftUpCorner;
  }

  TilePos rightDownCorner()
  {
		if (!isCornersUpToDate)
		{
			recreateCorners();
		}
		return  _rightDownCorner;
  }

  TilesArray& operator=(const TilesArray& a)
  {
		tiles = a.tiles;
		_leftUpCorner = a._leftUpCorner;
		_rightDownCorner = a._rightDownCorner;
		isRectangular = a.isRectangular;
		if (!a.isCornersUpToDate){
			recreateCorners();
		}
    return *this;
  }

	void recreateCorners()
	{
		if (tiles.empty())
		{
			_leftUpCorner = TilePos(-1, -1);
			_rightDownCorner = TilePos(-1, -1);
			isCornersUpToDate = true;
			return;
		}

		_rightDownCorner = TilePos(0, 9999);
		_leftUpCorner = TilePos(9999, 0);
		foreach(it, tiles)
		{
			const TilePos& pos = (*it)->pos();
			if (pos.i() < _leftUpCorner.i()) { _leftUpCorner.setI(pos.i()); }
			if (pos.j() > _leftUpCorner.j()) { _leftUpCorner.setJ(pos.j()); }
			if (pos.j() < _rightDownCorner.j()) { _rightDownCorner.setJ(pos.j()); }
			if (pos.i() > _rightDownCorner.i()) { _rightDownCorner.setI(pos.i()); }
		}

		isCornersUpToDate = true;

		int areaBetweenCorners = (_leftUpCorner.j() - _rightDownCorner.j() + 1) * (_rightDownCorner.i() - _leftUpCorner.i() + 1);

		isRectangular = areaBetweenCorners == tiles.size();
	}

	void updateCorners(TilePos& pos)
	{
		if (pos.i() < _leftUpCorner.i()) { _leftUpCorner.setI(pos.i()); }
		if (pos.j() > _leftUpCorner.j()) { _leftUpCorner.setJ(pos.j()); }
		if (pos.j() < _rightDownCorner.j()) { _rightDownCorner.setJ(pos.j()); }
		if (pos.i() > _rightDownCorner.i()) { _rightDownCorner.setI(pos.i()); }
  }

	TilesArray& append(Tile* tile)
	{
		tiles.push_back(tile);
		isCornersUpToDate = false;
		return *this;
	}

  TilesArray& append( const TilesArray& a )
  {
    tiles.insert( tiles.end(), a.tiles.begin(), a.tiles.end() );

		recreateCorners();

    return *this;
  }

	void replace(size_type pos, Tile* tile){
		tiles[pos] = tile;
		isCornersUpToDate = false;
	}

  TilesArray walkableTiles( bool alllands=false ) const
  {
    TilesArray ret;
    foreach( i, tiles)
    {
      if( (*i)->isWalkable( alllands ) )
          ret.tiles.push_back( *i );
    }

    return ret;
  }

	

  TilesArray& remove( TilePos& pos )
  {
    foreach( it, tiles )
    {
      if( (*it)->pos() == pos )
      {
        tiles.erase( it );
				recreateCorners();
				isRectangular = false;
        break;
      }
    }

		isCornersUpToDate = false;

    return *this;
  }

  TileOverlayList overlays() const
  {
    TileOverlayList ret;
    foreach( i, tiles)
    {
      if( (*i)->overlay().isValid() )
        ret << (*i)->overlay();
    }

    return ret;
  }

  Tile* random() const
  {
    return tiles.size() > 0 ? tiles[ math::random( tiles.size() ) ] : 0;
  }
};

}//end namespace

#endif //_CAESARIA_TILESARRAY_INCLUDE_H_
