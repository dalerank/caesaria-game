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

#include "citystatistic.hpp"
#include "cityhelper.hpp"
#include "trade_options.hpp"
#include "building/house.hpp"
#include "building/constants.hpp"
#include "building/granary.hpp"
#include "house_level.hpp"
#include "goodstore.hpp"
#include "cityfunds.hpp"
#include "building/farm.hpp"

using namespace constants;

#include <map>

unsigned int CityStatistic::getCurrentWorkersNumber(PlayerCityPtr city)
{
  CityHelper helper( city );

  WorkingBuildingList buildings = helper.find<WorkingBuilding>( building::any );

  int workersNumber = 0;
  foreach( WorkingBuildingPtr bld, buildings )
  {
    workersNumber += bld->getWorkersCount();
  }

  return workersNumber;
}

unsigned int CityStatistic::getVacantionsNumber(PlayerCityPtr city)
{
  CityHelper helper( city );

  WorkingBuildingList buildings = helper.find<WorkingBuilding>( building::any );

  int workersNumber = 0;
  foreach( WorkingBuildingPtr bld, buildings )
  {
    workersNumber += bld->getMaxWorkers();
  }

  return workersNumber;
}

unsigned int CityStatistic::getAvailableWorkersNumber(PlayerCityPtr city)
{
  CityHelper helper( city );

  HouseList houses = helper.find<House>( building::house );

  int workersNumber = 0;
  foreach( HousePtr house, houses )
  {
    workersNumber += (house->getServiceValue( Service::recruter ) + house->getWorkersCount());
  }

  return workersNumber;
}

unsigned int CityStatistic::getMontlyWorkersWages(PlayerCityPtr city)
{
  int workersNumber = getCurrentWorkersNumber( city );

  if( workersNumber == 0 )
    return 0;

  //wages all worker in year
  //workers take salary in sestertius 1/100 part of dinarius
  int wages = workersNumber * city->getFunds().getWorkerSalary() / 100;

  wages = std::max<int>( wages, 1 );

  return wages;
}

unsigned int CityStatistic::getWorklessNumber(PlayerCityPtr city)
{
  CityHelper helper( city );

  HouseList houses = helper.find<House>( building::house );

  int worklessNumber = 0;
  foreach( HousePtr house, houses )
  {
    worklessNumber += house->getServiceValue( Service::recruter );
  }

  return worklessNumber;
}

unsigned int CityStatistic::getWorklessPercent(PlayerCityPtr city)
{
  return getWorklessNumber( city ) * 100 / (getAvailableWorkersNumber( city )+1);
}

unsigned int CityStatistic::getFoodStock(PlayerCityPtr city)
{
  CityHelper helper( city );

  int foodSum = 0;

  GranaryList granaries = helper.find<Granary>( building::granary );
  foreach( GranaryPtr gr, granaries )
  {
    foodSum += gr->getGoodStore().getCurrentQty();
  }

  return foodSum;
}

unsigned int CityStatistic::getFoodMonthlyConsumption(PlayerCityPtr city)
{
  CityHelper helper( city );

  int foodComsumption = 0;
  HouseList houses = helper.find<House>( building::house );

  foreach( HousePtr house, houses )
  {
    foodComsumption += house->getSpec().computeMonthlyFoodConsumption( house );
  }

  return foodComsumption;
}

unsigned int CityStatistic::getFoodProducing(PlayerCityPtr city)
{
  CityHelper helper( city );

  int foodProducing = 0;
  FarmList farms = helper.find<Farm>( building::foodGroup );

  foreach( FarmPtr farm, farms )
  {
    foodProducing += farm->getProduceQty();
  }

  return foodProducing;
}
