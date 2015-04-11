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
#include "city/helper.hpp"
#include "city/trade_options.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"

namespace econ
{

namespace {
CAESARIA_LITERALCONST(history)
enum { defaultSalary=30 };
}

class Treasury::Impl
{
public:
  int taxRate;
  int workerSalary;
  int money;
  int lastYearUpdate;
  int maxDebt;

  Treasury::IssuesHistory history;

signals public:
  Signal1<int> onChangeSignal;
  Signal1<econ::Issue::Type> onNewIssueSignal;
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
  case econ::Issue::unknown:
    Logger::warning( "Funds: wrong issue type %d", issue.type );
    return;
  break;

  case econ::Issue::overduePayment:
  case econ::Issue::overdueEmpireTax:
    needUpdateTreasury = false;
  break;

  default:
  {
    IssuesValue& step = _d->history.front();
    if( step.count( (econ::Issue::Type)issue.type ) == 0 )
    {
      step[ (econ::Issue::Type)issue.type ] = 0;
    }

    step[ (econ::Issue::Type)issue.type ] += abs( issue.money );

    _updateCreditDebt(step, issue);

    if( needUpdateTreasury )
    {
      _d->money += issue.money;
    }
  }
  break;
  }

  emit _d->onNewIssueSignal( (econ::Issue::Type)issue.type );

  if( saveMoney != _d->money )
  {
    emit _d->onChangeSignal( _d->money );
  }
}

void Treasury::_updateCreditDebt(IssuesValue& step, econ::Issue issue){
    if(issue.type == econ::Issue::taxIncome || issue.type == econ::Issue::exportGoods || issue.type == econ::Issue::donation){
         step[ econ::Issue::debet ] += issue.money;
    } else if (issue.type == econ::Issue::importGoods || issue.type == econ::Issue::workersWages || issue.type == econ::Issue::buildConstruction
                || issue.type == econ::Issue::creditPercents || issue.type == econ::Issue::playerSalary || issue.type == econ::Issue::sundries
                        || issue.type == econ::Issue::empireTax) {
         step[ econ::Issue::credit ] += issue.money;
    }

    step[ econ::Issue::cityProfit ] = step[ econ::Issue::debet ] + step[ econ::Issue::credit ];
}

int Treasury::money() const { return _d->money; }

int Treasury::profit() const
{
  int balanceLastYear = getIssueValue( econ::Issue::balance, lastYear );
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
  step[ econ::Issue::balance ] = _d->money;
  step[ econ::Issue::cityProfit ] = profit();

  _d->lastYearUpdate = date.year();
  _d->history.insert( _d->history.begin(), IssuesValue() );

  if( _d->history.size() > 2 )
  {
    _d->history.pop_back();
  }
}

int Treasury::getIssueValue(econ::Issue::Type type, int age ) const
{
  if( (unsigned int)age >= _d->history.size() )
    return 0;

  const IssuesValue& step = _d->history[ age ];
     
  IssuesValue::const_iterator it = step.find( type );
  return ( it == step.end() ) ? 0 : it->second;
}

int Treasury::taxRate() const{  return _d->taxRate;}
void Treasury::setTaxRate(const unsigned int value) {  _d->taxRate = value;}
int Treasury::workerSalary() const{  return _d->workerSalary;}
void Treasury::setWorkerSalary(const unsigned int value){  _d->workerSalary = value;}

VariantMap Treasury::save() const
{
  VariantMap ret;

  VARIANT_SAVE_ANY_D( ret, _d, money )
  VARIANT_SAVE_ANY_D( ret, _d, taxRate )
  VARIANT_SAVE_ANY_D( ret, _d, workerSalary )
  VARIANT_SAVE_ANY_D( ret, _d, lastYearUpdate )
  
  VariantList history;
  foreach(  stepIt, _d->history )
  {
    VariantList stepHistory;
    foreach( it, *stepIt )
    {
      stepHistory << it->first << it->second;
    }

    history.push_back( stepHistory );
  }

  ret[ literals::history ] = history;

  return ret;
}

void Treasury::load( const VariantMap& stream )
{
  VARIANT_LOAD_ANY_D( _d, money, stream )
  VARIANT_LOAD_ANYDEF_D( _d, taxRate, 7, stream )
  VARIANT_LOAD_ANYDEF_D( _d, workerSalary, defaultSalary, stream )
  VARIANT_LOAD_ANY_D( _d, lastYearUpdate, stream )

  VariantList history = stream.get( literals::history ).toList();
  _d->history.clear();
  foreach( it, history )
  {
    _d->history.push_back( IssuesValue() );
    IssuesValue& last = _d->history.back();
    const VariantList& historyStep = (*it).toList();
    VariantList::const_iterator stepIt=historyStep.begin(); 
    while( stepIt != historyStep.end() )
    {
      econ::Issue::Type type = (econ::Issue::Type)stepIt->toInt(); ++stepIt;
      int value = stepIt->toInt(); ++stepIt;
      
      last[ type ] = value;
    }
  }
}

Treasury::~Treasury(){}
Signal1<int>& Treasury::onChange(){  return _d->onChangeSignal; }
Signal1<econ::Issue::Type>&Treasury::onNewIssue(){ return _d->onNewIssueSignal; }

}//end namespace funds
