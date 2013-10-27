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

#include "citizen_group.hpp"

int CitizenGroup::count() const
{
  int ret = 0;
  for( const_iterator t=begin(); t != end(); t++ ){ ret += t->second; }
  return ret;
}

int CitizenGroup::count( Age group ) const
{
  int tmin=0, tmax=0;
  switch( group )
  {
  case newborn: tmax=1; break;
  case child: tmin=childMin; tmax=8; break;
  case scholar: tmin=9; tmax=15; break;
  case student: tmin=16; tmax=20; break;
  case mature: tmin=matureMin; tmax=50; break;
  case aged: tmin=51; tmax=99; break;
  case longliver: tmin=99; tmax=99; break;
  }

  int ret=0;
  for( const_iterator t=begin(); t != end(); t++ )
  {
    if( t->first >= tmin && t->first <= tmax )
    {
      ret += t->second;
    }
  }

  return ret;
}

CitizenGroup CitizenGroup::retrieve(int count)
{
  CitizenGroup ret;

  while( count > 0 && size() > 0 )
  {
    int groupIndex = rand() % size();
    iterator g = begin();
    std::advance( g, groupIndex );
    if( g->second > 0 )
    {
      ret[ g->first ] += 1;
      (*this)[ g->first ] -= 1;
      count--;
    }

    if( g->second <= 0 )
    {
      erase( g );
    }
  }

  return ret;
}

CitizenGroup& CitizenGroup::operator += (const CitizenGroup& b)
{
  for( const_iterator g=b.begin(); g != b.end(); g++ )
  {
    (*this)[ g->first ] += g->second;
  }

  return *this;
}

VariantList CitizenGroup::save() const
{
  VariantList ret;

  for( const_iterator g = begin(); g != end(); g++ )
  {
    if( g->second != 0 )
    {
      VariantList gv;
      gv.push_back( g->first );
      gv.push_back( g->second );
      ret.push_back( gv );
    }
  }

  return ret;
}

void CitizenGroup::load(const VariantList& stream)
{
  for( VariantList::const_iterator g=stream.begin(); g != stream.end(); g++ )
  {
    VariantList gv = (*g).toList();
    int age = gv.get( 0, 0 );
    int count = gv.get( 1, 0 );
    (*this)[ age ] = count;
  }
}
