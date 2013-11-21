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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "house_level.hpp"

#include "building/house.hpp"
#include "core/exception.hpp"
#include "core/gettext.hpp"
#include "core/stringhelper.hpp"
#include "core/variant.hpp"
#include "core/saveadapter.hpp"
#include "goodstore.hpp"
#include "core/foreach.hpp"
#include "city.hpp"
#include "game/goodhelper.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"

#include <string>
#include <map>
#include <list>

class HouseLevelSpec::Impl
{
public:
  int houseLevel;
  int maxHabitantsByTile;
  std::string levelName;
  std::string internalName;
 
  int taxRate;

  // required services
  int minEntertainmentLevel;
  int minHealthLevel;
  int minDesirability, maxDesirability;
  int minEducationLevel;
  int crime;
  int prosperity;
  int minWaterLevel;  // access to water (no water=0, well=1, fountain=2)
  int minReligionLevel;  // number of religions
  int minFoodLevel;  // number of food types

  typedef std::map<Good::Type, int> RequiredGoods;
  RequiredGoods requiredGoods;  // rate of good usage for every good (furniture, pottery, ...)

  typedef std::map<Good::Type, float> GoodConsumptionMuls;
  GoodConsumptionMuls consumptionMuls;
};

int HouseLevelSpec::getLevel() const
{
   return _d->houseLevel;
}

const std::string& HouseLevelSpec::getLevelName() const
{
   return _d->levelName;
}

bool HouseLevelSpec::isPatrician() const
{
   return _d->houseLevel > 12;
}

int HouseLevelSpec::getMaxHabitantsByTile() const
{
   return _d->maxHabitantsByTile;
}

int HouseLevelSpec::getTaxRate() const
{
   return _d->taxRate;
}

int HouseLevelSpec::getMinEntertainmentLevel() const
{
  return _d->minEntertainmentLevel;
}

int HouseLevelSpec::getMinEducationLevel() const
{
  return _d->minEducationLevel;
}
//
// int HouseLevelSpec::getMinHealthLevel()
// {
//    return _minHealthLevel;
// }
//
int HouseLevelSpec::getMinReligionLevel() const
{
  return _d->minReligionLevel;
}
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


bool HouseLevelSpec::checkHouse( HousePtr house, std::string* retMissing )
{
  bool res = true;
  int value;
  std::string reason;
  std::string defaultStr;
  std::string& ref = retMissing ? *retMissing : defaultStr;

  if( house->getHabitants().count() == 0 )
  {
    return false;
  }

  value = computeDesirabilityLevel( house, reason );
  if( value < _d->minDesirability )
  {
    res = false;
    ref = _("##low_desirability##");
  }

  value = computeEntertainmentLevel( house );
  if( value < _d->minEntertainmentLevel )
  {
    res = false;
    ref = _("##missing_entertainment##");
  }

  value = computeEducationLevel( house, reason );
  if( value < _d->minEducationLevel )
  {
    res = false;
    ref = /*_("##missing_education##") + */reason;
  }

  value = computeHealthLevel( house, reason );
  if( value < _d->minHealthLevel )
  {
    res = false;
    ref = /*_( "##missing_health##" ) + */reason;
  }

  value = computeReligionLevel( house );
  if( value < _d->minReligionLevel )
  {
    res = false;
    ref = _("##missing_religion##");
  }

  value = computeWaterLevel(house, reason);
  if( value < _d->minWaterLevel )
  {
    res = false;
    ref = /* _("##missing_water##") + */reason;
  }

  value = computeFoodLevel(house);
  if( value < _d->minFoodLevel )
  {
    res = false;
    ref = _("##missing_food##");
  }

  if( _d->requiredGoods[Good::pottery] != 0 && house->getGoodStore().getCurrentQty(Good::pottery) == 0)
  {
    res = false;
    ref = _("##missing_pottery##");
  }

  if( _d->requiredGoods[Good::furniture] != 0 && house->getGoodStore().getCurrentQty(Good::furniture) == 0)
  {
    res = false;
    ref = _("##missing_furniture##");
  }

  if( _d->requiredGoods[Good::oil] != 0 && house->getGoodStore().getCurrentQty(Good::oil) == 0)
  {
    res = false;
    ref = _("##missing_oil##");
  }

  return res;
}

int HouseLevelSpec::computeWaterLevel(HousePtr house, std::string &oMissingRequirement)
{
  // no water=0, well=1, fountain=2
  int res = 0;
  if (house->hasServiceAccess(Service::fontain))
  {
    res = 2;
  }
  else if (house->hasServiceAccess(Service::well))
  {
    res = 1;
    oMissingRequirement = _("##need fountain##");
  }
  else
  {
    oMissingRequirement = _("##need water##");
  }
  return res;
}


