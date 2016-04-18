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
#include "objects/infodb.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "objects/constants.hpp"
#include "objects/overlay.hpp"
#include "core/variant_list.hpp"

using namespace gfx;

namespace city
{

namespace development
{

enum { maxLimit=999 };

struct BuildingRule
{
  object::Type type;
  bool mayBuild;
  unsigned int quotes;
};

class BuildingRules : public std::map< object::Type, BuildingRule >
{
public:
  VariantMap saveRules() const
  {
    VariantMap ret;
    for( auto& it : *this )
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
    for( const auto& item : stream )
    {
      object::Type btype = object::toType( item.first );
      (*this)[ btype ].mayBuild = item.second.toBool();
    }
  }

  void loadQuotes( const VariantMap& stream )
  {
    for( const auto& item : stream )
    {
      object::Type btype = object::toType( item.first );
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

bool Options::isCheckDesirability() const {  return _d->check_desirability; }
unsigned int Options::maximumForts() const { return _d->maximumForts; }

unsigned int Options::getBuildingsQuote(const object::Type type) const
{
  BuildingRules::const_iterator it = _d->rules.find( type );
  return it != _d->rules.end() ? it->second.quotes : maxLimit;
}

void Options::setAvailable(bool av)
{
  auto types = object::InfoDB::instance().availableTypes();
  for (auto t : types)
    setBuildingAvailable(t, av);
}

void Options::clear() {  _d->rules.clear(); }

void Options::load(const VariantMap& options)
{
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

bool Options::isBuildingAvailable(const object::Type type) const
{
  BuildingRules::iterator it = _d->rules.find( type );
  return (it != _d->rules.end() ? it->second.mayBuild : false);
}

void Options::toggleBuildingAvailable(const object::Type type)
{
  bool isEnabled = isBuildingAvailable( type );
  setBuildingAvailable( type, !isEnabled );
}

}//end namespace development

}//end namespace city
