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

  bool checkDesirability;
};

CityBuildOptions::CityBuildOptions() : _d( new Impl )
{
  _d->checkDesirability = true;
}

CityBuildOptions::~CityBuildOptions() {}

void CityBuildOptions::setBuildingAvailble( const TileOverlay::Type type, bool mayBuild )
{
  _d->rules[ type ] = mayBuild;
}

void CityBuildOptions::setBuildingAvailble( const TileOverlay::Type start, const TileOverlay::Type stop, bool mayBuild )
{
  for( int i=start; i <= stop; i++ )
    _d->rules[ (TileOverlay::Type)i ] = mayBuild;
}

bool CityBuildOptions::isBuildingsAvailble( const TileOverlay::Type start, const TileOverlay::Type stop ) const
{
  bool available = false;
  for( int i=start; i <= stop; i++ )
    available |= _d->rules[ (TileOverlay::Type)i ];

  return available;
}

bool CityBuildOptions::isCheckDesirability() const {  return _d->checkDesirability; }

void CityBuildOptions::setGroupAvailable( const BuildMenuType type, Variant vmb )
{
  if( vmb.isNull() )
    return;

  bool mayBuild = (vmb.toString() != disableAll);
  switch( type )
  {
  case BM_FARM: setBuildingAvailble( building::wheatFarm, building::pigFarm, mayBuild ); break;
  case BM_WATER: setBuildingAvailble( building::reservoir, building::well, mayBuild ); break;
  case BM_HEALTH: setBuildingAvailble( building::doctor, building::barber, mayBuild ); break;
  case BM_RAW_MATERIAL: setBuildingAvailble( building::marbleQuarry, building::clayPit, mayBuild ); break;
  case BM_RELIGION: setBuildingAvailble( building::templeCeres, building::oracle, mayBuild ); break;
  case BM_FACTORY: setBuildingAvailble( building::winery, building::pottery, mayBuild ); break;
  case BM_EDUCATION: setBuildingAvailble( building::school, building::library, mayBuild ); break;
  case BM_ENTERTAINMENT: setBuildingAvailble( building::amphitheater, building::chariotSchool, mayBuild ); break;
  case BM_ADMINISTRATION: setBuildingAvailble( building::senate, building::governorPalace, mayBuild ); break;
  case BM_ENGINEERING:
    setBuildingAvailble( building::engineerPost, building::wharf, mayBuild );
    setBuildingAvailble( construction::plaza, mayBuild );
    setBuildingAvailble( construction::garden, mayBuild );
  break;
  case BM_SECURITY: setBuildingAvailble( building::prefecture, building::fortArea, mayBuild ); break;
  case BM_COMMERCE: setBuildingAvailble( building::market, building::warehouse, mayBuild ); break;
  case BM_TEMPLE: setBuildingAvailble( building::templeCeres, building::templeVenus, mayBuild ); break;
  case BM_BIGTEMPLE: setBuildingAvailble( building::cathedralCeres, building::cathedralVenus, mayBuild ); break;
  case BM_MAX: setBuildingAvailble( construction::unknown, building::typeCount, mayBuild );

  default:
  break;
  }
}

bool CityBuildOptions::isGroupAvailable(const BuildMenuType type) const
{
  switch( type )
  {
  case BM_FARM:         return isBuildingsAvailble( building::wheatFarm, building::pigFarm ); break;
  case BM_WATER:        return isBuildingsAvailble( building::reservoir, building::well ); break;
  case BM_HEALTH:       return isBuildingsAvailble( building::doctor, building::barber ); break;
  case BM_RAW_MATERIAL: return isBuildingsAvailble( building::marbleQuarry, building::clayPit ); break;
  case BM_RELIGION:     return isBuildingsAvailble( building::templeCeres, building::oracle ); break;
  case BM_FACTORY:      return isBuildingsAvailble( building::winery, building::pottery ); break;
  case BM_EDUCATION:    return isBuildingsAvailble( building::school, building::library ); break;
  case BM_ENTERTAINMENT: return isBuildingsAvailble( building::amphitheater, building::chariotSchool ); break;
  case BM_ADMINISTRATION: return isBuildingsAvailble( building::senate, building::governorPalace ); break;
  case BM_ENGINEERING:  return isBuildingsAvailble( building::engineerPost, building::wharf ); break;
  case BM_SECURITY:     return isBuildingsAvailble( building::prefecture, building::fortArea ); break;
  case BM_COMMERCE:     return isBuildingsAvailble( building::market, building::warehouse ); break;
  case BM_TEMPLE:       return isBuildingsAvailble( building::templeCeres, building::templeVenus ); break;
  case BM_BIGTEMPLE:    return isBuildingsAvailble( building::cathedralCeres, building::cathedralVenus ); break;
  default:
  break;
  }

  return false;
}

void CityBuildOptions::clear() {  _d->rules.clear(); }

void CityBuildOptions::load(const VariantMap& options)
{
  setGroupAvailable( BM_FARM, options.get( "farm" ) );
  setGroupAvailable( BM_RAW_MATERIAL, options.get( "raw_material" ) );
  setGroupAvailable( BM_FACTORY, options.get( "factory" ) );
  setGroupAvailable( BM_WATER, options.get( "water" ) );
  setGroupAvailable( BM_HEALTH, options.get( "health" ) );
  setGroupAvailable( BM_RELIGION, options.get( "religion" ) );
  setGroupAvailable( BM_EDUCATION, options.get( "education" ) );
  setGroupAvailable( BM_ENTERTAINMENT, options.get( "entertainment" ) );
  setGroupAvailable( BM_ADMINISTRATION, options.get( "govt" ) );
  setGroupAvailable( BM_ENGINEERING, options.get( "engineering" ) );
  setGroupAvailable( BM_SECURITY, options.get( "security" ) );
  setGroupAvailable( BM_COMMERCE, options.get( "commerce" ) );

  VariantMap buildings = options.get( "buildings" ).toMap();
  foreach( item, buildings )
  {
    TileOverlay::Type btype = MetaDataHolder::getType( item->first );
    setBuildingAvailble( btype, item->second.toBool() );
  }

  Variant chDes = options.get( "check_desirability" );
  if( chDes.isValid() )
    _d->checkDesirability = (bool)chDes;
}

VariantMap CityBuildOptions::save() const
{
  VariantMap blds;
  foreach( it, _d->rules )
  {
    blds[ MetaDataHolder::getTypename( it->first ) ] = it->second;
  }

  VariantMap ret;
  ret[ "buildings" ] = blds;
  ret[ "check_desirability" ] = _d->checkDesirability;
  return ret;
}

CityBuildOptions& CityBuildOptions::operator=(const CityBuildOptions& a)
{
  _d->rules = a._d->rules;
  _d->checkDesirability = a._d->checkDesirability;

  return *this;
}

bool CityBuildOptions::isBuildingAvailble(const TileOverlay::Type type ) const
{
  Impl::BuildingRules::iterator it = _d->rules.find( type );
  return (it != _d->rules.end() ? (*it).second : true);
}
