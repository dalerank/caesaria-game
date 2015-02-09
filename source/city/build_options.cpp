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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "build_options.hpp"
#include "objects/metadata.hpp"
#include "core/foreach.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "objects/constants.hpp"

using namespace constants;
using namespace gfx;

namespace city
{

namespace development
{

static const char* disable_all = "disable_all";

struct BuildingRule
{
  TileOverlay::Type type;
  bool mayBuild;
  unsigned int quotes;
};

class BranchHelper : public EnumsHelper<Branch>
{
public:
  static BranchHelper& instance()
  {
    static BranchHelper inst;
    return inst;
  }

  typedef std::set<int> Types;
  typedef std::map<Branch, Types> Config;
  Config config;

  BranchHelper() : EnumsHelper<Branch>( unknown )
  {
#define __REG_BR(a) append( a, CAESARIA_STR_EXT(a) );
    __REG_BR( water )
    __REG_BR( health )
    __REG_BR( security )
    __REG_BR( education )
    __REG_BR( engineering )
    __REG_BR( administration )
    __REG_BR( entertainment )
    __REG_BR( commerce )
    __REG_BR( farm )
    __REG_BR( raw_material )
    __REG_BR( factory )
    __REG_BR( religion )
    __REG_BR( temple )
    __REG_BR( big_temple )
    __REG_BR( all )
#undef __REG_BR
  }  
};

class Options::Impl
{
public:
  typedef std::map< TileOverlay::Type, BuildingRule > BuildingRules;
  typedef std::vector< TilePos > MemPoints;

  BuildingRules rules;
  MemPoints memPoints;

