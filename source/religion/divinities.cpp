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
#include "events/event.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"

using namespace constants;

namespace religion
{

void RomeDivinityBase::load(const VariantMap& vm)
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
}

void RomeDivinityBase::assignFestival(int type)
{
  _relation = math::clamp<float>( _relation + type * 10, 0, 100 );
}

VariantMap RomeDivinityBase::save() const
{
  VariantMap ret;
  ret[ "name" ] = Variant( _name );
  ret[ "service" ] = Variant( ServiceHelper::getName( _service ) );
  ret[ "image" ] = Variant( _pic.getName() + ".png" );
  ret[ "relation" ] = _relation;
  ret[ "lastFestivalDate" ] = _lastFestival;
  ret[ "lastActionDate"] = _lastActionDate;
  ret[ "shortDesc" ] = Variant( _shortDesc );

  return ret;
}

void RomeDivinityBase::updateRelation(float income, PlayerCityPtr city)
{
  CityHelper helper( city );
  float cityBalanceKoeff = helper.getBalanceKoeff();

  _relation = math::clamp<float>( _relation + income - getDefaultDecrease() * cityBalanceKoeff, 0, 100 );
}

std::string RomeDivinityBase::getMoodDescription() const
{
  if( _moodDescr.empty() )
    return "no_descriptions_divinity_mood";

  int delim = 100 / _moodDescr.size();
  return _moodDescr[ _relation / delim ];
}

RomeDivinityBase::RomeDivinityBase()
{
}

void RomeDivinityBase::setInternalName(const std::string& newName)
{
  ReferenceCounted::setDebugName( newName );
}

RomeDivinityPtr RomeDivinityCeres::create()
{
  RomeDivinityPtr ret( new RomeDivinityCeres() );
  ret->setInternalName( divNames[ romeDivCeres ] );
  ret->drop();

  return ret;
}

void RomeDivinityCeres::updateRelation(float income, PlayerCityPtr city)
{
  RomeDivinityBase::updateRelation( income, city );

  if( getRelation() < 1 && _lastActionDate.getMonthToDate( GameDate::current() ) > 10 )
  {
    _lastActionDate = GameDate::current();
    events::GameEventPtr event = events::ShowInfoboxEvent::create( _("##wrath_of_ceres_title##"),
                                                                   _("##wrath_of_ceres_description##") );
    event->dispatch();

    CityHelper helper( city );
    FarmList farms = helper.find<Farm>( building::any );

    foreach( farm, farms ) { (*farm)->updateProgress( -(*farm)->getProgress() ); }
  }
}

}//end namespace religion
