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

#ifndef __CAESARIA_CITYSTATISTIC_H_INCLUDED__
#define __CAESARIA_CITYSTATISTIC_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "core/signals.hpp"
#include "good/good.hpp"
#include "predefinitions.hpp"
#include "objects/predefinitions.hpp"

namespace city
{

class Statistic
{
public:
  typedef std::map< Good::Type, int > GoodsMap;

  static void getWorkersNumber( PlayerCityPtr city, int& workersNumber, int& maxWorkers );
  static unsigned int getAvailableWorkersNumber( PlayerCityPtr city );
  static unsigned int getMontlyWorkersWages( PlayerCityPtr city );
  static unsigned int getWorklessNumber( PlayerCityPtr city );
  static unsigned int getWorklessPercent( PlayerCityPtr city );
  static unsigned int getFoodStock( PlayerCityPtr city );
  static unsigned int getFoodMonthlyConsumption( PlayerCityPtr city );
  static unsigned int getFoodProducing( PlayerCityPtr city );
  static unsigned int getTaxValue( PlayerCityPtr city );
  static HouseList getEvolveEducationReadyHouse( PlayerCityPtr );
  static unsigned int getCrimeLevel( PlayerCityPtr city );
  static GoodsMap getGoodsMap( PlayerCityPtr city );
};

}//end namespace city

#endif //__CAESARIA_CITYSTATISTIC_H_INCLUDED__