  bool checkDesirability;
  unsigned int maximumForts;
};

Options::Options() : _d( new Impl )
{
  _d->checkDesirability = true;
  _d->maximumForts = 999;
  _d->memPoints.resize( 10 );
}

Options::~Options() {}

void Options::setBuildingAvailble( const TileOverlay::Type type, bool mayBuild )
{
  _d->rules[ type ].mayBuild = mayBuild;
}

void Options::setBuildingAvailble( const TileOverlay::Type start, const TileOverlay::Type stop, bool mayBuild )
{
  for( int i=start; i <= stop; i++ )
    _d->rules[ (TileOverlay::Type)i ].mayBuild = mayBuild;
}

bool Options::isBuildingsAvailble( const TileOverlay::Type start, const TileOverlay::Type stop ) const
{
  bool mayBuild = false;
  for( int i=start; i <= stop; i++ )
    mayBuild |= _d->rules[ (TileOverlay::Type)i ].mayBuild;

  return mayBuild;
}

bool Options::isCheckDesirability() const {  return _d->checkDesirability; }
unsigned int Options::maximumForts() const { return _d->maximumForts; }

void Options::setGroupAvailable( const development::Branch type, Variant vmb )
{
  if( vmb.isNull() )
    return;

  bool mayBuild = (vmb.toString() != disable_all);
  switch( type )
  {
  case development::farm: setBuildingAvailble( objects::wheat_farm, objects::meat_farm, mayBuild ); break;
  case development::water: setBuildingAvailble( objects::reservoir, objects::well, mayBuild ); break;
  case development::health: setBuildingAvailble( objects::clinic, objects::barber, mayBuild ); break;
  case development::raw_material: setBuildingAvailble( objects::quarry, objects::clay_pit, mayBuild ); break;
  case development::religion: setBuildingAvailble( objects::small_ceres_temple, objects::oracle, mayBuild ); break;
  case development::factory: setBuildingAvailble( objects::wine_workshop, objects::pottery_workshop, mayBuild ); break;
  case development::education: setBuildingAvailble( objects::school, objects::library, mayBuild ); break;
  case development::entertainment: setBuildingAvailble( objects::amphitheater, objects::chariotSchool, mayBuild ); break;
  case development::administration: setBuildingAvailble( objects::senate, objects::governorPalace, mayBuild ); break;
  case development::engineering:
    setBuildingAvailble( objects::engineering_post, objects::wharf, mayBuild );
    setBuildingAvailble( objects::plaza, mayBuild );
    setBuildingAvailble( objects::garden, mayBuild );
  break;
  case development::security: setBuildingAvailble( objects::prefecture, objects::fortArea, mayBuild ); break;
  case development::commerce: setBuildingAvailble( objects::market, objects::warehouse, mayBuild ); break;
  case development::temple: setBuildingAvailble( objects::small_ceres_temple, objects::small_venus_temple, mayBuild ); break;
  case development::big_temple: setBuildingAvailble( objects::big_ceres_temple, objects::big_venus_temple, mayBuild ); break;
  case development::all: setBuildingAvailble( objects::unknown, objects::typeCount, mayBuild );

  default:
  break;
  }
}

bool Options::isGroupAvailable(const Branch type) const
{
  switch( type )
  {
  case development::farm:         return isBuildingsAvailble( objects::wheat_farm, objects::meat_farm ); break;
  case development::water:        return isBuildingsAvailble( objects::reservoir, objects::well ); break;
  case development::health:       return isBuildingsAvailble( objects::clinic, objects::barber ); break;
  case development::raw_material: return isBuildingsAvailble( objects::quarry, objects::clay_pit ); break;
  case development::religion:     return isBuildingsAvailble( objects::small_ceres_temple, objects::oracle ); break;
  case development::factory:      return isBuildingsAvailble( objects::wine_workshop, objects::pottery_workshop ); break;
  case development::education:    return isBuildingsAvailble( objects::school, objects::library ); break;
  case development::entertainment:return isBuildingsAvailble( objects::amphitheater, objects::chariotSchool ); break;
  case development::administration:return isBuildingsAvailble( objects::senate, objects::governorPalace ); break;
  case development::engineering:  return isBuildingsAvailble( objects::engineering_post, objects::wharf ); break;
  case development::security:     return isBuildingsAvailble( objects::prefecture, objects::fortArea ); break;
  case development::commerce:     return isBuildingsAvailble( objects::market, objects::warehouse ); break;
  case development::temple:       return isBuildingsAvailble( objects::small_ceres_temple, objects::small_venus_temple ); break;
  case development::big_temple:   return isBuildingsAvailble( objects::big_ceres_temple, objects::big_venus_temple ); break;
  default:
  break;
  }

  return false;
}

unsigned int Options::getBuildingsQuote(const TileOverlay::Type type) const
{
  Impl::BuildingRules::const_iterator it = _d->rules.find( type );
  return it != _d->rules.end() ? it->second.quotes : 999;
}

TilePos Options::memPoint(unsigned int index) const
{
  index = math::clamp<unsigned int>( index, 0, _d->memPoints.size()-1 );
  return _d->memPoints[ index ];
}

void Options::setMemPoint(unsigned int index, TilePos point)
{
  index = math::clamp<unsigned int>( index, 0, _d->memPoints.size()-1 );
  _d->memPoints[ index ] = point;
}

void Options::clear() {  _d->rules.clear(); }

void Options::load(const VariantMap& options)
{
  setGroupAvailable( development::farm, options.get( "farm" ) );
  setGroupAvailable( development::raw_material, options.get( "raw_material" ) );
  setGroupAvailable( development::factory, options.get( "factory" ) );
  setGroupAvailable( development::water, options.get( "water" ) );
  setGroupAvailable( development::health, options.get( "health" ) );
  setGroupAvailable( development::religion, options.get( "religion" ) );
  setGroupAvailable( development::education, options.get( "education" ) );
  setGroupAvailable( development::entertainment, options.get( "entertainment" ) );
  setGroupAvailable( development::administration, options.get( "govt" ) );
  setGroupAvailable( development::engineering, options.get( "engineering" ) );
  setGroupAvailable( development::security, options.get( "security" ) );
  setGroupAvailable( development::commerce, options.get( "commerce" ) );

  VariantMap buildings = options.get( "buildings" ).toMap();
  foreach( item, buildings )
  {
    TileOverlay::Type btype = MetaDataHolder::findType( item->first );
    setBuildingAvailble( btype, item->second.toBool() );
  }

  VariantList points = options.get("points").toList();
  unsigned int index=0;
  foreach( it, points )
  {
    setMemPoint( index, it->toTilePos() );
    index++;
  }

  _d->checkDesirability = options.get( "check_desirability", _d->checkDesirability );
  _d->maximumForts = options.get( "maximumForts", _d->maximumForts );
}

VariantMap Options::save() const
{
  VariantMap blds;
  VariantMap quotes;
  foreach( it, _d->rules )
  {
    std::string typeName = MetaDataHolder::findTypename( it->first );
    blds[ typeName ] = it->second.mayBuild;
    quotes[ typeName ] = it->second.quotes;
  }

  VariantList points;
  foreach( it, _d->memPoints )
  {
    points.push_back( *it );
  }

  VariantMap ret;
  ret[ "buildings" ] = blds;
  ret[ "quotes" ] = quotes;
  ret[ "maximumForts" ] = _d->maximumForts;
  ret[ "check_desirability" ] = _d->checkDesirability;
  ret[ "points" ] = points;
  return ret;
}

Options& Options::operator=(const development::Options& a)
{
  _d->rules = a._d->rules;
  _d->checkDesirability = a._d->checkDesirability;
  _d->maximumForts = a._d->maximumForts;
  _d->memPoints = a._d->memPoints;

  return *this;
}

bool Options::isBuildingAvailble(const TileOverlay::Type type ) const
{
  Impl::BuildingRules::iterator it = _d->rules.find( type );
  return (it != _d->rules.end() ? (*it).second.mayBuild : true);
}

Branch toBranch(const std::string& name) { return BranchHelper::instance().findType( name ); }
std::string toString(Branch branch) { return BranchHelper::instance().findName( branch ); }

void loadBranchOptions(const std::string &filename)
{
  BranchHelper& helper = BranchHelper::instance();
  VariantMap vm = config::load( filename );
  BranchHelper::Config& conf = helper.config;

  foreach( it, vm )
  {
    Branch branch = helper.findType( it->first );
    if( branch != development::unknown )
    {
      BranchHelper::Types& branchData = conf[ branch];
      VariantList vmTypes = it->second.toList();

      foreach( bIt, vmTypes )
      {
        TileOverlay::Type ovType = MetaDataHolder::findType( bIt->toString() );
        if( ovType != constants::objects::unknown )
          branchData.insert( ovType );
      }
    }
  }
}

}//end namespace development

}//end namespace city
