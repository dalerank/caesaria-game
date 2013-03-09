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



#include <model_loader.hpp>

#include <house_level.hpp>
#include <iostream>
#include <exception.hpp>
#include <vector>


ModelLoader::ModelLoader()
{
}


void ModelLoader::loadHouseModel(const std::string& filename)
{
   std::ifstream inFile;
   inFile.open(filename.c_str());
   std::string line;

   int linenum = 0;
   while (std::getline (inFile, line))
   {
      linenum++;
      if (line.at(0) != '#')
      {
         // this is not a comment (comments start by #)
         // std::cout << "Line #" << linenum << ":" << line << std::endl;
         std::istringstream linestream(line);

         HouseLevelSpec spec;
         spec._houseLevel = readInt(linestream);
         spec._levelName = readString(linestream);
         spec._maxHabitantsByTile = readInt(linestream);
         readInt(linestream);  // min desirability
         readInt(linestream);  // desirability levelUp
         spec._minEntertainmentLevel = readInt(linestream);
         spec._minWaterLevel = readInt(linestream);
         spec._minReligionLevel = readInt(linestream);
         spec._minEducationLevel = readInt(linestream);
         spec._minHealthLevel = readInt(linestream);
         spec._minFoodLevel = readInt(linestream);
         spec._requiredGoods[G_WHEAT] = 1;  // hard coded ... to be changed!
         spec._requiredGoods[G_POTTERY] = readInt(linestream);  // pottery
         spec._requiredGoods[G_OIL] = readInt(linestream);  // oil
         spec._requiredGoods[G_FURNITURE] = readInt(linestream);  // furniture
         spec._requiredGoods[G_WINE] = readInt(linestream);  // wine
         readInt(linestream);  // crime
         readInt(linestream);  // prosperity
         spec._taxRate = readInt(linestream);  // tax_rate

         HouseLevelSpec::setHouseLevelSpec(spec);
      }
   }
   inFile.close();
}


int ModelLoader::readInt(std::istringstream& stream)
{
   int res;
   std::string str_val;
   std::getline(stream, str_val, ';');
   std::stringstream(str_val) >> res;
   return res;
}

std::string ModelLoader::readString(std::istringstream& stream)
{
   std::string str_val;
   std::getline(stream, str_val, ';');
   return str_val;
}

