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
  events::GameEventPtr event = events::ShowInfobox::create( _("##wrath_of_mars_title##"),
                                                            _("##wrath_of_mars_text##"),
                                                            events::ShowInfobox::send2scribe );
  event->dispatch();


}

void Mars::_doSmallCurse(PlayerCityPtr city)
{

}

void Mars::_doBlessing(PlayerCityPtr city)
{
  EnemySoldierList enemies;
  enemies << city->getWalkers( walker::any );

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