int HouseLevelSpec::computeFoodLevel(HousePtr house)
{
  int res = 0;

  const GoodStore& goodStore = house->getGoodStore();
  res += goodStore.getCurrentQty(Good::wheat) > 0 ? 1 : 0;
  res += goodStore.getCurrentQty(Good::fish) > 0 ? 1 : 0;
  res += goodStore.getCurrentQty(Good::meat) > 0 ? 1 : 0;
  res += goodStore.getCurrentQty(Good::fruit) > 0 ? 1 : 0;
  res += goodStore.getCurrentQty(Good::vegetable) > 0 ? 1 :0;

  return res;
}


int HouseLevelSpec::computeEntertainmentLevel(HousePtr house) const
{
   int res = 0;
   res += house->hasServiceAccess(Service::theater) ? 10 : 0;
   res += house->hasServiceAccess(Service::amphitheater) ? 20 : 0;
   res += house->hasServiceAccess(Service::colloseum) ? 30 : 0;
   res += house->hasServiceAccess(Service::hippodrome) ? 40 : 0;
   return res;
}


int HouseLevelSpec::computeHealthLevel( HousePtr house, std::string &oMissingRequirement)
{
   // no health=0, bath=1, bath+doctor/hospital=2, bath+doctor/hospital+barber=3, bath+doctor+hospital+barber=4
   int res = 0;
   if (house->hasServiceAccess(Service::baths))
   {
      res = 1;

      if (house->hasServiceAccess(Service::doctor) || house->hasServiceAccess(Service::hospital))
      {
         res = 2;

         if (house->hasServiceAccess(Service::barber))
         {
            res = 3;

            if (house->hasServiceAccess(Service::doctor) && house->hasServiceAccess(Service::hospital))
            {
               res = 4;
            }
            else
            {
               if (house->hasServiceAccess(Service::doctor))
               {
                  oMissingRequirement = _("##need_hospital##");
               }
               else
               {
                  oMissingRequirement = _("##need_doctor##");
               }
            }
         }
         else
         {
            oMissingRequirement = _("##need_barber##");
         }
      }
      else
      {
         oMissingRequirement = _("##need_doctor_or_hospital##");
      }
   }
   else
   {
      oMissingRequirement = _("##need_bath##");
   }
   return res;
}


int HouseLevelSpec::computeEducationLevel(HousePtr house, std::string &oMissingRequirement)
{
   int res = 0;
   if( house->hasServiceAccess(Service::school) )
   {
      res = 1;
      if( house->hasServiceAccess(Service::college) )
      {
         res = 2;
         if( house->hasServiceAccess(Service::library) )
         {
            res = 3;
         }
         else
         {
            oMissingRequirement = _("##need_library##");
         }
      }
      else
      {
         oMissingRequirement = _("##need_colege##");
      }
   }
   else
   {
      oMissingRequirement = _("##need_school##");
   }

   return res;
}

int HouseLevelSpec::computeReligionLevel(HousePtr house)
{
   int res = 0;
   res += house->hasServiceAccess(Service::religionMercury) ? 1 : 0;
   res += house->hasServiceAccess(Service::religionVenus) ? 1 : 0;
   res += house->hasServiceAccess(Service::religionMars) ? 1 : 0;
   res += house->hasServiceAccess(Service::religionNeptune) ? 1 : 0;
   res += house->hasServiceAccess(Service::religionCeres) ? 1 : 0;
   return res;
}

float HouseLevelSpec::evaluateServiceNeed(HousePtr house, const Service::Type service)
{
   float res = 0;

   switch (service)
   {
   case Service::religionCeres:
   case Service::religionNeptune:
   case Service::religionMars:
   case Service::religionVenus:
   case Service::religionMercury:
      res = evaluateReligionNeed(house, service);
      break;
   case Service::theater:
   case Service::amphitheater:
   case Service::colloseum:
   case Service::hippodrome:
      res = evaluateEntertainmentNeed(house, service);
      break;
   case Service::school:
   case Service::library:
   case Service::college:
      res = evaluateEducationNeed(house, service);
      break;
   case Service::baths:
   case Service::barber:
   case Service::doctor:
   case Service::hospital:
      res = evaluateHealthNeed(house, service);
      break;
   case Service::forum:
      res = 1;
      break;
   default:
      break;
   }

   return res * (100 - house->getServiceValue(service));
}

