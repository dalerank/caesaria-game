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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "constants.hpp"
#include "game/settings.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"

namespace citylayer
{

static const VariantMap invalidConfig;

class Helper::Impl
{
public:
	typedef std::map<Type, VariantMap> Configs;
	Configs configs;
};
	
Helper& Helper::instance()
{
  static Helper inst;
  return inst;
}

std::string Helper::prettyName(Type t)
{
  std::string typeName = instance().findName( t );
  return "##ovrm_" + typeName + "##";
}

const VariantMap& Helper::getConfig(Type t)
{
  const Impl::Configs& configs = instance()._d->configs;
  Impl::Configs::const_iterator it = configs.find( t );
  return it != configs.end() ? it->second : invalidConfig;
}

Helper::Helper() 
  : EnumsHelper<Type>( count ), _d( new Impl )
{
#define LAYER(a) append( citylayer::a, CAESARIA_STR_EXT(a) );
    LAYER(simple)
    LAYER(water)
    LAYER(fire)
    LAYER(damage)
    LAYER(desirability)
    LAYER(entertainments)
    LAYER(entertainment)
    LAYER(theater)
    LAYER(amphitheater)
    LAYER(colloseum)
    LAYER(hippodrome)
    LAYER(health)
    LAYER(healthAll)
    LAYER(doctor)
    LAYER(hospital)
    LAYER(barber)
    LAYER(baths)
    LAYER(food)
    LAYER(religion)
    LAYER(risks)
    LAYER(crime)
    LAYER(aborigen)
    LAYER(troubles)
    LAYER(educations)
    LAYER(education)
    LAYER(school)
    LAYER(library)
    LAYER(academy)
    LAYER(commerce)
    LAYER(tax)
    LAYER(sentiment)
    LAYER(build)
    LAYER(destroyd)
    LAYER(market)
    LAYER(all)
#undef LAYER

  VariantMap vm = config::load( SETTINGS_RC_PATH(layersOptsModel) );
  foreach( it, vm )
  {
    Type layerType = findType( it->first );
    if( layerType != count )
    {
      _d->configs[ layerType ] = it->second.toMap();
    }
  }
}

}//end namespace citylayer
