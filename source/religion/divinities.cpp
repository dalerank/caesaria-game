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

#include "divinities.hpp"
#include "gfx/picture.hpp"
#include "game/gamedate.hpp"
#include "city/helper.hpp"
#include "objects/farm.hpp"
#include "events/showinfobox.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "walker/fishing_boat.hpp"
#include "good/goodstore.hpp"
#include "objects/warehouse.hpp"

using namespace constants;
using namespace gfx;

namespace religion
{

namespace rome
{

namespace {
  int minMoodByPop[]
}

void RomeDivinity::load(const VariantMap& vm)
{
  if( vm.empty() )
    return;

  _name = vm.get( "name" ).toString();
  _service = ServiceHelper::getType( vm.get( "service" ).toString() );
  _pic = Picture::load( vm.get( "image" ).toString() );
  _relation = (float)vm.get( "relation", 100.f );
  _lastFestival = vm.get( "lastFestivalDate", GameDate::current() ).toDateTime() ;
  _shortDesc = vm.get( "shortDesc" ).toString();

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

void RomeDivinity::assignFestival(int type)
{
  _relation = math::clamp<float>( _relation + type * 10, 0, 100 );
}

VariantMap RomeDivinity::save() const
{
  VariantMap ret;
  ret[ "name" ] = Variant( _name );
  ret[ "service" ] = Variant( ServiceHelper::getName( _service ) );
  ret[ "image" ] = Variant( _pic.name() );
  ret[ "relation" ] = _relation;
  ret[ "lastFestivalDate" ] = _lastFestival;
  ret[ "lastActionDate"] = _lastActionDate;
  ret[ "shortDesc" ] = Variant( _shortDesc );

  return ret;
}

float RomeDivinity::relation() const
{
  int festivalFactor = 12 - std::min( 40, _lastFestival.getMonthToDate( GameDate::current() ) );
  return _relation + festivalFactor;
}

void RomeDivinity::updateRelation(float income, PlayerCityPtr city)
{
  int minMoodbyPop = 50 - math::clamp( city->getPopulation() / 10, 0, 50 );
  _relation = math::clamp<float>( _relation + (income - monthDecrease()) * cityBalanceKoeff, minMoodbyPop, 100 );
}

std::string RomeDivinity::moodDescription() const
{
  if( _moodDescr.empty() )
    return "no_descriptions_divinity_mood";

  int delim = 100 / _moodDescr.size();
  return _moodDescr[ math::clamp<int>( _relation / delim, 0, _moodDescr.size()-1 ) ];
}

RomeDivinity::RomeDivinity()
{
  _relation = 0;
}

void RomeDivinity::setInternalName(const std::string& newName){  setDebugName( newName );}
std::string RomeDivinity::internalName() const{  return getDebugName();}

}//end namespace rome

}//end namespace religion
