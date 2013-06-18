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

#include "oc3_build_options.hpp"
#include <map>

class CityBuildOptions::Impl
{
public:
  typedef std::map< BuildingType, bool > BuildingRules;
  BuildingRules rules;
};

CityBuildOptions::CityBuildOptions() : _d( new Impl )
{

}

CityBuildOptions::~CityBuildOptions()
{

}

void CityBuildOptions::setBuildingAvailble( const BuildingType type, bool mayBuild )
{
  _d->rules[ type ] = mayBuild;
}

void CityBuildOptions::setIndustryAvaible( const BuildMenuType type, bool mayBuild )
{
  switch( type )
  {
  case BM_FARM:
    _d->rules[ B_OLIVE ] = mayBuild; _d->rules[ B_WHEAT ] = mayBuild;
    _d->rules[ B_FRUIT ] = mayBuild; _d->rules[ B_VEGETABLE ] = mayBuild;
    _d->rules[ B_GRAPE ] = mayBuild; _d->rules[ B_MEAT ] = mayBuild;
  break;

  case BM_RAW_MATERIAL:
    _d->rules[ B_MARBLE ] = mayBuild; _d->rules[ B_IRON ] = mayBuild;
    _d->rules[ B_TIMBER ] = mayBuild; _d->rules[ B_CLAY_PIT ] = mayBuild;
  break;

  case BM_FACTORY:
    _d->rules[ B_WINE ] = mayBuild; _d->rules[ B_OIL ] = mayBuild;
    _d->rules[ B_WEAPON ] = mayBuild; _d->rules[ B_FURNITURE ] = mayBuild;
    _d->rules[ B_POTTERY ] = mayBuild; 
  break;

  }
}

void CityBuildOptions::clear()
{
  _d->rules.clear();
}

bool CityBuildOptions::isBuildingAvailble( const BuildingType type ) const
{
  Impl::BuildingRules::iterator it = _d->rules.find( type );
  return (it != _d->rules.end() ? (*it).second : true);
}