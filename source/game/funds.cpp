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

#include "funds.hpp"
#include "objects/construction.hpp"
#include "core/variant_list.hpp"
#include "city/trade_options.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include "core/serialized_map.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"

namespace econ
{

namespace {
enum { defaultSalary=30 };
}

class IssuesDetailedHistory::Impl
{
public:
  IssuesDetailedHistory::DateIssues issues;
};

class Treasury::Impl
{
public:
  SerializedMap<int, int> salaries;

  int workerSalary;
  int taxRate;
  int money;
  int lastYearUpdate;
  int maxDebt;

  IssuesHistory history;
  IssuesDetailedHistory detailedHistory;

  struct {
    Signal1<int> onChange;
    Signal1<econ::Issue::Type> onNewIssue;
  } signal;
};

Treasury::Treasury() : _d( new Impl )
{
  _d->money = 0;
  _d->workerSalary = defaultSalary;
  _d->lastYearUpdate = 0;
  _d->maxDebt = econ::maxDebt - 100;
  _d->history.push_back( IssuesValue() );
}

void Treasury::resolveIssue( econ::Issue issue )
{
  int saveMoney = _d->money;
  bool needUpdateTreasury = true;
  switch( issue.type )
  {
  case Issue::unknown:
    Logger::warning( "Funds: wrong issue type {0}", issue.type );
    return;
  break;

  case Issue::overduePayment:
  case Issue::overdueEmpireTax:
    needUpdateTreasury = false;
  break;

  default:
  {
    IssuesValue& step = _d->history.front();
    if( step.count( (Issue::Type)issue.type ) == 0 )
    {
      step[ (Issue::Type)issue.type ] = 0;
    }

    step[ (Issue::Type)issue.type ] += abs( issue.money );

    _updateCreditDebt(step, issue);

    if( needUpdateTreasury )
    {
      _d->money += issue.money;
      step[ Issue::balance ] = _d->money;
    }
  }
  break;
  }

  _d->detailedHistory.addIssue( issue );

  emit _d->signal.onNewIssue( (Issue::Type)issue.type );

  if( saveMoney != _d->money )
  {
    emit _d->signal.onChange( _d->money );
  }
}

void Treasury::_updateCreditDebt(IssuesValue& step, Issue issue)
{
    if(issue.type == Issue::taxIncome || issue.type == Issue::exportGoods || issue.type == Issue::donation)
    {
         step[ Issue::debet ] += issue.money;
    }
    else if ( issue.type == Issue::importGoods || issue.type == Issue::workersWages || issue.type == Issue::buildConstruction
              || issue.type == Issue::creditPercents || issue.type == Issue::playerSalary || issue.type == Issue::sundries
              || issue.type == Issue::empireTax)
    {
         step[ Issue::credit ] += issue.money;
    }

    step[ Issue::cityProfit ] = step[ Issue::debet ] + step[ Issue::credit ];
}

int Treasury::money() const { return _d->money; }

int Treasury::profit() const
{
  int balanceLastYear = getIssueValue( Issue::balance, lastYear );
  return _d->money - balanceLastYear;
}

bool Treasury::haveMoneyForAction(unsigned int money, bool useDebt)
{
  if( useDebt )
    return (_d->money - (int)money > _d->maxDebt);
  else
    return (_d->money - (int)money >= 0);
}

void Treasury::updateHistory( const DateTime& date )
{
  if( _d->lastYearUpdate == date.year() )
  {
    return;
  }

  IssuesValue& step = _d->history.front();
  step[ Issue::balance ] = _d->money;
  step[ Issue::cityProfit ] = profit();

  _d->lastYearUpdate = date.year();
  _d->history.insert( _d->history.begin(), IssuesValue() );

  if( _d->history.size() > 2 )
  {
    _d->history.pop_back();
  }
}

int Treasury::getIssueValue(Issue::Type type, int age ) const
{
  if( (unsigned int)age >= _d->history.size() )
    return 0;

  const IssuesValue& step = _d->history[ age ];
     
  IssuesValue::const_iterator it = step.find( type );
  return ( it == step.end() ) ? 0 : it->second;
}

int Treasury::taxRate() const{  return _d->taxRate;}
void Treasury::setTaxRate(const unsigned int value) {  _d->taxRate = value;}
void Treasury::setWorkerSalary(const unsigned int value) { _d->workerSalary = value; }

void Treasury::setWorkerSalary(int wtype, const unsigned int value)
{
  _d->salaries[ wtype ] = value;
}

int Treasury::workerSalary(int wtype) const
{
  auto it = _d->salaries.find( wtype );
  if( it != _d->salaries.end() )
    return it->second;

  return _d->workerSalary;
}

VariantMap Treasury::save() const
{
  VariantMap ret;

  VARIANT_SAVE_ANY_D( ret, _d, money )
  VARIANT_SAVE_ANY_D( ret, _d, taxRate )
  VARIANT_SAVE_ANY_D( ret, _d, workerSalary )
  VARIANT_SAVE_ANY_D( ret, _d, lastYearUpdate )
  VARIANT_SAVE_CLASS_D( ret, _d, history )
  VARIANT_SAVE_CLASS_D( ret, _d, detailedHistory )
  VARIANT_SAVE_CLASS_D( ret, _d, salaries )

  return ret;
}

void Treasury::load( const VariantMap& stream )
{
  VARIANT_LOAD_ANY_D( _d, money, stream )
  VARIANT_LOAD_ANYDEF_D( _d, taxRate, 7, stream )
  VARIANT_LOAD_ANYDEF_D( _d, workerSalary, defaultSalary, stream )
  VARIANT_LOAD_ANY_D( _d, lastYearUpdate, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, history, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, detailedHistory, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, salaries, stream )
}

Treasury::~Treasury(){}
Signal1<int>& Treasury::onChange(){  return _d->signal.onChange; }
Signal1<Issue::Type>& Treasury::onNewIssue(){ return _d->signal.onNewIssue; }

VariantList IssuesHistory::save() const
{
  VariantList ret;
  for( auto& step : *this )
    ret.push_back( step.save() );

  return ret;
}

void IssuesHistory::load(const VariantList& stream)
{
  clear();
  for( auto& it : stream )
  {
    push_back( IssuesValue() );
    IssuesValue& last = back();
    last.load( it.toList() );
  }
}

void IssuesDetailedHistory::addIssue(Issue issue)
{
  DateIssue dIssue( issue.type, issue.money, game::Date::current() );
  _d->issues.push_back( dIssue );
  if( _d->issues.size() > 1 )
  {
    if( _d->issues.front().time.year() != dIssue.time.year() )
      _d->issues.erase( _d->issues.begin() );
  }
}

const IssuesDetailedHistory::DateIssues& IssuesDetailedHistory::issues()
{
  return _d->issues;
}

IssuesDetailedHistory::IssuesDetailedHistory() : _d( new Impl )
{

}

IssuesDetailedHistory::~IssuesDetailedHistory() {}

VariantList IssuesDetailedHistory::save() const
{
  VariantList ret;
  for( auto& it : _d->issues )
    ret.push_back( it.save() );

  return ret;
}

void IssuesDetailedHistory::load(const VariantList& stream)
{
  _d->issues.clear();
  for( auto& it : stream )
  {
    DateIssue dIssue;
    dIssue.load( it.toList() );
    _d->issues.push_back( dIssue );
  }
}

VariantList IssuesDetailedHistory::DateIssue::save() const
{
  VariantList ret;
  ret << time << type << money;
  return ret;
}

void IssuesDetailedHistory::DateIssue::load(const VariantList& stream)
{
  time = stream.get( 0 ).toDateTime();
  type = (Type)stream.get( 1 ).toInt();
  money = stream.get( 2 );
}

VariantList IssuesValue::save() const
{
  VariantList ret;
  for( auto& it  : *this )
    ret << it.first << it.second;

  return ret;
}

void IssuesValue::load(const VariantList& stream)
{
  VariantListReader reader( stream );
  while( !reader.atEnd() )
  {
    econ::Issue::Type type = (Issue::Type)reader.next().toInt(); //type
    int value = reader.next().toInt(); //value

    (*this)[ type ] = value;
  }
}

}//end namespace funds
