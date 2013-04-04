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



#include <house_level.hpp>

#include <house.hpp>
#include <exception.hpp>
#include <iostream>
#include <gettext.hpp>



std::map<int, HouseLevelSpec> HouseLevelSpec::_spec_by_level;
std::map<int, int> HouseLevelSpec::_level_by_id;


void HouseLevelSpec::init()
{
   std::cout << "HouseLevelSpec INIT" << std::endl;

   _level_by_id.clear();
   _level_by_id[0] = 0;
   _level_by_id[1] = 1;
   _level_by_id[2] = 1;
   _level_by_id[3] = 2;
   _level_by_id[4] = 2;
   _level_by_id[5] = 1; // 2x2
   _level_by_id[6] = 2; // 2x2
   _level_by_id[7] = 3;
   _level_by_id[8] = 3;
   _level_by_id[9] = 4;
   _level_by_id[10] = 4;
   _level_by_id[11] = 3; // 2x2
   _level_by_id[12] = 4; // 2x2
   _level_by_id[13] = 5;
   _level_by_id[14] = 5;
   _level_by_id[15] = 6;
   _level_by_id[16] = 6;
   _level_by_id[17] = 5; // 2x2
   _level_by_id[18] = 6; // 2x2
   _level_by_id[19] = 7;
   _level_by_id[20] = 7;
   _level_by_id[21] = 8;
   _level_by_id[22] = 8;
   _level_by_id[23] = 7; // 2x2
   _level_by_id[24] = 8; // 2x2
   _level_by_id[25] = 9;
   _level_by_id[26] = 9;
   _level_by_id[27] = 10;
   _level_by_id[28] = 10;
   _level_by_id[29] = 9; // 2x2
   _level_by_id[30] = 10; // 2x2
   _level_by_id[31] = 11; // 2x2
   _level_by_id[32] = 11; // 2x2
   _level_by_id[33] = 12; // 2x2
   _level_by_id[34] = 12; // 2x2
   _level_by_id[35] = 13; // 2x2
   _level_by_id[36] = 13; // 2x2
   _level_by_id[37] = 14; // 2x2
   _level_by_id[38] = 14; // 2x2
   _level_by_id[39] = 15; // 3x3
   _level_by_id[40] = 15; // 3x3
   _level_by_id[41] = 16; // 3x3
   _level_by_id[42] = 16; // 3x3
   _level_by_id[43] = 17; // 4x4
   _level_by_id[44] = 17; // 4x4
   _level_by_id[45] = 0;
}

HouseLevelSpec& HouseLevelSpec::getHouseLevelSpec(const int houseLevel)
{
   int level = (std::min)(houseLevel, 17);
   return _spec_by_level[level];
}

void HouseLevelSpec::setHouseLevelSpec(HouseLevelSpec &spec)
{
   _spec_by_level[spec.getHouseLevel()] = spec;
}

int HouseLevelSpec::getHouseLevel(const int houseId)
{
   return _level_by_id[houseId];
}

int HouseLevelSpec::getHouseLevel()
{
   return _houseLevel;
}

std::string& HouseLevelSpec::getLevelName()
{
   return _levelName;
}

bool HouseLevelSpec::isPatrician()
{
   return _houseLevel > 12;
}

int HouseLevelSpec::getMaxHabitantsByTile()
{
   return _maxHabitantsByTile;
}

int HouseLevelSpec::getTaxRate()
{
   return _taxRate;
}

// int HouseLevelSpec::getMinEntertainmentLevel()
// {
//    return _minEntertainmentLevel;
// }
//
// int HouseLevelSpec::getMinEducationLevel()
// {
//    return _minEducationLevel;
// }
//
// int HouseLevelSpec::getMinHealthLevel()
// {
//    return _minHealthLevel;
// }
//
// int HouseLevelSpec::getMinReligionLevel()
// {
//    return _minReligionLevel;
// }
//
// int HouseLevelSpec::getMinWaterLevel()
// {
//    return _minWaterLevel;
// }
//
// int HouseLevelSpec::getMinFoodLevel()
// {
//    return _minFoodLevel;
// }


