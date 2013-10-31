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

#ifndef __OPENCAESAR3_CITYSERVICE_FESTIVAL_H_INCLUDED__
#define __OPENCAESAR3_CITYSERVICE_FESTIVAL_H_INCLUDED__

#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "divinity.hpp"
#include "core/variant.hpp"

class CityServiceFestival : public CityService
{
public:
  static CityServicePtr create( CityPtr city );
  static std::string getDefaultName();

  DateTime getLastFestivalDate() const;
  DateTime getNextFestivalDate() const;
  void assignFestival( RomeDivinityType name , int size);

  void update( const unsigned int time );

  VariantMap save() const;
  void load( VariantMap stream );

private:
  CityServiceFestival( CityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_CITYSERVICE_FESTIVAL_H_INCLUDED__
