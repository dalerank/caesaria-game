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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com


#include "house_level.hpp"

#include "objects/house.hpp"
#include "core/exception.hpp"
#include "core/stringhelper.hpp"
#include "core/variant.hpp"
#include "core/saveadapter.hpp"
#include "good/goodstore.hpp"
#include "core/foreach.hpp"
#include "city/helper.hpp"
#include "good/goodhelper.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "game/gamedate.hpp"

#include <string>
#include <map>
#include <list>

class HouseSpecification::Impl
{
public:
  int houseLevel;
  int maxHabitantsByTile;
  std::string levelName;
  std::string internalName;
  unsigned int srvcInterval, goodInterval, foodInterval;
 
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

int HouseSpecification::level() const {   return _d->houseLevel;}
const std::string& HouseSpecification::levelName() const{   return _d->levelName;}
bool HouseSpecification::isPatrician() const{   return _d->houseLevel > 12;}
int HouseSpecification::getMaxHabitantsByTile() const{   return _d->maxHabitantsByTile;}
int HouseSpecification::taxRate() const{   return _d->taxRate;}
int HouseSpecification::getMinEntertainmentLevel() const{  return _d->minEntertainmentLevel;}
int HouseSpecification::getMinEducationLevel() const{  return _d->minEducationLevel;}
//
// int HouseLevelSpec::getMinHealthLevel()
// {
//    return _minHealthLevel;
// }
//
int HouseSpecification::getMinReligionLevel() const{  return _d->minReligionLevel;}
//
// int HouseLevelSpec::getMinWaterLevel()
// {
//    return _minWaterLevel;
// }
//
int HouseSpecification::getMinFoodLevel() const{  return _d->minFoodLevel;}


bool HouseSpecification::checkHouse( HousePtr house, std::string* retMissing )
{
  bool res = true;
  int value;
  std::string reason;
  std::string defaultStr;
  std::string& ref = retMissing ? *retMissing : defaultStr;

  if( house->getHabitants().count() == 0 )
  {
    ref = "##house_no_citizen##";
    return false;
  }

  value = computeDesirabilityLevel( house, reason );
  if( house->isCheckedDesirability() && value < _d->minDesirability )
  {
    ref = "##low_desirability##";
    return false;
  }

  value = findLowLevelHouseNearby( house, reason );
  if( value > 0 )
  {
    ref = "##nearby_building_negative_effect##";
    return false;
  }

  value = computeEntertainmentLevel( house );
  if( value < _d->minEntertainmentLevel )
  {
    if( value == 0 )
    {
      ref = "##missing_entertainment##";
    }
    else
    {
      switch( _d->minEntertainmentLevel / 20 )
      {
      case 0: ref = "##missing_entertainment_theater##"; break;
      case 1: ref = "##missing_entertainment_amph##"; break;
      case 2: ref = "##missing_entertainment_also##"; break;
      case 3: ref = "##missing_entertainment_colloseum##"; break;
      case 4: ref = "##missing_entertainment_hippodrome##"; break;
        //##missing_entertainment_patrician##
      }
    }
    return false;
  }

  value = computeEducationLevel( house, reason );
  if( value < _d->minEducationLevel )
  {
    ref = /*_("##missing_education##") + */reason;
    return false;
  }

  value = computeHealthLevel( house, reason );
  if( value < _d->minHealthLevel )
  {
    ref = /*_( "##missing_health##" ) + */reason;
    return false;
  }

  value = computeReligionLevel( house );
  if( value < _d->minReligionLevel )
  {
    switch( _d->minReligionLevel )
    {
    case 0: ref = "##missing_religion##"; break;
    case 1: ref = "##missing_second_religion##"; break;
    case 2: ref = "##missing_third_religion##"; break;
    }
    return false;
  }

  value = computeWaterLevel(house, reason);
  if( value < _d->minWaterLevel )
  {
    ref = reason;
    return false;
  }

  value = computeFoodLevel(house);
  if( value < _d->minFoodLevel )
  {
    if( !house->hasServiceAccess( Service::market ) ) { ref = "##missing_market##"; }
    else
    {
      switch( _d->minFoodLevel )
      {
      case 1: ref = "##missing_food##"; break;
      case 2: ref = "##missing_second_food##"; break;
      case 3: ref = "##missing_third_food##"; break;
      }
    }
    return false;
  }

  if( _d->requiredGoods[Good::pottery] != 0 && house->getGoodStore().qty(Good::pottery) == 0)
  {
    ref = "##missing_pottery##";
    return false;
  }

  if( _d->requiredGoods[Good::furniture] != 0 && house->getGoodStore().qty(Good::furniture) == 0)
  {
    ref = "##missing_furniture##";
    return false;
  }

  if( _d->requiredGoods[Good::oil] != 0 && house->getGoodStore().qty(Good::oil) == 0)
  {
    ref = "##missing_oil##";
    return false;
  }

  if( _d->requiredGoods[Good::wine] != 0 && house->getGoodStore().qty(Good::wine) == 0)
  {
    ref = "##missing_wine##";
    return false;
  }

  if( _d->requiredGoods[Good::prettyWine] != 0 && house->getGoodStore().qty(Good::prettyWine) == 0)
  {
    ref = "##missing_second_wine##";
    return false;
  }


  return res;
}

unsigned int HouseSpecification::getServiceConsumptionInterval() const{  return _d->srvcInterval;}
unsigned int HouseSpecification::getFoodConsumptionInterval() const{  return _d->foodInterval; }
unsigned int HouseSpecification::getGoodConsumptionInterval() const{ return _d->goodInterval; }

int HouseSpecification::findLowLevelHouseNearby(HousePtr house, std::string& oMissingRequirement)
{
  city::Helper helper( house->_city() );

  Size size = house->size();
  TilePos offset( size.width(), size.height() );
  TilePos housePos = house->pos();
  HouseList houses = helper.find<House>( constants::building::house, housePos - offset, housePos + offset );

  int ret = 0;
  foreach( it, houses )
  {
    int pop = (*it)->getHabitants().count();
    int bLevel = (*it)->getSpec().level();
    if( pop > 0 && (_d->houseLevel - bLevel > 2) )
    {
      ret = 1;
      oMissingRequirement = MetaDataHolder::getTypename( (*it)->type() );
      break;
    }
  }

  return ret;
}

int HouseSpecification::computeWaterLevel(HousePtr house, std::string &oMissingRequirement)
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
    oMissingRequirement = "##missing_fountain##";
  }
  else
  {
    oMissingRequirement = "##missing_water##";
  }
  return res;
}


