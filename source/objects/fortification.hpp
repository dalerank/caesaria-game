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

#ifndef __CAESARIA_FORTIFICATION_H_INCLUDED__
#define __CAESARIA_FORTIFICATION_H_INCLUDED__

#include "wall.hpp"

class Fortification : public Wall
{
public:
  Fortification();
  virtual ~Fortification();

  virtual bool build(PlayerCityPtr city, const TilePos& pos );
  const gfx::Picture& picture(PlayerCityPtr city,
                            TilePos pos,
                            const gfx::TilesArray& tmp ) const;

  int getDirection() const;

  virtual void destroy();
  virtual Point offset( const gfx::Tile &tile, const Point &subpos) const;

  void updatePicture(PlayerCityPtr city);
  bool isTowerEnter() const;
  bool mayPatrol() const;
  virtual bool isFlat() const;

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_FORTIFICATION_H_INCLUDED__
