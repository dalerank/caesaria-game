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
#include "gfx/helper.hpp"
#include "objects/overlay.hpp"
#include "core/variant_list.hpp"

using namespace gfx;

namespace city
{

namespace development
{

CAESARIA_LITERALCONST(farm)

static const char* disable_all = "disable_all";
enum { maxLimit=999 };

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
  typedef std::map<Branch,Types> Config;
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

class BuildingRules : public std::map< object::Type, BuildingRule >
{
public:
  VariantMap saveRules() const
  {
    VariantMap ret;
    for( auto it : *this )
      ret[ object::toString( it.first ) ] = it.second.mayBuild;

    return ret;
  }

  VariantMap saveQuotes() const
  {
    VariantMap ret;
    for( auto& it : *this )
      ret[ object::toString( it.first ) ] = it.second.quotes;

    return ret;
  }

  void loadRules( const VariantMap& stream )
  {
    for( auto& item : stream )
    {
      object::Type btype = object::findType( item.first );
      (*this)[ btype ].mayBuild = item.second.toBool();
    }
  }

  void loadQuotes( const VariantMap& stream )
  {
    for( auto& item : stream )
    {
      object::Type btype = object::findType( item.first );
      (*this)[ btype ].quotes = item.second.toInt();
    }
  }
};

class Options::Impl
{
public:
  BuildingRules rules;

