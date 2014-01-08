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

#ifndef __CAESARIA_CITYSERVICE_ANIMALS_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_ANIMALS_H_INCLUDED__

#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"

class CityServiceAnimals : public CityService
{
public:
  static CityServicePtr create( PlayerCityPtr city );

  void update( const unsigned int time );

private:
  CityServiceAnimals();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_CITYSERVICE_ANIMALS_H_INCLUDED__
