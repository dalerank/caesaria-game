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

#ifndef __CAESARIA_PATHWAY_H_INCLUDED__
#define __CAESARIA_PATHWAY_H_INCLUDED__

#include "predefinitions.hpp"
#include "core/position.hpp"
#include "core/variant.hpp"
#include "core/scopedptr.hpp"
#include "core/direction.hpp"
#include "core/delegate.hpp"

class TilesArray;
class Tilemap;

class Pathway
{
public:
  Pathway();
  Pathway( const Pathway& copy );

  virtual ~Pathway();

  void init( Tilemap& tilemap, Tile& origin );

  int getLength() const;
  const Tile& getOrigin() const;
  const Tile& getDestination() const;
  bool isReverse() const;
  unsigned int getStep() const;

  void begin();
  void rbegin();
  constants::Direction getNextDirection();
  bool isDestination() const;

  void setNextDirection(constants::Direction direction);
  void setNextTile( const Tile& tile);
  bool contains(Tile& tile);
  const TilesArray& getAllTiles() const;

  void prettyPrint() const;
  void toggleDirection();

  Pathway& operator=(const Pathway& other );

  Pathway copy( int start, int stop=-1) const;

  void load( const VariantMap& stream );
  VariantMap save() const;

  bool isValid() const;
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

bool operator<(const Pathway &v1, const Pathway &v2);
typedef Delegate2< const Tile*, bool& > TilePossibleCondition;

#endif //__CAESARIA_PATHWAY_H_INCLUDED__