float HouseLevelSpec::evaluateEntertainmentNeed(HousePtr house, const Service::Type service)
{
   //int houseLevel = house.getLevelSpec().getHouseLevel();
   return (float)next()._d->minEntertainmentLevel;
}

float HouseLevelSpec::evaluateEducationNeed(HousePtr house, const Service::Type service)
{
   float res = 0;
   //int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = next()._d->minEducationLevel;
   if( minLevel == 1 )
   {
      // need school or library
      if (service != Service::college)
      {
         res = (float)( 100 - house->getServiceValue(service) );
      } 
   }
   else if (minLevel == 2)
   {
      // need school and library
      if (service != Service::college)
      {
         res = (float)( 100 - house->getServiceValue(service) );
      }
   }
   else if (minLevel == 3)
   {
      // need school and library and college
      res = (float)( 100 - house->getServiceValue(service) );
   }
   // std::cout << "education need: " << service << " " << res << std::endl;
   return res;
}

float HouseLevelSpec::evaluateHealthNeed(HousePtr house, const Service::Type service)
{
   float res = 0;
   //int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = next()._d->minHealthLevel;
   if (minLevel >= 1 && service == Service::baths)
   {
      // minLevel>=1  => need baths
      res = (float)( 100 - house->getServiceValue(service) );
   }

   if (minLevel >= 2 && (service == Service::doctor || service == Service::hospital))
   {
      if (minLevel == 4)
      {
         // need doctor and hospital
         res = (float)( 100 - house->getServiceValue(service) );
      }
      else
      {
         // need doctor or hospital
         res = (float)( 100 - std::max(house->getServiceValue(Service::doctor), house->getServiceValue(Service::hospital)) );
      }
   }

   if (minLevel >= 3 && service == Service::barber)
   {
      // minLevel>=3  => need barber
      res = (float)( 100 - house->getServiceValue(service) );
   }

   return (std::max<float>)( res, 100 - house->getHealthLevel() );
}

float HouseLevelSpec::evaluateReligionNeed(HousePtr house, const Service::Type service)
{
   //int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = next()._d->minReligionLevel;

   return (float)minLevel;
}

int HouseLevelSpec::computeMonthlyConsumption(House &house, const Good::Type goodType, bool real)
{
  int res = 0;
  if (_d->requiredGoods[goodType] != 0)
  {
    res = house.getHabitants().count() * _d->requiredGoods[goodType];
  }

  res *= (real ? _d->consumptionMuls[ goodType ] : 1);

  return res;
}

const std::string& HouseLevelSpec::getInternalName() const
{
  return _d->internalName;
}

int HouseLevelSpec::getRequiredGoodLevel(Good::Type type) const
{
  return _d->requiredGoods[type];
}

int HouseLevelSpec::getProsperity() const
{
  return _d->prosperity;
}

int HouseLevelSpec::getCrime() const
{
  return _d->crime;
}

HouseLevelSpec::~HouseLevelSpec()
{

}

HouseLevelSpec::HouseLevelSpec() : _d( new Impl )
{

}

HouseLevelSpec::HouseLevelSpec( const HouseLevelSpec& other ) : _d( new Impl )
{
  *this = other;
}

HouseLevelSpec HouseLevelSpec::next() const
{
  return HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel+1);
}

int HouseLevelSpec::computeDesirabilityLevel(HousePtr house, std::string& oMissingRequirement) const
{
  PlayerCityPtr city = house->_getCity();

  TilesArray area = city->getTilemap().getArea( house->getTilePos() - TilePos( 2, 2 ), house->getSize() + Size( 4 ) );

  float middleDesirbl = (float)area.front()->getDesirability();
  foreach( Tile* tile, area )
  {
    middleDesirbl = (middleDesirbl + (float)tile->getDesirability() )/2.f;
  }

  return (int)middleDesirbl;
}

HouseLevelSpec& HouseLevelSpec::operator=( const HouseLevelSpec& other )
{
  _d->houseLevel = other._d->houseLevel;
  _d->maxHabitantsByTile = other._d->maxHabitantsByTile;
  _d->levelName = other._d->levelName;
  _d->internalName = other._d->internalName;
  _d->taxRate = other._d->taxRate;

  // required services
  _d->minEntertainmentLevel = other._d->minEntertainmentLevel;
  _d->minHealthLevel = other._d->minHealthLevel;
  _d->minDesirability = other._d->minDesirability;
  _d->maxDesirability = other._d->maxDesirability;
  _d->minEducationLevel = other._d->minEducationLevel;
  _d->crime = other._d->crime;
  _d->prosperity = other._d->prosperity;
  _d->minWaterLevel = other._d->minWaterLevel;  // access to water (no water=0, well=1, fountain=2)
  _d->minReligionLevel = other._d->minReligionLevel;  // number of religions
  _d->minFoodLevel = other._d->minFoodLevel;  // number of food types
  _d->requiredGoods = other._d->requiredGoods;
  _d->consumptionMuls = other._d->consumptionMuls;

  return *this;
}

