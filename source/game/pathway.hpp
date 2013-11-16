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

#ifndef __OPENCAESAR3_PATHWAY_H_INCLUDED__
#define __OPENCAESAR3_PATHWAY_H_INCLUDED__

#include "predefinitions.hpp"
#include "core/position.hpp"
#include "core/variant.hpp"
#include "core/scopedptr.hpp"
#include "core/direction.hpp"

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

  void load( const VariantMap& stream );
  VariantMap save() const;

  bool isValid() const;
private:
  Tilemap* _tilemap;
  Tile* _origin;

  class Impl;
  ScopedPtr< Impl > _d;
};

bool operator<(const Pathway &v1, const Pathway &v2);

#endif //__OPENCAESAR3_PATHWAY_H_INCLUDED__
