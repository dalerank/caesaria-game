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

#include "build_options.hpp"
#include "objects/metadata.hpp"
#include "core/foreach.hpp"
#include "objects/constants.hpp"
#include <map>

using namespace constants;

static const char* disableAll = "disable_all";

class CityBuildOptions::Impl
{
public:
  typedef std::map< TileOverlay::Type, bool > BuildingRules;
  BuildingRules rules;
};

CityBuildOptions::CityBuildOptions() : _d( new Impl )
{

}

CityBuildOptions::~CityBuildOptions()
{

}

void CityBuildOptions::setBuildingAvailble( const TileOverlay::Type type, bool mayBuild )
{
  _d->rules[ type ] = mayBuild;
}

void CityBuildOptions::setIndustryAvaible( const BuildMenuType type, bool mayBuild )
{
  switch( type )
  {
  case BM_FARM:
    _d->rules[ building::oliveFarm ] = mayBuild; _d->rules[ building::wheatFarm ] = mayBuild;
    _d->rules[ building::fruitFarm ] = mayBuild; _d->rules[ building::vegetableFarm ] = mayBuild;
    _d->rules[ building::grapeFarm ] = mayBuild; _d->rules[ building::pigFarm ] = mayBuild;
  break;

  case BM_RAW_MATERIAL:
    _d->rules[ building::marbleQuarry ] = mayBuild; _d->rules[ building::ironMine ] = mayBuild;
    _d->rules[ building::timberLogger ] = mayBuild; _d->rules[ building::clayPit ] = mayBuild;
  break;

  case BM_FACTORY:
    _d->rules[ building::winery ] = mayBuild; _d->rules[ building::creamery ] = mayBuild;
    _d->rules[ building::weaponsWorkshop ] = mayBuild; _d->rules[ building::furnitureWorkshop ] = mayBuild;
    _d->rules[ building::pottery ] = mayBuild;
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
  foreach( item, buildings )
  {
    TileOverlay::Type btype = MetaDataHolder::getType( item->first );
    setBuildingAvailble( btype, item->second.toBool() );
  }
}

CityBuildOptions& CityBuildOptions::operator=(const CityBuildOptions& a)
{
  _d->rules = a._d->rules;

  return *this;
}

bool CityBuildOptions::isBuildingAvailble(const TileOverlay::Type type ) const
{
  Impl::BuildingRules::iterator it = _d->rules.find( type );
  return (it != _d->rules.end() ? (*it).second : true);
}
