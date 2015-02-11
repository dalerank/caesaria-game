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
#include "city/helper.hpp"
#include "mars.hpp"
#include "events/showinfobox.hpp"
#include "game/gamedate.hpp"
#include "core/gettext.hpp"
#include "good/goodstore.hpp"
#include "walker/enemysoldier.hpp"
#include "events/postpone.hpp"
#include "core/saveadapter.hpp"
#include "objects/fort.hpp"
#include "game/settings.hpp"
#include "objects/extension.hpp"

using namespace constants;
using namespace gfx;

namespace religion
{

namespace rome
{

DivinityPtr Mars::create()
{
  DivinityPtr ret( new Mars() );
  ret->setInternalName( baseDivinityNames[ romeDivMars ] );
  ret->drop();

  return ret;
}

void Mars::updateRelation(float income, PlayerCityPtr city)
{  
  RomeDivinity::updateRelation( income, city );
}

void Mars::_doWrath(PlayerCityPtr city)
{
  events::GameEventPtr message = events::ShowInfobox::create( _("##wrath_of_mars_title##"),
                                                            _("##wrath_of_mars_text##"),
                                                            events::ShowInfobox::send2scribe,
                                                            ":/smk/God_Mars.smk" );
  message->dispatch();


  VariantMap vm = config::load( game::Settings::rcpath( "mars_wrath.model" ) );
  events::GameEventPtr barb_attack = events::PostponeEvent::create( "", vm );
  barb_attack->dispatch();
}

void Mars::_doSmallCurse(PlayerCityPtr city)
{  
  city::Helper helper( city );
  FortList forts = helper.find<Fort>( objects::militaryGroup );

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
    title = "##smallcurse_of_mars_title##";
    text = "##smallcurse_of_mars_failed_text##";
  }

  events::GameEventPtr message = events::ShowInfobox::create( _(title),
                                                              _(text),
                                                              events::ShowInfobox::send2scribe );

  message->dispatch();
}

void Mars::_doBlessing(PlayerCityPtr city)
{
  EnemySoldierList enemies;
  enemies << city->walkers( walker::any );

  bool blessingDone = false;
  int step = enemies.size() / 3;
  for( int k=0; k < step; k++ )
  {
    int index = math::random( enemies.size() );
    EnemySoldierList::iterator it = enemies.begin();
    std::advance( it, index );
    (*it)->die();
    blessingDone = true;
  }

  if( blessingDone )
  {
    events::GameEventPtr event = events::ShowInfobox::create( _("##spirit_of_mars_title##"),
                                                              _("##spirit_of_mars_text##"),
                                                              events::ShowInfobox::send2scribe );
    event->dispatch();
  }
}

}//end namespace rome

}//end namespace religion
