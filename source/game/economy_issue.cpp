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

#include "economy_issue.hpp"
#include "core/enumerator.hpp"

namespace econ
{

class EconIssueHelper : public EnumsHelper<Issue::Type>
{
public:
  EconIssueHelper() : EnumsHelper(Issue::unknown)
  {
#define _O(a) append(Issue::a,#a);
    _O(taxIncome)
    _O(exportGoods)
    _O(donation)
    _O(importGoods)
    _O(workersWages)
    _O(buildConstruction)
    _O(creditPercents)
    _O(playerSalary)
    _O(sundries)
    _O(moneyStolen)
    _O(empireTax)
    _O(debet)
    _O(credit)
    _O(cityProfit)
    _O(overduePayment)
    _O(overdueEmpireTax)
    _O(balance)
    _O(caesarsHelp)
    _O(issueTypeCount)
#undef _O
  }

};

Issue::Type findType(const std::string& name)
{
  static EconIssueHelper helper;
  return helper.findType(name);
}

}//end namespace econ