bool HouseLevelSpec::checkHouse(House &house)
{
   bool res = true;
   int value;
   std::string reason;

   value = computeEntertainmentLevel(house);
   // std::cout << "entertainment=" << value << std::endl;
   if (value < _minEntertainmentLevel)
   {
      res = false;
      std::cout << "missing entertainment" << std::endl;
   }

   value = computeEducationLevel(house, reason);
   // std::cout << "education=" << value << " " << reason << std::endl;
   if (value < _minEducationLevel)
   {
      res = false;
      std::cout << "missing education, " << reason << std::endl;
   }

   value = computeHealthLevel(house, reason);
   // std::cout << "health=" << value << " " << reason << std::endl;
   if (value < _minHealthLevel)
   {
      res = false;
      std::cout << "missing health, " << reason << std::endl;
   }

   value = computeReligionLevel(house);
   // std::cout << "religion=" << value << std::endl;
   if (value < _minReligionLevel)
   {
      res = false;
      std::cout << "missing religion" << std::endl;
   }

   value = computeWaterLevel(house, reason);
   // std::cout << "water=" << value << " " << reason << std::endl;
   if (value < _minWaterLevel)
   {
      res = false;
      std::cout << "missing water, " << reason << std::endl;
   }

   value = computeFoodLevel(house);
   // std::cout << "food=" << value << std::endl;
   if (value < _minFoodLevel)
   {
      res = false;
      std::cout << "missing food" << std::endl;
   }

   if (_requiredGoods[G_POTTERY] != 0 && house.getGoodStore().getCurrentQty(G_POTTERY) == 0)
   {
      res = false;
      std::cout << "missing pottery" << std::endl;
   }

   if (_requiredGoods[G_FURNITURE] != 0 && house.getGoodStore().getCurrentQty(G_FURNITURE) == 0)
   {
      res = false;
      std::cout << "missing furniture" << std::endl;
   }

   if (_requiredGoods[G_OIL] != 0 && house.getGoodStore().getCurrentQty(G_OIL) == 0)
   {
      res = false;
      std::cout << "missing oil" << std::endl;
   }

   return res;
}


int HouseLevelSpec::computeWaterLevel(House &house, std::string &oMissingRequirement)
{
   // no water=0, well=1, fountain=2
   int res = 0;
   if (house.hasServiceAccess(S_FOUNTAIN))
   {
      res = 2;
   }
   else if (house.hasServiceAccess(S_WELL))
   {
      res = 1;
      oMissingRequirement = _("need fountain");
   }
   else
   {
      oMissingRequirement = _("need water");
   }
   return res;
}


int HouseLevelSpec::computeFoodLevel(House &house)
{
   int res = 0;

   GoodStore& goodStore = house.getGoodStore();
   if (goodStore.getCurrentQty(G_WHEAT) > 0)
   {
      res++;
   }
   if (goodStore.getCurrentQty(G_FISH) > 0)
   {
      res++;
   }
   if (goodStore.getCurrentQty(G_MEAT) > 0)
   {
      res++;
   }
   if (goodStore.getCurrentQty(G_FRUIT) > 0)
   {
      res++;
   }
   if (goodStore.getCurrentQty(G_VEGETABLE) > 0)
   {
      res++;
   }

   return res;
}


int HouseLevelSpec::computeEntertainmentLevel(House &house)
{
   int res = 0;
   if (house.hasServiceAccess(S_THEATER))
   {
      res += 10;
   }
   if (house.hasServiceAccess(S_AMPHITHEATER))
   {
      res += 20;
   }
   if (house.hasServiceAccess(S_COLLOSSEUM))
   {
      res += 30;
   }
   if (house.hasServiceAccess(S_HIPPODROME))
   {
      res += 40;
   }
   return res;
}


int HouseLevelSpec::computeHealthLevel(House &house, std::string &oMissingRequirement)
{
   // no health=0, bath=1, bath+doctor/hospital=2, bath+doctor/hospital+barber=3, bath+doctor+hospital+barber=4
   int res = 0;
   if (house.hasServiceAccess(S_BATHS))
   {
      res = 1;

      if (house.hasServiceAccess(S_DOCTOR) || house.hasServiceAccess(S_HOSPITAL))
      {
         res = 2;

         if (house.hasServiceAccess(S_BARBER))
         {
            res = 3;

            if (house.hasServiceAccess(S_DOCTOR) && house.hasServiceAccess(S_HOSPITAL))
            {
               res = 4;
            }
            else
            {
               if (house.hasServiceAccess(S_DOCTOR))
               {
                  oMissingRequirement = _("need hospital");
               }
               else
               {
                  oMissingRequirement = _("need doctor");
               }
            }
         }
         else
         {
            oMissingRequirement = _("need barber");
         }
      }
      else
      {
         oMissingRequirement = _("need doctor or hospital");
      }
   }
   else
   {
      oMissingRequirement = _("need bath");
   }
   return res;
}


int HouseLevelSpec::computeEducationLevel(House &house, std::string &oMissingRequirement)
{
   int res = 0;
   if (house.hasServiceAccess(S_LIBRARY) || house.hasServiceAccess(S_SCHOOL))
   {
      res = 1;

      if (house.hasServiceAccess(S_LIBRARY) && house.hasServiceAccess(S_SCHOOL))
      {
         res = 2;

         if (house.hasServiceAccess(S_COLLEGE))
         {
            res = 3;
         }
         else
         {
            oMissingRequirement = _("need college");
         }
      }
      else if (house.hasServiceAccess(S_SCHOOL))
      {
         oMissingRequirement = _("need library");
      }
      else
      {
         oMissingRequirement = _("need school");
      }
   }
   else
   {
      oMissingRequirement = _("need library or school");
   }

   return res;
}


