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

#ifndef __CAESARIA_THROWING_WEAPON_H_INCLUDED__
#define __CAESARIA_THROWING_WEAPON_H_INCLUDED__

#include "walker.hpp"
#include "core/predefinitions.hpp"

class ThrowingWeapon : public Walker
{
public:
  void toThrow( TilePos src, TilePos dst );

  virtual void timeStep(const unsigned long time);
  virtual const Point& mappos() const;
  virtual void turn(TilePos pos);
  virtual void setPicOffset( Point offset );
  virtual const gfx::Picture& getMainPicture();

  TilePos dstPos() const;
  TilePos startPos() const;

  virtual ~ThrowingWeapon();
protected:
  virtual void _reachedPathway();
  virtual void _onTarget() = 0;
  virtual const char* rcGroup() const = 0;
  virtual int _rcStartIndex() const = 0;

  ThrowingWeapon( PlayerCityPtr city );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_THROWING_WEAPON_H_INCLUDED__
