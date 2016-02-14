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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_RELIGION_CONFIG_H_INCLUDED__
#define __CAESARIA_RELIGION_CONFIG_H_INCLUDED__

namespace religion
{

namespace debug
{
enum { doBlessing=-103, doSmallCurse=-102, doWrath=-101 };
}

namespace relation
{
enum {
  wrathfull=10,
  badmood=20,
  negative=30,
  minimum4wrath=40,
  neitralMood=50,
  maximum=100
};

}

namespace penalty
{
enum { wp4wrath=5, wp4badmood=2, wp4negativeRel=1, brokenGod=25, maximum=50 };
}

namespace award
{
enum { admiredGod=50 };
}

}//end namespace config

#endif //__CAESARIA_CITY_CONFIG_H_INCLUDED__
