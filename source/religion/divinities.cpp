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
#include "city/helper.hpp"
#include "objects/farm.hpp"
#include "events/showinfobox.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "walker/fishing_boat.hpp"
#include "good/goodstore.hpp"
#include "objects/warehouse.hpp"
#include "core/utils.hpp"
#include "core/variant_map.hpp"

using namespace constants;
using namespace gfx;

namespace religion
{

namespace rome
{

namespace {
CAESARIA_LITERALCONST(name)
}

void RomeDivinity::load(const VariantMap& vm)
{
  if( vm.empty() )
    return;

  _name = vm.get( lc_name ).toString();
  _service = ServiceHelper::getType( vm.get( "service" ).toString() );
  _pic = Picture::load( vm.get( "image" ).toString() );
  _relation = (float)vm.get( "relation", 100.f );
  _lastFestival = vm.get( "lastFestivalDate", game::Date::current() ).toDateTime() ;

  _shortDesc = vm.get( "shortDesc" ).toString();
  if( _shortDesc.empty() )
  {
    _shortDesc  = utils::format( 0xff, "##%s_desc##", internalName().c_str() );
  }
  _wrathPoints = vm.get( "wrath" );
  _blessingDone = vm.get( "blessingDone" );
  _smallCurseDone = vm.get( "smallCurseDone");
  _needRelation = vm.get( "needRelation" );
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
  ret[ lc_name ] = Variant( _name );
  ret[ "service" ] = Variant( ServiceHelper::getName( _service ) );
  ret[ "image" ] = Variant( _pic.name() );
  ret[ "relation" ] = _relation;
  ret[ "lastFestivalDate" ] = _lastFestival;
  ret[ "shortDesc" ] = Variant( _shortDesc );
  ret[ "wrath" ] = _wrathPoints;
  ret[ "blessingDone" ] = _blessingDone;
  ret[ "smallCurseDone" ] = _smallCurseDone;
  ret[ "needRelation" ] = _needRelation;
  ret[ "effectPoints" ] = _effectPoints;

  return ret;
}

float RomeDivinity::relation() const { return _relation; }
float RomeDivinity::monthDecrease() const { return 0.5f; }
void RomeDivinity::setEffectPoint(int value) { _effectPoints = value; }
int RomeDivinity::wrathPoints() const { return _wrathPoints; }
DateTime RomeDivinity::lastFestivalDate() const { return _lastFestival; }

void RomeDivinity::updateRelation(float income, PlayerCityPtr city)
{
  if( income == -101.f )
  {
    _doWrath( city );
    return;
  }
  else if( income == -102.f )
  {
    _doSmallCurse( city );
    return;
  }
  else if( income == -103.f )
  {
    _doBlessing( city );
    return;
  }

  unsigned int minMood = 50 - math::clamp( city->population() / 10, 0u, 50u );
  int festivalFactor = 12 - std::min( 40, _lastFestival.monthsTo( game::Date::current() ) );
  _needRelation = math::clamp<int>( income + festivalFactor + _effectPoints, minMood, 100 );

  _relation += math::signnum( _needRelation - _relation );

  if( _relation <= 50 )
  {
    _blessingDone = false;
    int wrathDelta = 0;
    if( _relation > 0 && _relation < 10 ) { wrathDelta = 5; }
    else if( _relation >= 10 && _relation < 20 ) { wrathDelta = 2; }
    else if( _relation >= 20 && _relation < 40 ) { wrathDelta = 1; }
    _wrathPoints = math::clamp<int>( _wrathPoints + wrathDelta, 0, 50 );
  }

  if( _relation >= 50 )
  {
    _smallCurseDone = false;
    _wrathPoints = 0;
  }
}

std::string RomeDivinity::moodDescription() const
{
  if( _moodDescr.empty() )
    return "##no_descriptions_divinity_mood##";

  int delim = 100 / _moodDescr.size();
  return _moodDescr[ math::clamp<int>( _relation / delim, 0, _moodDescr.size()-1 ) ];
}

void RomeDivinity::checkAction( PlayerCityPtr city )
{
  if( _relation >= 100 && !_blessingDone )
  {
    _doBlessing( city );
    _blessingDone = true;
    _relation -= 50;
  }
  else if( _wrathPoints >= 20 && !_smallCurseDone && _lastFestival.monthsTo( game::Date::current() ) > 3 )
  {
    _doSmallCurse( city );
    _smallCurseDone = true;
    _wrathPoints = 0;
    _relation += 12;
  }
  else if( _wrathPoints >= 50 && _lastFestival.monthsTo( game::Date::current() ) > 3 )
  {
    _doWrath( city );
    _wrathPoints = 0;
    _relation += 30;
  }
}

RomeDivinity::RomeDivinity()
{
  _relation = 50;
  _needRelation = 50;
  _blessingDone = 0;
  _smallCurseDone = 0;
}

void RomeDivinity::setInternalName(const std::string& newName){  setDebugName( newName );}
std::string RomeDivinity::internalName() const{  return debugName();}

}//end namespace rome

}//end namespace religion
