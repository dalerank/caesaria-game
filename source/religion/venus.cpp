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

#include "city/city.hpp"
#include "venus.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "objects/extension.hpp"
#include "core/gettext.hpp"
#include "objects/house.hpp"
#include "city/sentiment.hpp"

using namespace constants;
using namespace gfx;

namespace religion
{

namespace rome
{

DivinityPtr Venus::create()
{
  DivinityPtr ret( new Venus() );
  ret->setInternalName( baseDivinityNames[ romeDivVenus ] );
  ret->drop();

  return ret;
}

void Venus::updateRelation(float income, PlayerCityPtr city)
{
  RomeDivinity::updateRelation( income, city );
}

void Venus::_doWrath( PlayerCityPtr city )
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##wrath_of_venus_title##"),
                                                            _("##wrath_of_venus_description##"),
                                                            events::ShowInfobox::send2scribe,
                                                            ":/smk/God_Venus.smk");
  event->dispatch();

  city::SentimentPtr sentiment;
  sentiment << city->findService( city::Sentiment::defaultName() );

  if( sentiment.isValid() )
  {
    sentiment->addBuff( -75, false, 12 );
  }
}

void Venus::_doBlessing(PlayerCityPtr city)
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##blessing_of_venus_title##"),
                                                            _("##blessing_of_venus_description##") );
  event->dispatch();
}

void Venus::_doSmallCurse(PlayerCityPtr city)
{
  int curseTupe = math::random( 3 );

  events::GameEventPtr e;
  switch( curseTupe )
  {
  case 0:
  {
    e = events::ShowInfobox::create( _("##smcurse_of_venus_title##"),
                                     _("##smcurse_of_venus_description##"),
                                       events::ShowInfobox::send2scribe );
    city::SentimentPtr sentiment;
    sentiment << city->findService( city::Sentiment::defaultName() );
    if( sentiment.isValid() )
    {
      sentiment->addBuff( -math::random( 5 ), true, 12 );
    }
  }
  break;

  case 1:
  {
    e = events::ShowInfobox::create( _("##smcurse_of_venus_title##"),
                                     _("##smcurse2_of_venus_description##"),
                                     events::ShowInfobox::send2scribe );

    HouseList houses;
    houses << city->overlays();

    int rndCount = math::random( houses.size() / 5 );
    for( int i=0; i < rndCount; i++ )
    {
      ConstructionPtr house;
      house << houses.random();
      ConstructionParamUpdater::assignTo( house, House::healthBuff, true, -2, DateTime::weekInMonth * 5 );
    }
  }
  break;
  }

  e->dispatch();
}

}//end namespace rome

}//end namespace religion
