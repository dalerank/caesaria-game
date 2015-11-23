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
#include "economy_issue.hpp"

namespace econ
{

enum { maxDebt=-4900 };

class IssuesDetailedHistory
{
public:
  struct DateIssue : public Issue
  {
    DateTime time;

    VariantList save() const;
    void load(const VariantList& stream);

    DateIssue() : Issue( Issue::unknown, 0) {}
    DateIssue( Issue::Type type, int money, const DateTime& time )
     : Issue( type, money )
    {
      this->time = time;
    }
  };

  typedef std::vector<DateIssue> DateIssues;

  void addIssue( Issue issue );
  const DateIssues& issues();
  IssuesDetailedHistory();
  ~IssuesDetailedHistory();

  VariantList save() const;
  void load( const VariantList& stream );

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

class IssuesValue : public std::map< Issue::Type, int >
{
public:
  VariantList save() const;
  void load( const VariantList& stream );
};

class IssuesHistory : public std::vector< IssuesValue >
{
public:
  VariantList save() const;
  void load( const VariantList& stream );
};

class Treasury
{
public:
  enum { thisYear=0, lastYear=1, twoYearsAgo=2, defaultTaxPrcnt=7 };
  enum { debtDisabled=0, debtEnabled=1  };

  Treasury();
  virtual ~Treasury();
  virtual void resolveIssue( Issue issue );

  void updateHistory( const DateTime& date );
  int getIssueValue( Issue::Type type, int age=thisYear ) const;
  int taxRate() const;
  void setTaxRate( const unsigned int value );
  int workerSalary( int wtype=0 ) const;
  void setWorkerSalary( const unsigned int value );
  void setWorkerSalary( int wtype, const unsigned int value );
  int money() const;
  int profit() const;

  bool haveMoneyForAction(unsigned int money , bool useDebt=debtEnabled);

  VariantMap save() const;
  void load( const VariantMap& stream );

signals public:
  Signal1<int>& onChange();
  Signal1<Issue::Type>& onNewIssue();

private:
  class Impl;
  ScopedPtr< Impl > _d;
  void _updateCreditDebt(IssuesValue& step, Issue issue);
};

}//end namespace funds

#endif //__CAESARIA_CITYFUNDS_H_INCLUDED__
