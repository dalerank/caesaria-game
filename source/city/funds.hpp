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

#ifndef __CAESARIA_CITYFUNDS_H_INCLUDED__
#define __CAESARIA_CITYFUNDS_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "core/signals.hpp"
#include "good/good.hpp"
#include "predefinitions.hpp"

struct FundIssue
{
  int type;
  int money;

  FundIssue() : type( 0 ), money( 0 ) {}
  FundIssue( int t, int m ) : type( t ), money( m ) {}
};

namespace city
{

class Funds
{
public:
  enum IssueType { unknown=0, taxIncome=1, 
                   exportGoods, donation, 
                   importGoods, workersWages, 
                   buildConstruction, creditPercents, 
                   playerSalary, sundries, moneyStolen,
                   empireTax, debet, credit, cityProfit,
                   overduePayment, overdueEmpireTax,
                   balance, caesarsHelp,
                   issueTypeCount };
  enum { thisYear=0, lastYear=1, twoYearAgo=2 };

  Funds();
  ~Funds();

  void resolveIssue( FundIssue issue );

  void updateHistory( const DateTime& date );

  int getIssueValue( IssueType type, int age=thisYear ) const;

  int taxRate() const;
  void setTaxRate( const unsigned int value );

  int workerSalary() const;
  void setWorkerSalary( const unsigned int value );

  int treasury() const;
  int profit() const;

  bool haveMoneyForAction( unsigned int money );

  VariantMap save() const;
  void load( const VariantMap& stream );

signals public:
  Signal1<int>& onChange();
  Signal1<IssueType>& onNewIssue();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace city

#endif //__CAESARIA_CITYFUNDS_H_INCLUDED__
