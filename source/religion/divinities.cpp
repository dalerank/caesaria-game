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

#include "city/city.hpp"
#include "divinities.hpp"
#include "gfx/picture.hpp"
#include "game/gamedate.hpp"
#include "objects/construction.hpp"
#include "objects/farm.hpp"
#include "events/showinfobox.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "walker/fishing_boat.hpp"
#include "good/store.hpp"
#include "objects/warehouse.hpp"
#include "core/utils.hpp"
#include "core/variant_map.hpp"
#include "core/format.hpp"
#include "city/states.hpp"
#include "core/variant_list.hpp"
#include "config.hpp"

using namespace gfx;

namespace religion
{

namespace {
GAME_LITERALCONST(name)
}

struct RomeDinConfig
{
  RomeDivinity::Type type;
  std::string name;
};

static std::map<RomeDivinity::Type, RomeDinConfig> RomeDinMap = {
  { RomeDivinity::Ceres, { RomeDivinity::Ceres, "ceres" } },
  { RomeDivinity::Mars, { RomeDivinity::Mars, "mars" } },
  { RomeDivinity::Neptune, { RomeDivinity::Neptune, "neptune" } },
  { RomeDivinity::Venus, { RomeDivinity::Venus, "venus" } },
  { RomeDivinity::Mercury, { RomeDivinity::Mercury, "mercury" } }
};

std::string RomeDivinity::findIntName(RomeDivinity::Type type)
{
  auto it = RomeDinMap.find( type );
  return it != RomeDinMap.end() ? it->second.name : "unknown";
}

StringArray RomeDivinity::getIntNames()
{
  StringArray ret;
  for( auto& i : RomeDinMap )
    ret.push_back( i.second.name );

  return ret;
}

std::vector<RomeDivinity::Type> RomeDivinity::getIntTypes()
{
  std::vector<RomeDivinity::Type> ret;
  for( auto& i : RomeDinMap )
    ret.push_back( i.first );

  return ret;
}

void RomeDivinity::load(const VariantMap& vm)
{
  if( vm.empty() )
    return;

  _name = vm.get( literals::name ).toString();
  _service = ServiceHelper::getType( vm.get( "service" ).toString() );
  _pic.load( vm.get( "image" ).toString() );
  _relation.current = (float)vm.get( "relation", relation::neitralMood );
  _lastFestival = vm.get( "lastFestivalDate", game::Date::current() ).toDateTime() ;

  _shortDesc = vm.get( "shortDesc" ).toString();
  if( _shortDesc.empty() )
  {
    _shortDesc  = fmt::format( "##{0}_desc##", internalName() );
  }
  _wrathPoints = vm.get( "wrath" );
  _blessingDone = vm.get( "blessingDone" );
  _smallCurseDone = vm.get( "smallCurseDone");
  _relation.target = vm.get( "needRelation" );
  _effectPoints = vm.get( "effectPoints" );
  _moodDescr.clear();

  Variant value = vm.get( "moodDescription" );
  if( value.isValid() )
  {
    _moodDescr << vm.get( "moodDescription" ).toList();
  }
  else
  {
    _moodDescr << "##god_wrathful##"
               << "##god_irriated##"
               << "##god_veryangry##"
               << "##god_verypoor##"
               << "##god_quitepoor##"
               << "##god_poor##"
               << "##god_displeased##"
               << "##god_indifferent##"
               << "##god_pleased##"
               << "##god_good##"
               << "##god_verygood##"
               << "##god_charmed##"
               << "##god_happy##"
               << "##god_excellent##"
               << "##god_exalted##";
  }
}

std::string RomeDivinity::shortDescription() const { return _shortDesc; }

Service::Type RomeDivinity::serviceType() const { return _service; }

const Picture&RomeDivinity::picture() const { return _pic; }

void RomeDivinity::assignFestival(int type)
{
  //_relation = math::clamp<float>( _relation + type * 10, 0, 100 );
  _lastFestival = game::Date::current();
}

VariantMap RomeDivinity::save() const
{
  VariantMap ret;
  ret[ literals::name ] = Variant( _name );
  ret[ "service" ] = Variant( ServiceHelper::getName( _service ) );
  ret[ "image" ] = Variant( _pic.name() );
  ret[ "relation" ] = _relation.current;
  ret[ "lastFestivalDate" ] = _lastFestival;
  ret[ "shortDesc" ] = Variant( _shortDesc );
  ret[ "wrath" ] = _wrathPoints;
  ret[ "blessingDone" ] = _blessingDone;
  ret[ "smallCurseDone" ] = _smallCurseDone;
  ret[ "needRelation" ] = _relation.target;
  ret[ "effectPoints" ] = _effectPoints;

  return ret;
}

float RomeDivinity::relation() const { return _relation.current; }
float RomeDivinity::monthDecrease() const { return 0.5f; }
void RomeDivinity::setEffectPoint(int value) { _effectPoints = value; }
int RomeDivinity::wrathPoints() const { return _wrathPoints; }
DateTime RomeDivinity::lastFestivalDate() const { return _lastFestival; }

void RomeDivinity::updateRelation(float income, PlayerCityPtr city)
{
  if( income == debug::doWrath )
  {
    _doWrath( city );
    return;
  }
  else if( income == debug::doSmallCurse )
  {
    _doSmallCurse( city );
    return;
  }
  else if( income == debug::doBlessing )
  {
    _doBlessing( city );
    return;
  }

  unsigned int minMood = relation::neitralMood - math::clamp( city->states().population / 10, 0u, 50u );
  int festivalFactor = 12 - std::min( 40, _lastFestival.monthsTo( game::Date::current() ) );
  _relation.target = math::clamp<int>( income + festivalFactor + _effectPoints, minMood, relation::maximum );

  _relation.current += math::signnum( _relation.target - _relation.current );

  if( _relation.current <= relation::neitralMood )
  {
    _blessingDone = false;
    int wrathDelta = 0;
    if( _relation.current > 0 && _relation.current < relation::wrathfull ) { wrathDelta = penalty::wp4wrath; }
    else if( _relation.current >= relation::wrathfull && _relation.current < relation::badmood ) { wrathDelta = penalty::wp4badmood; }
    else if( _relation.current >= relation::badmood && _relation.current < relation::minimum4wrath ) { wrathDelta = penalty::wp4negativeRel; }
    _wrathPoints = math::clamp<int>( _wrathPoints + wrathDelta, 0, penalty::maximum );
  }

  if( _relation.current >= relation::neitralMood )
  {
    _smallCurseDone = false;
    _wrathPoints = 0;
  }
}

std::string RomeDivinity::moodDescription() const
{
  if( _moodDescr.empty() )
    return "##no_descriptions_divinity_mood##";

  int delim = relation::maximum / _moodDescr.size();
  return _moodDescr[ math::clamp<int>( _relation.current / delim, 0, _moodDescr.size()-1 ) ];
}

void RomeDivinity::checkAction( PlayerCityPtr city )
{
  if( _relation.current >= relation::maximum && !_blessingDone )
  {
    _doBlessing( city );
    _blessingDone = true;
    _relation.current -= relation::neitralMood;
  }
  else if( _wrathPoints >= 20 && !_smallCurseDone && _lastFestival.monthsTo( game::Date::current() ) > 3 )
  {
    _doSmallCurse( city );
    _smallCurseDone = true;
    _wrathPoints = 0;
    _relation.current += 12;
  }
  else if( _wrathPoints >= 50 && _lastFestival.monthsTo( game::Date::current() ) > 3 )
  {
    _doWrath( city );
    _wrathPoints = 0;
    _relation.current += 30;
  }
}

RomeDivinity::RomeDivinity(Type type)
{
  _dtype = type;
  _relation.current = relation::neitralMood;
  _relation.target = relation::neitralMood;
  _blessingDone = 0;
  _smallCurseDone = 0;
}

RomeDivinity::Type RomeDivinity::dtype() const { return _dtype; }
void RomeDivinity::setInternalName(const std::string& newName) { setDebugName( newName );}
std::string RomeDivinity::internalName() const { return debugName();}

}//end namespace religion
