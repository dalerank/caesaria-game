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

#ifndef __CAESARIA_SPEAR_H_INCLUDED__
#define __CAESARIA_SPEAR_H_INCLUDED__

#include "throwing_weapon.hpp"

class Spear : public ThrowingWeapon
{
public:
  static SpearPtr create( PlayerCityPtr city );
  void setPicInfo( const std::string& rc, unsigned int index );

protected:
  virtual void _onTarget();
  virtual const char* rcGroup() const;
  virtual int _rcStartIndex() const;

private:
  Spear( PlayerCityPtr city );

  std::string _picRc;
  unsigned int _picIndex;
};

#endif //__CAESARIA_SPEAR_H_INCLUDED__
