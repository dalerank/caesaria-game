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

#include "citizen_group.hpp"

unsigned int CitizenGroup::count() const
{
  unsigned int ret = 0;
  foreach( t,_peoples) { ret += *t; }
  return ret;
}

static std::pair<unsigned int, unsigned int> __getGroupRange( CitizenGroup::Age group )
{
  std::pair<unsigned int, unsigned int> ret(0, 0);
  switch( group )
  {
  case CitizenGroup::newborn: ret.second=1; break;
  case CitizenGroup::child: ret.first=CitizenGroup::childMin; ret.second=8; break;
  case CitizenGroup::scholar: ret.first=9; ret.second=15; break;
  case CitizenGroup::student: ret.first=16; ret.second=20; break;
  case CitizenGroup::mature: ret.first=CitizenGroup::matureMin; ret.second=50; break;
  case CitizenGroup::aged: ret.first=51; ret.second=99; break;
  case CitizenGroup::longliver: ret.second=99; ret.second=99; break;
  }

  return ret;
}

unsigned int CitizenGroup::count( Age group ) const
{
  std::pair<unsigned int, unsigned int> range = __getGroupRange( group );

  return count( range.first, range.second );
}

unsigned int CitizenGroup::count(unsigned int beginAge, unsigned int endAge) const
{
  unsigned int ret=0;
  for( unsigned int i=beginAge; i<=endAge; i++)
  {
    ret += _peoples[ i ];
  }

  return ret;
}

CitizenGroup CitizenGroup::retrieve( unsigned int rcount)
{
  CitizenGroup ret;

  if( rcount >= count() )
  {
    CitizenGroup ret = *this;
    clear();
    return ret;
  }

  while( rcount > 0 )
  {
    for( int age=newborn; age <= longliver; age++ )
    {
      unsigned int n = std::min( _peoples[ age ], 1u );

      ret[ age ] += n;
      _peoples[ age ] -= n;
      rcount -= n;

      if( rcount == 0 )
        break;
    }

    if( count() == 0 )
      break;
  }

  return ret;
}

CitizenGroup CitizenGroup::retrieve( Age group, unsigned int rcount)
{
  CitizenGroup ret;
  std::pair<unsigned int, unsigned int> range = __getGroupRange( group );

  while( rcount > 0 )
  {
    for( unsigned int age=range.first; age <= range.second; age++ )
    {
      unsigned int n = std::min( _peoples[ age ], 1u );

      ret[ age ] += n;
      _peoples[ age ] -= n;
      rcount -= n;

      if( rcount == 0 )
        break;
    }

    if( count( group ) == 0 )
      break;
  }

  return ret;
}

CitizenGroup& CitizenGroup::include(CitizenGroup& b)
{
  *this += b;
  b.clear();

  return *this;
}

void CitizenGroup::exclude(CitizenGroup& group)
{
  for( int index=newborn; index <= longliver; index++ )
  {
    if( group[index] == 0 || _peoples[ index ] == 0)
      continue;

    if( _peoples[ index ] >= group[ index ] )
    {
      _peoples[ index ] -= group[ index ];
      group[ index ] = 0;
    }
    else
    {
      group[ index ] -= _peoples[ index ];
      _peoples[ index ] = 0;
    }
  }
}

unsigned int& CitizenGroup::operator[](unsigned int age)
{
  return _peoples[ age ];
}

CitizenGroup& CitizenGroup::operator += (const CitizenGroup& b)
{
  for( int index=newborn; index <= longliver; index++ )
  {
    _peoples[ index ] += b._peoples[ index ];
  }

  return *this;
}

CitizenGroup CitizenGroup::operator-(const CitizenGroup &b) const
{
  CitizenGroup result = *this;
  CitizenGroup rb = b;
  result.exclude( rb );

  return result;
}

CitizenGroup CitizenGroup::operator+(const CitizenGroup &b) const
{
  CitizenGroup result = *this;
  result += b;
  return result;
}

bool CitizenGroup::empty() const {  return (_peoples.empty() || (0 == count())); }
void CitizenGroup::clear() { foreach( t,_peoples ) (*t) = 0; }
void CitizenGroup::set(const CitizenGroup& b) { _peoples = b._peoples; }

void CitizenGroup::makeOld()
{
  _peoples.pop_back();
  _peoples.insert( _peoples.begin(), 1, 0 );
}

unsigned int CitizenGroup::child_n()   const { return count( child   ); }
unsigned int CitizenGroup::mature_n()  const { return count( mature  ); }
unsigned int CitizenGroup::aged_n()    const { return count( aged    ); }
unsigned int CitizenGroup::scholar_n() const { return count( scholar ); }
unsigned int CitizenGroup::student_n() const { return count( student ); }

VariantList CitizenGroup::save() const
{
  VariantList ret;

  int index=0;
  foreach( g,_peoples )
  {
    if( *g > 0 )
    {
      VariantList gv;
      gv.push_back( index );
      gv.push_back( *g );
      ret.push_back( gv );
    }
    index++;
  }

  return ret;
}

void CitizenGroup::load(const VariantList& stream)
{
  foreach( g, stream )
  {
    VariantList gv = (*g).toList();
    unsigned int age = gv.get( 0, 0u );
    unsigned int count = gv.get( 1, 0u );
    _peoples[ age ] = count;
  }
}

CitizenGroup::CitizenGroup()
{
  _peoples.resize( longliver+1 );
  _peoples.reserve( longliver+2 );
}

CitizenGroup::CitizenGroup(CitizenGroup::Age age, int value)
{
  _peoples.resize( longliver+1 );
  _peoples.reserve( longliver+2 );
  _peoples[ age ] = value;
}

CitizenGroup CitizenGroup::random(int value)
{
  CitizenGroup ret;

  while( value > 0 )
  {
    int ageGroup = math::random( 100 );
    int rValue = math::random( value ) + 1;
    ret[ ageGroup ] += rValue;

    value -= rValue;
  }

  return ret;
}
