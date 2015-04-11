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

#include "city/city.hpp"
#include "ceres.hpp"
#include "game/gamedate.hpp"
#include "objects/farm.hpp"
#include "events/showinfobox.hpp"
#include "objects/extension.hpp"
#include "city/statistic.hpp"
#include "core/gettext.hpp"

using namespace gfx;

namespace religion
{

namespace rome
{

DivinityPtr Ceres::create()
{
  DivinityPtr ret( new Ceres() );
  ret->setInternalName( baseDivinityNames[ romeDivCeres ] );
  ret->drop();

  return ret;
}

void Ceres::updateRelation(float income, PlayerCityPtr city)
{
  RomeDivinity::updateRelation( income, city );
}

void Ceres::_doWrath( PlayerCityPtr city )
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##wrath_of_ceres_title##"),
                                                            _("##wrath_of_ceres_description##"),
                                                            events::ShowInfobox::send2scribe,
                                                            "god_ceres");
  event->dispatch();

  FarmList farms = city::statistic::findfarms( city );

  foreach( farm, farms )
  {
    FactoryProgressUpdater::assignTo( ptr_cast<Factory>( *farm ), -8, DateTime::weekInMonth * DateTime::monthsInYear );
  }
}

void Ceres::_doBlessing(PlayerCityPtr city)
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##blessing_of_ceres_title##"),
                                                            _("##blessing_of_ceres_description##") );
  event->dispatch();

  FarmList farms;
  farms << city->overlays();

  foreach(farm, farms )
  {
    (*farm)->updateProgress( 100.f -  (*farm)->progress() );
    FactoryProgressUpdater::assignTo( ptr_cast<Factory>( *farm ), 5, game::Date::days2ticks( 60 ) );
  }
}

void Ceres::_doSmallCurse(PlayerCityPtr city)
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##smallcurse_of_ceres_title##"),
                                                            _("##smallcurse_of_ceres_description##") );
  event->dispatch();

  FarmList farms = city::statistic::findfarms( city );

  foreach( farm, farms )
  {
    (*farm)->updateProgress( -(*farm)->progress() );
  }
}

}//end namespace rome

}//end namespace religion
