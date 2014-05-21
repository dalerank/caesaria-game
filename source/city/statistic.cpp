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

#include "statistic.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "trade_options.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "objects/granary.hpp"
#include "objects/house_level.hpp"
#include "good/goodstore.hpp"
#include "city/funds.hpp"
#include "objects/farm.hpp"
#include "objects/warehouse.hpp"
#include "city/cityservice_disorder.hpp"
#include <map>

using namespace constants;

namespace city
{

void Statistic::getWorkersNumber(PlayerCityPtr city, int& workersNumber, int& maxWorkers )
{
  WorkingBuildingList buildings;
  buildings << city->overlays();

  workersNumber = 0;
  maxWorkers = 0;
  foreach( bld, buildings )
  {
    workersNumber += (*bld)->numberWorkers();
    maxWorkers += (*bld)->maxWorkers();
  }
}

unsigned int Statistic::getAvailableWorkersNumber(PlayerCityPtr city)
{
  Helper helper( city );

  HouseList houses = helper.find<House>( building::house );

  int workersNumber = 0;
  foreach( h, houses )
  {
    workersNumber += (*h)->habitants().count( CitizenGroup::mature );
  }

  return workersNumber;
}

unsigned int Statistic::getMontlyWorkersWages(PlayerCityPtr city)
{
  int workersNumber, maxWorkers;
  getWorkersNumber( city, workersNumber, maxWorkers );

  if( workersNumber == 0 )
    return 0;

  //wages all worker in year
  //workers take salary in sestertius 1/100 part of dinarius
  int wages = workersNumber * city->funds().workerSalary() / 100;

  wages = std::max<int>( wages, 1 );

  return wages;
}

unsigned int Statistic::getWorklessNumber(PlayerCityPtr city)
{
  Helper helper( city );

  HouseList houses = helper.find<House>( building::house );

  int worklessNumber = 0;
  foreach( h, houses ) { worklessNumber += (*h)->getServiceValue( Service::recruter ); }

  return worklessNumber;
}

unsigned int Statistic::getWorklessPercent(PlayerCityPtr city)
{
  return getWorklessNumber( city ) * 100 / (getAvailableWorkersNumber( city )+1);
}

unsigned int Statistic::getCrimeLevel( PlayerCityPtr city )
{
  DisorderPtr ds = ptr_cast<Disorder>( city->findService( Disorder::getDefaultName() ) );
  return ds.isValid() ? ds->value() : 0;
}

unsigned int Statistic::getFoodStock(PlayerCityPtr city)
{
  Helper helper( city );

  int foodSum = 0;

  GranaryList granaries = helper.find<Granary>( building::granary );
  foreach( gr, granaries ) { foodSum += (*gr)->store().qty(); }

  return foodSum;
}

unsigned int Statistic::getFoodMonthlyConsumption(PlayerCityPtr city)
{
  Helper helper( city );

  int foodComsumption = 0;
  HouseList houses = helper.find<House>( building::house );

  foreach( h, houses ) { foodComsumption += (*h)->spec().computeMonthlyFoodConsumption( *h ); }

  return foodComsumption;
}

unsigned int Statistic::getFoodProducing(PlayerCityPtr city)
{
  Helper helper( city );

  int foodProducing = 0;
  FarmList farms = helper.find<Farm>( building::foodGroup );

  foreach( f, farms ) { foodProducing += (*f)->getProduceQty(); }

  return foodProducing;
}

unsigned int Statistic::getTaxValue(PlayerCityPtr city)
{
  Helper helper( city );
  HouseList houses = helper.find<House>( building::house );

  float taxValue = 0.f;
  float taxRate = city->funds().taxRate();
  foreach( house, houses )
  {
    int maxhb = (*house)->maxHabitants();
    if( maxhb == 0 )
      continue;

    int maturehb = (*house)->habitants().count( CitizenGroup::mature );
    int housetax = (*house)->spec().taxRate();
    taxValue += housetax * maturehb * taxRate / maxhb;
  }

  return taxValue;
}

HouseList Statistic::getEvolveEducationReadyHouse(PlayerCityPtr city)
{
  HouseList ret;

  Helper helper( city );
  HouseList houses = helper.find<House>( building::house );

  foreach( it, houses )
  {
    gfx::TileOverlay::Type btype;
    (*it)->spec().next().checkHouse( *it, NULL, &btype );
    switch( btype )
    {
    case building::school:
    case building::library:
    case building::academy:
      ret.push_back( *it );
    break;

    default: break;
    }
  }

  return ret;
}

Statistic::GoodsMap Statistic::getGoodsMap(PlayerCityPtr city)
{
  Helper helper( city );
  GoodsMap cityGoodsAvailable;

  WarehouseList warehouses = helper.find<Warehouse>( building::warehouse );
  foreach( wh, warehouses )
  {
    for( int i=Good::wheat; i < Good::goodCount; i++ )
    {
      Good::Type goodType = (Good::Type)i;
      cityGoodsAvailable[ goodType ] += (*wh)->store().qty( goodType );
    }
  }

  return cityGoodsAvailable;
}

}//end namespace city