int HouseSpecification::computeFoodLevel(HousePtr house)
{
  int res = 0;

  const GoodStore& goodStore = house->getGoodStore();
  res += goodStore.qty(Good::wheat) > 0 ? 1 : 0;
  res += goodStore.qty(Good::fish) > 0 ? 1 : 0;
  res += goodStore.qty(Good::meat) > 0 ? 1 : 0;
  res += goodStore.qty(Good::fruit) > 0 ? 1 : 0;
  res += goodStore.qty(Good::vegetable) > 0 ? 1 :0;

  return res;
}


int HouseSpecification::computeEntertainmentLevel(HousePtr house) const
{
   int res = 0;
   res += house->hasServiceAccess(Service::theater) ? 10 : 0;
   res += house->hasServiceAccess(Service::amphitheater) ? 20 : 0;
   res += house->hasServiceAccess(Service::colloseum) ? 30 : 0;
   res += house->hasServiceAccess(Service::hippodrome) ? 40 : 0;
   return res;
}


int HouseSpecification::computeHealthLevel( HousePtr house, std::string &oMissingRequirement)
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
                  oMissingRequirement = "##missing_hospital##";
               }
               else
               {
                  oMissingRequirement = "##missing_doctor##";
               }
            }
         }
         else
         {
            oMissingRequirement = "##missing_barber##";
         }
      }
      else
      {
         oMissingRequirement = "##missing_doctor_or_hospital##";
      }
   }
   else
   {
      oMissingRequirement = "##missing_bath##";
   }
   return res;
}


