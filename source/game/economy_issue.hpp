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

#ifndef __CAESARIA_ECONOMY_ISSUE_H_INCLUDED__
#define __CAESARIA_ECONOMY_ISSUE_H_INCLUDED__

#include <string>

namespace econ
{

struct Issue
{
  enum Type { unknown=0, taxIncome=1,
              exportGoods, donation,
              importGoods, workersWages,
              buildConstruction, creditPercents,
              playerSalary, sundries, moneyStolen,
              empireTax, debet, credit, cityProfit,
              overduePayment, overdueEmpireTax,
              balance, caesarsHelp,
              issueTypeCount };
  Type type = unknown;
  int money = 0;

  Issue( Type t, int m ) : type( t ), money( m ) {}
};

Issue::Type findType(const std::string& name);

inline unsigned int calcTaxValue( unsigned int population, int koeff )
{
  return population / 1000 * koeff;
}

}//end namespace econ

#endif //__CAESARIA_ECONOMY_ISSUE_H_INCLUDED__