  bool check_desirability;
  unsigned int maximumForts;
};

Options::Options() : _d( new Impl )
{
  _d->check_desirability = true;
  _d->maximumForts = maxLimit;
}

Options::~Options() {}

void Options::setBuildingAvailable( const object::Type type, bool mayBuild )
{
  _d->rules[ type ].mayBuild = mayBuild;
}

void Options::setBuildingAvailable( const Range& range, bool mayBuild )
{
  for( auto& i : range )
    _d->rules[ i ].mayBuild = mayBuild;
}

bool Options::isBuildingsAvailable(const Range& range) const
{
  bool mayBuild = false;
  for( auto& i : range )
    mayBuild |= _d->rules[ i ].mayBuild;

  return mayBuild;
}

bool Options::isCheckDesirability() const {  return _d->check_desirability; }
unsigned int Options::maximumForts() const { return _d->maximumForts; }

void Options::setGroupAvailable( const development::Branch type, Variant vmb )
{
  if( vmb.isNull() )
    return;

  bool mayBuild = (vmb.toString() != disable_all);
  Range range = Range::fromBranch( type );

  for( auto& i : range )
    setBuildingAvailable( i, mayBuild );
}

bool Options::isGroupAvailable(const Branch type) const
{
  Range range = Range::fromBranch( type );

  if( range.empty() )
    return false;

  return isBuildingsAvailable( range );
}

unsigned int Options::getBuildingsQuote(const object::Type type) const
{
  BuildingRules::const_iterator it = _d->rules.find( type );
  return it != _d->rules.end() ? it->second.quotes : maxLimit;
}

void Options::clear() {  _d->rules.clear(); }

void Options::load(const VariantMap& options)
{
  setGroupAvailable( development::farm,          options.get( literals::farm ) );
  setGroupAvailable( development::raw_material,  options.get( "raw_material" ) );
  setGroupAvailable( development::factory,       options.get( "factory" ) );
  setGroupAvailable( development::water,         options.get( "water" ) );
  setGroupAvailable( development::health,        options.get( "health" ) );
  setGroupAvailable( development::religion,      options.get( "religion" ) );
  setGroupAvailable( development::education,     options.get( "education" ) );
  setGroupAvailable( development::entertainment, options.get( "entertainment" ) );
  setGroupAvailable( development::administration,options.get( "govt" ) );
  setGroupAvailable( development::engineering,   options.get( "engineering" ) );
  setGroupAvailable( development::security,      options.get( "security" ) );
  setGroupAvailable( development::commerce,      options.get( "commerce" ) );

  _d->rules.loadRules( options.get( "buildings" ).toMap() );
  _d->rules.loadQuotes( options.get( "quotes" ).toMap() );
  VARIANT_LOAD_ANYDEF_D( _d, check_desirability, _d->check_desirability, options )
  VARIANT_LOAD_ANYDEF_D( _d, maximumForts, _d->maximumForts, options )
}

VariantMap Options::save() const
{
  VariantMap ret;
  ret[ "buildings" ] = _d->rules.saveRules();
  ret[ "quotes" ] = _d->rules.saveQuotes();
  VARIANT_SAVE_ANY_D( ret, _d, maximumForts )
  VARIANT_SAVE_ANY_D( ret, _d, check_desirability )
  return ret;
}

Options& Options::operator=(const development::Options& a)
{
  _d->rules = a._d->rules;
  _d->check_desirability = a._d->check_desirability;
  _d->maximumForts = a._d->maximumForts;

  return *this;
}

bool Options::isBuildingAvailable(const object::Type type ) const
{
  BuildingRules::iterator it = _d->rules.find( type );
  return (it != _d->rules.end() ? (*it).second.mayBuild : true);
}

Branch findBranch(const std::string& name) { return BranchHelper::instance().findType( name ); }
std::string toString(Branch branch) { return BranchHelper::instance().findName( branch ); }

void loadBranchOptions( vfs::Path filename )
{
  BranchHelper& helper = BranchHelper::instance();
  VariantMap vm = config::load( filename );
  BranchHelper::Config& conf = helper.config;

  for( auto& it : vm )
  {
    Branch branch = helper.findType( it.first );
    if( branch != development::unknown )
    {
      BranchHelper::Types& branchData = conf[ branch ];
      VariantList vmTypes = it.second.toList();

      for( auto& bIt : vmTypes )
      {
        object::Type ovType = object::findType( bIt.toString() );
        if( ovType != object::unknown )
          branchData.insert( ovType );
      }
    }
  }
}

Range Range::fromBranch(const Branch branch)
{
  Range ret;

  BranchHelper::Config& conf = BranchHelper::instance().config;

  if( !conf[ branch ].empty() )
  {
    for( auto& type : conf[ branch ] )
      ret << type;

    return ret;
  }

  return _defaultRange( branch );
}

Range Range::fromSequence(const object::Type start, const object::Type stop)
{
  Range ret;
  for( object::Type i=start; i <= stop; ++i )
    ret << i;

  return ret;
}

Range& Range::operator<<(const object::Type type)
{
  this->insert( type );
  return *this;
}

Range Range::_defaultRange(const Branch branch)
{
  Range ret;
  switch( branch )
  {
  case development::farm: ret = Range::fromSequence( object::wheat_farm, object::meat_farm); break;
  case development::water: ret = Range::fromSequence( object::reservoir, object::well); break;
  case development::health: ret = Range::fromSequence( object::clinic, object::barber); break;
  case development::raw_material: ret = Range::fromSequence( object::quarry, object::clay_pit); break;
  case development::religion: ret = Range::fromSequence( object::small_ceres_temple, object::oracle); break;
  case development::factory: ret = Range::fromSequence( object::wine_workshop, object::pottery_workshop); break;
  case development::education: ret = Range::fromSequence( object::school, object::library ); break;
  case development::entertainment: ret = Range::fromSequence( object::amphitheater, object::chariotSchool ); break;
  case development::administration: ret = Range::fromSequence( object::senate, object::governorPalace ); break;

  case development::engineering:
    ret = Range::fromSequence( object::engineering_post, object::wharf);
    ret << object::plaza;
    ret << object::garden;
    ret << object::roadBlock;
  break;

  case development::security: ret = Range::fromSequence( object::prefecture, object::fortArea ); break;
  case development::commerce: ret = Range::fromSequence( object::market, object::warehouse ); break;
  case development::temple: ret = Range::fromSequence( object::small_ceres_temple, object::small_venus_temple ); break;
  case development::big_temple: ret = Range::fromSequence( object::big_ceres_temple, object::big_venus_temple ); break;
  case development::all:
  {
    object::Types types = MetaDataHolder::instance().availableTypes();
    for( auto& type : types )
      ret.insert( type );
  }
  break;

  default: break;
  }

  return ret;
}

void Options::toggleBuildingAvailable(const object::Type type)
{
  bool isEnabled = isBuildingAvailable( type );
  setBuildingAvailable( type, !isEnabled );
}

}//end namespace development

}//end namespace city
