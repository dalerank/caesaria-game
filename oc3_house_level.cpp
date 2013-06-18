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



#include "oc3_house_level.hpp"

#include "oc3_house.hpp"
#include "oc3_exception.hpp"
#include "oc3_gettext.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_variant.hpp"
#include "oc3_saveadapter.hpp"

typedef std::map<int, HouseLevelSpec> HouseLevelSpecsMap;
typedef std::map<int, int> HouseLevelSpecsEqMap;

int HouseLevelSpec::getHouseLevel()
{
   return _houseLevel;
}

const std::string& HouseLevelSpec::getLevelName() const
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
      StringHelper::debug( 0xff, "missing entertainment" );
   }

   value = computeEducationLevel(house, reason);
   // std::cout << "education=" << value << " " << reason << std::endl;
   if (value < _minEducationLevel)
   {
      res = false;
      StringHelper::debug( 0xff, "missing education, %s", reason.c_str() );
   }

   value = computeHealthLevel(house, reason);
   // std::cout << "health=" << value << " " << reason << std::endl;
   if (value < _minHealthLevel)
   {
      res = false;
      StringHelper::debug( 0xff, "missing health, %s", reason.c_str() );
   }

   value = computeReligionLevel(house);
   // std::cout << "religion=" << value << std::endl;
   if (value < _minReligionLevel)
   {
      res = false;
      StringHelper::debug( 0xff, "missing religion" );
   }

   value = computeWaterLevel(house, reason);
   // std::cout << "water=" << value << " " << reason << std::endl;
   if (value < _minWaterLevel)
   {
      res = false;
      StringHelper::debug( 0xff, "missing water, %s", reason.c_str() );
   }

   value = computeFoodLevel(house);
   // std::cout << "food=" << value << std::endl;
   if (value < _minFoodLevel)
   {
      res = false;
      StringHelper::debug( 0xff, "missing food" );
   }

   if (_requiredGoods[G_POTTERY] != 0 && house.getGoodStore().getCurrentQty(G_POTTERY) == 0)
   {
      res = false;
      StringHelper::debug( 0xff, "missing pottery" );
   }

   if (_requiredGoods[G_FURNITURE] != 0 && house.getGoodStore().getCurrentQty(G_FURNITURE) == 0)
   {
      res = false;
      StringHelper::debug( 0xff, "missing furniture" );
   }

   if (_requiredGoods[G_OIL] != 0 && house.getGoodStore().getCurrentQty(G_OIL) == 0)
   {
      res = false;
      StringHelper::debug( 0xff, "missing oil" );
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
   return (float)HouseSpecHelper::getInstance().getHouseLevelSpec(houseLevel+1)._minEntertainmentLevel;
}

