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

#include "oc3_citizen_group.hpp"

int CitizenGroup::count() const
{
  int ret = 0;
  for( const_iterator t=begin(); t != end(); t++ ){ ret += t->second; }
  return ret;
}

int CitizenGroup::count( Age group) const
{
  int tmin=0, tmax=0;
  switch( group )
  {
  case child: tmax=8; break;
  case young: tmin=9; tmax=15; break;
  case student: tmin=16; tmax=20; break;
  case mature: tmin=21; tmax=50; break;
  case aged: tmin=51; tmax=99; break;
  }

  int ret=0;
  for( const_iterator t=begin(); t != end(); t++ )
  {
    if( t->first >= tmin && t->first < tmax )
    {
      ret += t->second;
    }
  }

  return ret;
}
