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

#include "objects/warehouse.hpp"
#include "mars.hpp"
#include "city/city.hpp"
#include "events/showinfobox.hpp"
#include "game/gamedate.hpp"
#include "core/gettext.hpp"
#include "good/store.hpp"
#include "walker/enemysoldier.hpp"
#include "events/postpone.hpp"
#include "core/saveadapter.hpp"
#include "objects/fort.hpp"
#include "objects/extension.hpp"
#include "city/spirit_of_mars.hpp"
#include "city/statistic.hpp"

using namespace gfx;
using namespace events;
using namespace city;

namespace religion
{

namespace rome
{

void Mars::updateRelation(float income, PlayerCityPtr city)
{  
  RomeDivinity::updateRelation( income, city );
}

object::Type Mars::templeType(Divinity::TempleSize size) const
{
  return size == bigTemple
                    ? BIG_TEMPLE_TYPE(mars)
                    : SML_TEMPLE_TYPE(mars);
}

Mars::Mars()
  : RomeDivinity( RomeDivinity::Mars )
{

}

void Mars::_doWrath(PlayerCityPtr city)
{
  events::dispatch<ShowInfobox>( _("##wrath_of_mars_title##"),
                                 _("##wrath_of_mars_text##"),
                                 true,
                                 "god_mars" );

  VariantMap vm = config::load( ":/mars_wrath.model" );
  events::dispatch<PostponeEvent>( "", vm );
}

void Mars::_doSmallCurse(PlayerCityPtr city)
{  
  FortList forts = city->statistic().objects.find<Fort>();

  std::string text, title;
  if( !forts.empty() )
  {
    title = "##smallcurse_of_mars_title##";
    text = "##smallcurse_of_mars_text##";
    FortPtr fort = forts.random();
    FortCurseByMars::assignTo( fort, 12 );
  }
  else
  {
    title = "##smallcurse_of_mars_failed_title##";
    text = "##smallcurse_of_mars_failed_text##";
  }

  events::dispatch<ShowInfobox>( _(title),
                                 _(text),
                                 true );

}

void Mars::_doBlessing(PlayerCityPtr city)
{
  events::dispatch<ShowInfobox>( _("##spirit_of_mars_title##"),
                                 _("##spirit_of_mars_text##"),
                                 true );

  SrvcPtr spiritOfmars = Srvc::create<SpiritOfMars>( city, 6 );
  spiritOfmars->attach();
}

}//end namespace rome

}//end namespace religion