int HouseSpecification::computeEducationLevel(HousePtr house, std::string &oMissingRequirement)
{
  int res = 0;
  if( house->hasServiceAccess(Service::school) )
  {
    res = 1;
    if( house->hasServiceAccess(Service::academy) )
    {
      res = 2;
      if( house->hasServiceAccess(Service::library) )
      {
        res = 3;
      }
      else
      {
        oMissingRequirement = "##missing_library##";
      }
    }
    else
    {
      oMissingRequirement = "##missing_college##";
    }
  }
  else
  {
    oMissingRequirement = "##missing_school##";
  }

  return res;
}

int HouseSpecification::computeReligionLevel(HousePtr house) const
{
  int res = 0;
  res += house->hasServiceAccess(Service::religionMercury) ? 1 : 0;
  res += house->hasServiceAccess(Service::religionVenus) ? 1 : 0;
  res += house->hasServiceAccess(Service::religionMars) ? 1 : 0;
  res += house->hasServiceAccess(Service::religionNeptune) ? 1 : 0;
  res += house->hasServiceAccess(Service::religionCeres) ? 1 : 0;
  return res;
}

float HouseSpecification::evaluateServiceNeed(HousePtr house, const Service::Type service)
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
   case Service::academy:
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

float HouseSpecification::evaluateEntertainmentNeed(HousePtr house, const Service::Type service)
{
   //int houseLevel = house.getLevelSpec().getHouseLevel();
   return (float)next()._d->minEntertainmentLevel;
}

float HouseSpecification::evaluateEducationNeed(HousePtr house, const Service::Type service)
{
  float res = 0;
  //int houseLevel = house.getLevelSpec().getHouseLevel();
  int minLevel = next()._d->minEducationLevel;
  switch( minLevel )
  {
  case 1:  // need school
    res = (service == Service::school
            ? (float)( 100 - house->getServiceValue(service) )
            : 0);
  break;

  case 2: // need school and academy
    res = (service != Service::library
            ? (float)( 100 - house->getServiceValue(service) )
            : 0);
  break;

  case 3: // need school and library and college
     res = (float)( 100 - house->getServiceValue(service) );
  break;
  }
  // std::cout << "education need: " << service << " " << res << std::endl;
  return res;
}

float HouseSpecification::evaluateHealthNeed(HousePtr house, const Service::Type service)
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

   return std::max<float>( res, 100 - house->getState( (Construction::Param)House::health ) );
}

float HouseSpecification::evaluateReligionNeed(HousePtr house, const Service::Type service)
{
   //int houseLevel = house.getLevelSpec().getHouseLevel();
   int minLevel = next()._d->minReligionLevel;

   return (float)minLevel;
}

int HouseSpecification::computeMonthlyGoodConsumption( HousePtr house, const Good::Type goodType, bool real) const
{
  if( house.isNull() )
  {
    Logger::warning( "HouseLevelSpec::computeMonthlyConsumption parameter is null" );
    return 0;
  }

  int res=0;
  switch( goodType )
  {
  case Good::furniture:
  case Good::oil:
  case Good::pottery:
  case Good::wine:
    res = 2;
  break;

  case Good::wheat:
  case Good::meat:
  case Good::fish:
  case Good::fruit:
  case Good::vegetable:
    res = house->getHabitants().count() / 2;
  break;

  default: res = 0;
  }

  res *= (real ? _d->consumptionMuls[ goodType ] : 1);

  return (res * _d->requiredGoods[goodType]);
}

int HouseSpecification::computeMonthlyFoodConsumption(HousePtr house) const
{
  if( house.isNull() )
  {
    Logger::warning( "HouseLevelSpec::computeMonthlyFoodConsumption parameter is null" );
    return 0;
  }

  return house->getHabitants().count() / 2;
}

const std::string& HouseSpecification::internalName() const{  return _d->internalName; }
int HouseSpecification::getRequiredGoodLevel(Good::Type type) const{  return _d->requiredGoods[type];}
int HouseSpecification::prosperity() const{  return _d->prosperity;}
int HouseSpecification::crime() const{  return _d->crime;}

