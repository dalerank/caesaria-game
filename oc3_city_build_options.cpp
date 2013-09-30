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

#include "oc3_city_build_options.hpp"
#include "oc3_building_data.hpp"
#include "oc3_foreach.hpp"
#include <map>

static const char* disableAll = "disable_all";

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
    _d->rules[ B_OLIVE_FARM ] = mayBuild; _d->rules[ B_WHEAT_FARM ] = mayBuild;
    _d->rules[ B_FRUIT_FARM ] = mayBuild; _d->rules[ B_VEGETABLE_FARM ] = mayBuild;
    _d->rules[ B_GRAPE_FARM ] = mayBuild; _d->rules[ B_PIG_FARM ] = mayBuild;
  break;

  case BM_RAW_MATERIAL:
    _d->rules[ B_MARBLE_QUARRY ] = mayBuild; _d->rules[ B_IRON_MINE ] = mayBuild;
    _d->rules[ B_TIMBER_YARD ] = mayBuild; _d->rules[ B_CLAY_PIT ] = mayBuild;
  break;

  case BM_FACTORY:
    _d->rules[ B_WINE_WORKSHOP ] = mayBuild; _d->rules[ B_OIL_WORKSHOP ] = mayBuild;
    _d->rules[ B_WEAPONS_WORKSHOP ] = mayBuild; _d->rules[ B_FURNITURE ] = mayBuild;
    _d->rules[ B_POTTERY ] = mayBuild; 
  break;

  default:
  break;
  }
}

void CityBuildOptions::clear()
{
  _d->rules.clear();
}

void CityBuildOptions::load(const VariantMap& options)
{
  if( options.get( "farm" ).toString() == disableAll )
    setIndustryAvaible( BM_FARM, false );

  if( options.get( "raw_material" ).toString() == disableAll )
    setIndustryAvaible( BM_RAW_MATERIAL, false );

  if( options.get( "factory" ).toString() == disableAll)
    setIndustryAvaible( BM_FACTORY, false );

  VariantMap buildings = options.get( "buildings" ).toMap();
  foreach( VariantMap::value_type& item, buildings )
  {
    BuildingType btype = BuildingDataHolder::getType( item.first );
    setBuildingAvailble( btype, item.second.toBool() );
  }
}

CityBuildOptions& CityBuildOptions::operator=(const CityBuildOptions& a)
{
  _d->rules = a._d->rules;

  return *this;
}

bool CityBuildOptions::isBuildingAvailble( const BuildingType type ) const
{
  Impl::BuildingRules::iterator it = _d->rules.find( type );
  return (it != _d->rules.end() ? (*it).second : true);
}
