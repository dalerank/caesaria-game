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

#ifndef __CAESARIA_WALL_H_INCLUDED__
#define __CAESARIA_WALL_H_INCLUDED__

#include "building.hpp"

class Wall : public Building
{
public:
  Wall();

  virtual bool build(const city::AreaInfo &info);

  virtual void initTerrain( gfx::Tile& terrain);
  virtual bool canBuild(const city::AreaInfo& areaInfo) const;
  virtual bool isNeedRoad() const;
  virtual void destroy();
  virtual void burn();
  virtual bool isWalkable() const; 

  void updatePicture(PlayerCityPtr city);

  const gfx::Picture& picture( const city::AreaInfo& areaInfo ) const;
};

#endif // __CAESARIA_WALL_H_INCLUDED__
