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

#ifndef __OPENCAESAR3_CITYFUNDS_H_INCLUDED__
#define __OPENCAESAR3_CITYFUNDS_H_INCLUDED__

#include "oc3_scopedptr.hpp"
#include "oc3_variant.hpp"
#include "oc3_signals.hpp"
#include "oc3_good.hpp"
#include "oc3_predefinitions.hpp"

struct FundIssue
{
  int type;
  int money;

  FundIssue() : type( 0 ), money( 0 ) {}
  FundIssue( int t, int m ) : type( t ), money( m ) {}
};

class CityFunds
{
public:
  enum IssueType { unknown=0, taxIncome=1, 
                   exportGoods, donation, 
                   importGoods, workersWages, 
                   buildConstruction, creditPercents, 
                   playerSalary, otherExpenditure,
                   empireTax, debet, credit, profit,
                   balance,
                   issueTypeCount };
  enum { thisYear=0, lastYear=1 };

  CityFunds();
  ~CityFunds();

  void resolveIssue( FundIssue issue );

  void updateHistory( const DateTime& date );

  int getIssueValue( IssueType type, int age=thisYear ) const;

  int getTaxRate() const;
  void setTaxRate( const unsigned int value );

  int getWorkerSalary() const;
  void setWorkerSalary( const unsigned int value );

  int getValue() const;

  VariantMap save() const;
  void load( const VariantMap& stream );

oc3_signals public:
  Signal1<int>& onChange();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

class CityStatistic
{
public:
  static unsigned int getCurrentWorkersNumber( CityPtr city );
  static unsigned int getAvailableWorkersNumber( CityPtr city );
  static unsigned int getVacantionsNumber( CityPtr city );
  static unsigned int getMontlyWorkersWages( CityPtr city );
  static unsigned int getWorklessNumber( CityPtr city );
};

#endif //__OPENCAESAR3_CITYFUNDS_H_INCLUDED__
