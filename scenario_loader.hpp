// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef SCENARIO_LOADER_HPP
#define SCENARIO_LOADER_HPP

#include "tilemap.hpp"
#include "scenario.hpp"

#include <fstream>
#include <string>

class ScenarioLoader
{
public:
   ScenarioLoader();

   void load(const std::string& filename, Scenario &oScenario);

private:
   void load_map(std::fstream& f, Scenario &oScenario);
   Picture& get_pic_by_id(const int imgId);
   void decode_img(const int imgId, Tile &oTile);
   void decode_terrain(const int terrainBitset, Tile &oTile);
   void init_entry_exit(std::fstream &f, City &ioCity);
   void init_climate(std::fstream &f, City &ioCity);

};


#endif