int HouseLevelSpec::computeReligionLevel(House &house)
{
   int res = 0;
   if (house.hasServiceAccess(S_TEMPLE_MERCURE))
   {
      res++;
   }
   if (house.hasServiceAccess(S_TEMPLE_VENUS))
   {
      res++;
   }
   if (house.hasServiceAccess(S_TEMPLE_MARS))
   {
      res++;
   }
   if (house.hasServiceAccess(S_TEMPLE_NEPTUNE))
   {
      res++;
   }
   if (house.hasServiceAccess(S_TEMPLE_CERES))
   {
      res++;
   }
   return res;
}



float HouseLevelSpec::evaluateServiceNeed(House &house, const ServiceType service)
{
   float res = 0;

   switch (service)
   {
   case S_TEMPLE_CERES:
   case S_TEMPLE_NEPTUNE:
   case S_TEMPLE_MARS:
   case S_TEMPLE_VENUS:
   case S_TEMPLE_MERCURE:
      res = evaluateReligionNeed(house, service);
      break;
   case S_THEATER:
   case S_AMPHITHEATER:
   case S_COLLOSSEUM:
   case S_HIPPODROME:
      res = evaluateEntertainmentNeed(house, service);
      break;
   case S_SCHOOL:
   case S_LIBRARY:
   case S_COLLEGE:
      res = evaluateEducationNeed(house, service);
      break;
   case S_BATHS:
   case S_BARBER:
   case S_DOCTOR:
   case S_HOSPITAL:
      res = evaluateHealthNeed(house, service);
      break;
   case S_FORUM:
      res = 1;
      break;
   default:
      break;
   }

   return res * (100 - house.getServiceAccess(service));
}

float HouseLevelSpec::evaluateEntertainmentNeed(House &house, const ServiceType service)
{
   int houseLevel = house.getLevelSpec().getHouseLevel();
   return (float)getHouseLevelSpec(houseLevel+1)._minEntertainmentLevel;
}

float HouseLevelSpec::evaluateEducationNeed(House &house, const ServiceType service)
{
   float res = 0;
   int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = getHouseLevelSpec(houseLevel+1)._minEducationLevel;
   if (minLevel == 1)
   {
      // need school or library
      if (service != S_COLLEGE)
      {
         res = (float)( 100 - std::max(house.getServiceAccess(S_SCHOOL), house.getServiceAccess(S_LIBRARY)) );
      } 
   }
   else if (minLevel == 2)
   {
      // need school and library
      if (service != S_COLLEGE)
      {
         res = (float)( 100 - house.getServiceAccess(service) );
      }
   }
   else if (minLevel == 3)
   {
      // need school and library and college
      res = (float)( 100 - house.getServiceAccess(service) );
   }
   // std::cout << "education need: " << service << " " << res << std::endl;
   return res;
}

float HouseLevelSpec::evaluateHealthNeed(House &house, const ServiceType service)
{
   float res = 0;
   int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = getHouseLevelSpec(houseLevel+1)._minHealthLevel;
   if (minLevel >= 1 && service == S_BATHS)
   {
      // minLevel>=1  => need baths
      res = (float)( 100 - house.getServiceAccess(service) );
   }
   if (minLevel >= 2 && (service == S_DOCTOR || service == S_HOSPITAL))
   {
      if (minLevel == 4)
      {
         // need doctor and hospital
         res = (float)( 100 - house.getServiceAccess(service) );
      }
      else
      {
         // need doctor or hospital
         res = (float)( 100 - std::max(house.getServiceAccess(S_DOCTOR), house.getServiceAccess(S_HOSPITAL)) );
      }
   }
   if (minLevel >= 3 && service == S_BARBER)
   {
      // minLevel>=3  => need barber
      res = (float)( 100 - house.getServiceAccess(service) );
   }

   return res;
}

float HouseLevelSpec::evaluateReligionNeed(House &house, const ServiceType service)
{
   int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = getHouseLevelSpec(houseLevel+1)._minReligionLevel;

   return (float)minLevel;
}

// float HouseLevelSpec::evaluateFoodNeed(House &house, const ServiceType service)
// {
//    int houseLevel = house.getLevelSpec().getHouseLevel();
//    return getHouseLevelSpec(houseLevel+1)._minFoodLevel;
// }

int HouseLevelSpec::computeMonthlyConsumption(House &house, const GoodType goodType)
{
   int res = 0;
   if (_requiredGoods[goodType] != 0)
   {
      res = house.getMaxHabitants() * _requiredGoods[goodType];
   }

   return res;
}