float HouseLevelSpec::evaluateEducationNeed(House &house, const ServiceType service)
{
   float res = 0;
   int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = HouseSpecHelper::getInstance().getHouseLevelSpec(houseLevel+1)._minEducationLevel;
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
   int minLevel = HouseSpecHelper::getInstance().getHouseLevelSpec(houseLevel+1)._minHealthLevel;
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
   int minLevel = HouseSpecHelper::getInstance().getHouseLevelSpec(houseLevel+1)._minReligionLevel;

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

const std::string& HouseLevelSpec::getInternalName() const
{
  return _internalName;
}

class HouseSpecHelper::Impl
{
public:
  typedef std::map<int, HouseLevelSpec> HouseLevels;
  HouseLevels spec_by_level;  // key=houseLevel, value=houseLevelSpec
  std::map<int, int> level_by_id;  // key=houseId, value=houseLevel
};

HouseSpecHelper& HouseSpecHelper::getInstance()
{
  static HouseSpecHelper inst;
  return inst;
}

HouseSpecHelper::HouseSpecHelper() : _d( new Impl )
{
  StringHelper::debug( 0xff, "HouseLevelSpec INIT" );

  _d->level_by_id.clear();
  _d->level_by_id[0] = 0;
  _d->level_by_id[1] = 1;
  _d->level_by_id[2] = 1;
  _d->level_by_id[3] = 2;
  _d->level_by_id[4] = 2;
  _d->level_by_id[5] = 1; // 2x2
  _d->level_by_id[6] = 2; // 2x2
  _d->level_by_id[7] = 3;
  _d->level_by_id[8] = 3;
  _d->level_by_id[9] = 4;
  _d->level_by_id[10] = 4;
  _d->level_by_id[11] = 3; // 2x2
  _d->level_by_id[12] = 4; // 2x2
  _d->level_by_id[13] = 5;
  _d->level_by_id[14] = 5;
  _d->level_by_id[15] = 6;
  _d->level_by_id[16] = 6;
  _d->level_by_id[17] = 5; // 2x2
  _d->level_by_id[18] = 6; // 2x2
  _d->level_by_id[19] = 7;
  _d->level_by_id[20] = 7;
  _d->level_by_id[21] = 8;
  _d->level_by_id[22] = 8;
  _d->level_by_id[23] = 7; // 2x2
  _d->level_by_id[24] = 8; // 2x2
  _d->level_by_id[25] = 9;
  _d->level_by_id[26] = 9;
  _d->level_by_id[27] = 10;
  _d->level_by_id[28] = 10;
  _d->level_by_id[29] = 9; // 2x2
  _d->level_by_id[30] = 10; // 2x2
  _d->level_by_id[31] = 11; // 2x2
  _d->level_by_id[32] = 11; // 2x2
  _d->level_by_id[33] = 12; // 2x2
  _d->level_by_id[34] = 12; // 2x2
  _d->level_by_id[35] = 13; // 2x2
  _d->level_by_id[36] = 13; // 2x2
  _d->level_by_id[37] = 14; // 2x2
  _d->level_by_id[38] = 14; // 2x2
  _d->level_by_id[39] = 15; // 3x3
  _d->level_by_id[40] = 15; // 3x3
  _d->level_by_id[41] = 16; // 3x3
  _d->level_by_id[42] = 16; // 3x3
  _d->level_by_id[43] = 17; // 4x4
  _d->level_by_id[44] = 17; // 4x4
  _d->level_by_id[45] = 0;
}

HouseLevelSpec& HouseSpecHelper::getHouseLevelSpec(const int houseLevel)
{
  int level = (math::clamp)(houseLevel, 0, 17);
  return _d->spec_by_level[level];
}

void HouseSpecHelper::setHouseLevelSpec(HouseLevelSpec &spec)
{
  _d->spec_by_level[ spec.getHouseLevel() ] = spec;
}

int HouseSpecHelper::getHouseLevel(const int houseId)
{
  return _d->level_by_id[houseId];
}

int HouseSpecHelper::getHouseLevel( const std::string& name )
{
  for( Impl::HouseLevels::iterator it=_d->spec_by_level.begin(); it != _d->spec_by_level.end(); it++ )
  {
    if( (*it).second.getInternalName() == name )
    {
      return (*it).second.getHouseLevel();
    }
  }

  return 0;
}

HouseSpecHelper::~HouseSpecHelper()
{

}

void HouseSpecHelper::initialize( const std::string& filename )
{
  VariantMap houses = SaveAdapter::load( filename );

  if( houses.empty() )
  {
    StringHelper::debug( 0xff, "Can't load house model from %s", filename.c_str() );
    return;
  }

  for( VariantMap::iterator it = houses.begin(); it != houses.end(); it++ )
  {
    // this is not a comment (comments start by #)
    // std::cout << "Line #" << linenum << ":" << line << std::endl;
    VariantMap hSpec = (*it).second.toMap();

    HouseLevelSpec spec;
    spec._houseLevel = hSpec.get( "level" ).toInt();
    spec._internalName = (*it).first;
    spec._levelName = hSpec.get( "title" ).toString();
    spec._maxHabitantsByTile = hSpec.get( "habitants" ).toInt();
    spec._midDesirability = hSpec.get( "minDesirability" ).toInt();  // min desirability
    spec._maxDesirability = hSpec.get( "maxDesirability" ).toInt();  // desirability levelUp
    spec._minEntertainmentLevel = hSpec.get( "entertainment" ).toInt();
    spec._minWaterLevel = hSpec.get( "water" ).toInt();
    spec._minReligionLevel = hSpec.get( "religion" ).toInt();
    spec._minEducationLevel = hSpec.get( "education" ).toInt();
    spec._minHealthLevel = hSpec.get( "health" ).toInt();
    spec._minFoodLevel = hSpec.get( "food" ).toInt();
    
    spec._requiredGoods[G_WHEAT] = 1;  // hard coded ... to be changed!
    spec._requiredGoods[G_POTTERY] = hSpec.get( "pottery" ).toInt();  // pottery
    spec._requiredGoods[G_OIL] = hSpec.get( "oil" ).toInt();  // oil
    spec._requiredGoods[G_FURNITURE] = hSpec.get( "furniture").toInt();// furniture
    spec._requiredGoods[G_WINE] = hSpec.get( "wine" ).toInt();  // wine
    spec._crime = hSpec.get( "crime" ).toInt();;  // crime
    spec._prosperity = hSpec.get( "prosperity" ).toInt();  // prosperity
    spec._taxRate = hSpec.get( "tax" ).toInt();// tax_rate

    HouseSpecHelper::getInstance().setHouseLevelSpec( spec );    
  }

}