class HouseSpecHelper::Impl
{
public:
  typedef std::map<int, int> HouseLevelSpecsEqMap;
  typedef std::map<int, HouseLevelSpec > HouseLevels;

  HouseLevels spec_by_level;  // key=houseLevel, value=houseLevelSpec
  HouseLevelSpecsEqMap level_by_id;  // key=houseId, value=houseLevel
};

HouseSpecHelper& HouseSpecHelper::getInstance()
{
  static HouseSpecHelper inst;
  return inst;
}

HouseSpecHelper::HouseSpecHelper() : _d( new Impl )
{
  Logger::warning( "HouseLevelSpec INIT" );

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

HouseLevelSpec HouseSpecHelper::getHouseLevelSpec(const int houseLevel)
{
  int level = (math::clamp)(houseLevel, 0, 17);
  return _d->spec_by_level[level];
}

int HouseSpecHelper::getHouseLevel(const int houseId)
{
  return _d->level_by_id[houseId];
}

int HouseSpecHelper::getHouseLevel( const std::string& name )
{
  foreach( Impl::HouseLevels::value_type& item, _d->spec_by_level )
  {
    if( item.second.getInternalName() == name )
    {
      return item.second.getLevel();
    }
  }

  return 0;
}

HouseSpecHelper::~HouseSpecHelper()
{

}

void HouseSpecHelper::initialize( const io::FilePath& filename )
{
  VariantMap houses = SaveAdapter::load( filename.toString() );

  if( houses.empty() )
  {
    Logger::warning( "Can't load house model from %s", filename.toString().c_str() );
    return;
  }


  foreach( VariantMap::value_type& item, houses )
  {
    // this is not a comment (comments start by #)
    // std::cout << "Line #" << linenum << ":" << line << std::endl;
    VariantMap hSpec = item.second.toMap();

    HouseLevelSpec spec;
    spec._d->houseLevel = hSpec[ "level" ].toInt();
    spec._d->internalName = item.first;
    spec._d->levelName = hSpec[ "title" ].toString();
    spec._d->maxHabitantsByTile = hSpec.get( "habitants" ).toInt();
    spec._d->minDesirability = hSpec.get( "minDesirability" ).toInt();  // min desirability
    spec._d->maxDesirability = hSpec.get( "maxDesirability" ).toInt();  // desirability levelUp
    spec._d->minEntertainmentLevel = hSpec.get( "entertainment" ).toInt();
    spec._d->minWaterLevel = hSpec.get( "water" ).toInt();
    spec._d->minReligionLevel = hSpec.get( "religion" ).toInt();
    spec._d->minEducationLevel = hSpec.get( "education" ).toInt();
    spec._d->minHealthLevel = hSpec.get( "health" ).toInt();
    spec._d->minFoodLevel = hSpec.get( "food" ).toInt();
    
    spec._d->requiredGoods[Good::wheat] = 1;  // hard coded ... to be changed!
    spec._d->requiredGoods[Good::pottery] = hSpec.get( "pottery" ).toInt();  // pottery
    spec._d->requiredGoods[Good::oil] = hSpec.get( "oil" ).toInt();  // oil
    spec._d->requiredGoods[Good::furniture] = hSpec.get( "furniture").toInt();// furniture
    spec._d->requiredGoods[Good::wine] = hSpec.get( "wine" ).toInt();  // wine
    spec._d->crime = hSpec.get( "crime" ).toInt();;  // crime
    spec._d->prosperity = hSpec.get( "prosperity" ).toInt();  // prosperity
    spec._d->taxRate = hSpec.get( "tax" ).toInt();// tax_rate

    for (int i = 0; i < Good::goodCount; ++i)
    {
      spec._d->consumptionMuls[ (Good::Type)i ] = 0.1;
    }

    //load consumption goods koefficient
    VariantMap varConsumptions = hSpec.get( "consumptionkoeff" ).toMap();
    foreach( VariantMap::value_type& v, varConsumptions )
    {
      spec._d->consumptionMuls[ GoodHelper::getType( v.first ) ] = (float)v.second;
    }

    _d->spec_by_level[ spec._d->houseLevel ] = spec;
  }

}
