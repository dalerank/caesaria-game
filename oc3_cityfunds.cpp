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

#include "oc3_cityfunds.hpp"
#include <map>

class CityFunds::Impl
{
public:
  int money;

  typedef std::map< CityFunds::IssueType, int > IssuesHistory;
  IssuesHistory history;
};

CityFunds::CityFunds() : _d( new Impl )
{
  _d->money = 0;
}

void CityFunds::resolveIssue( FundIssue issue )
{
  switch( issue.type )
  {
  case unknown: _OC3_DEBUG_BREAK_IF( "wrong issue" ); break;
  default:
    if( _d->history.find( (IssueType)issue.type ) == _d->history.end() )
    {
      _d->history[ (IssueType)issue.type ] = 0;
    }

    _d->history[ (IssueType)issue.type ] += issue.money;
    _d->money += issue.money;
  break;
  }
}

int CityFunds::getValue() const
{
  return _d->money;
}

void CityFunds::clearHistory()
{
  _d->history.clear();
}

int CityFunds::getIssueValue( IssueType type ) const
{
  Impl::IssuesHistory::iterator it = _d->history.find( type );
  return ( it == _d->history.end() ) ? 0 : it->second;
}

VariantMap CityFunds::save() const
{
  VariantMap ret;

  ret[ "money" ] = _d->money;
  
  VariantList history;
  for( Impl::IssuesHistory::iterator it = _d->history.begin(); it != _d->history.end(); it++ )
  {
    history.push_back( it->first );
    history.push_back( it->second );
  }

  ret[ "history" ] = history;

  return ret;
}

void CityFunds::load( const VariantMap& stream )
{
  _d->money = stream.get( "money" ).toInt();

  VariantList history = stream.get( "history" ).toList();

  VariantList::iterator it = history.begin();
  while( it != history.end() )
  {
    IssueType type = (IssueType)it->toInt(); it++;
    int value = it->toInt(); it++;
    
    _d->history[ type ] = value;
  }
}

CityFunds::~CityFunds()
{

}