HouseSpecification::~HouseSpecification() {}

HouseSpecification::HouseSpecification() : _d( new Impl )
{
  _d->srvcInterval = GameDate::ticksInMonth() / 16;
  _d->foodInterval = GameDate::ticksInMonth();
  _d->goodInterval = GameDate::ticksInMonth() / 2;
}

HouseSpecification::HouseSpecification( const HouseSpecification& other ) : _d( new Impl )
{
  *this = other;
}

HouseSpecification HouseSpecification::next() const
{
  return HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel+1);
}

int HouseSpecification::computeDesirabilityLevel(HousePtr house, std::string& oMissingRequirement) const
{
  PlayerCityPtr city = house->_city();

  TilesArray area = city->tilemap().getArea( house->pos() - TilePos( 2, 2 ), house->size() + Size( 4 ) );

  float middleDesirbl = (float)area.front()->getDesirability();

  foreach( tile, area ) { middleDesirbl = (middleDesirbl + (float)(*tile)->getDesirability() )/2.f; }

  return (int)middleDesirbl;
}

HouseSpecification& HouseSpecification::operator=( const HouseSpecification& other )
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
  typedef std::map<int, HouseSpecification > HouseLevels;

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

HouseSpecification HouseSpecHelper::getHouseLevelSpec(const int houseLevel)
{
  int level = (math::clamp)(houseLevel, 0, 17);
  return _d->spec_by_level[level];
}

int HouseSpecHelper::getHouseLevel(const int houseId){  return _d->level_by_id[houseId];}

int HouseSpecHelper::getHouseLevel( const std::string& name )
{
  foreach( item, _d->spec_by_level )
  {
    if( item->second.internalName() == name )
    {
      return item->second.level();
    }
  }

  return 0;
}

HouseSpecHelper::~HouseSpecHelper(){}

void HouseSpecHelper::initialize( const vfs::Path& filename )
{
  VariantMap houseSpecs = SaveAdapter::load( filename.toString() );

  if( houseSpecs.empty() )
  {
    Logger::warning( "Can't load house model from %s", filename.toString().c_str() );
    return;
  }

  foreach( item, houseSpecs )
  {
    // this is not a comment (comments start by #)
    // std::cout << "Line #" << linenum << ":" << line << std::endl;
    VariantMap hSpec = item->second.toMap();

    HouseSpecification spec;
    spec._d->houseLevel = hSpec[ "level" ].toInt();
    spec._d->internalName = item->first;
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
    spec._d->requiredGoods[Good::fish] = 1;
    spec._d->requiredGoods[Good::meat] = 1;
    spec._d->requiredGoods[Good::fruit] = 1;
    spec._d->requiredGoods[Good::vegetable] = 1;
    spec._d->requiredGoods[Good::pottery] = hSpec.get( "pottery" ).toInt();  // pottery
    spec._d->requiredGoods[Good::oil] = hSpec.get( "oil" ).toInt();  // oil
    spec._d->requiredGoods[Good::furniture] = hSpec.get( "furniture").toInt();// furniture
    spec._d->requiredGoods[Good::wine] = hSpec.get( "wine" ).toInt();  // wine
    spec._d->crime = hSpec.get( "crime" ).toInt();  // crime
    spec._d->prosperity = hSpec.get( "prosperity" ).toInt();  // prosperity
    spec._d->taxRate = hSpec.get( "tax" ).toInt();// tax_rate

    for (int i = 0; i < Good::goodCount; ++i)
    {
      spec._d->consumptionMuls[ (Good::Type)i ] = 1;
    }

    //load consumption goods koefficient
    VariantMap varConsumptions = hSpec.get( "consumptionkoeff" ).toMap();
    foreach( v, varConsumptions )
    {
      spec._d->consumptionMuls[ GoodHelper::getType( v->first ) ] = (float)v->second;
    }

    _d->spec_by_level[ spec._d->houseLevel ] = spec;
  }
}
