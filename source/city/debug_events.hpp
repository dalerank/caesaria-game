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

#ifndef __CAESARIA_CITY_DEBUG_EVENTS_H_INCLUDED__
#define __CAESARIA_CITY_DEBUG_EVENTS_H_INCLUDED__

namespace city
{

namespace debug_event
{

enum {
  add_enemy_archers=0,
  add_enemy_soldiers,
  add_empire_barbarian,
  add_wolves,
  send_mars_wrath,
  win_mission,
  add_1000_dn,
  add_player_money,
  send_chastener,
  test_request,
  send_player_army,
  screenshot,
  send_venus_wrath,
  all_sound_off,
  toggle_grid_visibility,
  toggle_overlay_base,
  toggle_show_path,
  toggle_show_roads,
  toggle_show_object_area,
  add_soldiers_in_fort,
  send_barbarian_to_player
};

}

}//end namespace city
#endif //__CAESARIA_CITY_DEBUG_EVENTS_H_INCLUDED__
