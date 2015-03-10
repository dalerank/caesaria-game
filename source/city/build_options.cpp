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
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "objects/constants.hpp"
#include "core/tilepos_array.hpp"

using namespace gfx;

namespace city
{

namespace development
{

static const char* disable_all = "disable_all";

struct BuildingRule
{
  object::Type type;
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

  typedef std::set<object::Type> Types;
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
  typedef std::map< object::Type, BuildingRule > BuildingRules;

  BuildingRules rules;
  TilePosArray memPoints;

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

void Options::setBuildingAvailble( const object::Type type, bool mayBuild )
{
  _d->rules[ type ].mayBuild = mayBuild;
}

void Options::setBuildingAvailble( const object::Type start, const object::Type stop, bool mayBuild )
{
  for( object::Type i=start; i <= stop; ++i )
    _d->rules[ i ].mayBuild = mayBuild;
}

bool Options::isBuildingsAvailble( const object::Type start, const object::Type stop ) const
{
  bool mayBuild = false;
  for( object::Type i=start; i <= stop; ++i )
    mayBuild |= _d->rules[ i ].mayBuild;

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
  case development::farm: setBuildingAvailble( object::wheat_farm, object::meat_farm, mayBuild ); break;
  case development::water: setBuildingAvailble( object::reservoir, object::well, mayBuild ); break;
  case development::health: setBuildingAvailble( object::clinic, object::barber, mayBuild ); break;
  case development::raw_material: setBuildingAvailble( object::quarry, object::clay_pit, mayBuild ); break;
  case development::religion: setBuildingAvailble( object::small_ceres_temple, object::oracle, mayBuild ); break;
  case development::factory: setBuildingAvailble( object::wine_workshop, object::pottery_workshop, mayBuild ); break;
  case development::education: setBuildingAvailble( object::school, object::library, mayBuild ); break;
  case development::entertainment: setBuildingAvailble( object::amphitheater, object::chariotSchool, mayBuild ); break;
  case development::administration: setBuildingAvailble( object::senate, object::governorPalace, mayBuild ); break;
  case development::engineering:
    setBuildingAvailble( object::engineering_post, object::wharf, mayBuild );
    setBuildingAvailble( object::plaza, mayBuild );
    setBuildingAvailble( object::garden, mayBuild );
  break;
  case development::security: setBuildingAvailble( object::prefecture, object::fortArea, mayBuild ); break;
  case development::commerce: setBuildingAvailble( object::market, object::warehouse, mayBuild ); break;
  case development::temple: setBuildingAvailble( object::small_ceres_temple, object::small_venus_temple, mayBuild ); break;
  case development::big_temple: setBuildingAvailble( object::big_ceres_temple, object::big_venus_temple, mayBuild ); break;
  case development::all:
  {
    MetaDataHolder::OverlayTypes types = MetaDataHolder::instance().availableTypes();
    foreach( it, types )
      setBuildingAvailble( *it, mayBuild );
  }

  default:
  break;
  }
}

bool Options::isGroupAvailable(const Branch type) const
{
  switch( type )
  {
  case development::farm:         return isBuildingsAvailble( object::wheat_farm, object::meat_farm ); break;
  case development::water:        return isBuildingsAvailble( object::reservoir, object::well ); break;
  case development::health:       return isBuildingsAvailble( object::clinic, object::barber ); break;
  case development::raw_material: return isBuildingsAvailble( object::quarry, object::clay_pit ); break;
  case development::religion:     return isBuildingsAvailble( object::small_ceres_temple, object::oracle ); break;
  case development::factory:      return isBuildingsAvailble( object::wine_workshop, object::pottery_workshop ); break;
  case development::education:    return isBuildingsAvailble( object::school, object::library ); break;
  case development::entertainment:return isBuildingsAvailble( object::amphitheater, object::chariotSchool ); break;
  case development::administration:return isBuildingsAvailble(object::senate, object::governorPalace ); break;
  case development::engineering:  return isBuildingsAvailble( object::engineering_post, object::wharf ); break;
  case development::security:     return isBuildingsAvailble( object::prefecture, object::fortArea ); break;
  case development::commerce:     return isBuildingsAvailble( object::market, object::warehouse ); break;
  case development::temple:       return isBuildingsAvailble( object::small_ceres_temple, object::small_venus_temple ); break;
  case development::big_temple:   return isBuildingsAvailble( object::big_ceres_temple, object::big_venus_temple ); break;
  default:
  break;
  }

  return false;
}

unsigned int Options::getBuildingsQuote(const object::Type type) const
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
    object::Type btype = object::toType( item->first );
    setBuildingAvailble( btype, item->second.toBool() );
  }

  VariantList vl_points = options.get("points").toList();
  _d->memPoints.fromVList( vl_points );
  _d->memPoints.resize( 10 );

  _d->checkDesirability = options.get( "check_desirability", _d->checkDesirability );
  _d->maximumForts = options.get( "maximumForts", _d->maximumForts );
}

VariantMap Options::save() const
{
  VariantMap blds;
  VariantMap quotes;
  foreach( it, _d->rules )
  {
    std::string typeName = object::toString( it->first );
    blds[ typeName ] = it->second.mayBuild;
    quotes[ typeName ] = it->second.quotes;
  }

  VariantMap ret;
  ret[ "buildings" ] = blds;
  ret[ "quotes" ] = quotes;
  ret[ "maximumForts" ] = _d->maximumForts;
  ret[ "check_desirability" ] = _d->checkDesirability;
  ret[ "points" ] = _d->memPoints.toVList();
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

bool Options::isBuildingAvailble(const object::Type type ) const
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
        object::Type ovType = object::toType( bIt->toString() );
        if( ovType != object::unknown )
          branchData.insert( ovType );
      }
    }
  }
}

}//end namespace development

}//end namespace city
