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

#ifndef __CAESARIA_CITY_CONFIG_H_INCLUDED__
#define __CAESARIA_CITY_CONFIG_H_INCLUDED__

#include "predefinitions.hpp"
#include "core/namedtype.hpp"

namespace config
{

namespace ambientsnd
{
enum { maxDistance=3, updateInterval=20 };
}

namespace animals
{
enum { defaultNumber=10 };
}

namespace crime
{
enum { maxLevel=10, protestorCost=15, rioterCost=20,
       level4protestor = 50, level4mugger = 70,
       defaultValue = 75, level4rioter = 90, maxValue=100 };
}

namespace festival
{
enum { prepareMonthsDelay=2 };
typedef enum { none=0, small, middle, big, count } Type;
}

namespace health
{
enum { levelNumber = 12, bad = 40, terrible = 20, maxValue=100 };
}

namespace workless
{
enum { high=15 };
}

namespace prosperity
{
enum { penalty=-1,
       award=1,
       cityHaveProfitAward=2,
       taxBrokenPenalty=3,
       normalWorklesPercent=5,
       caesarHelpCityPenalty=10,
       normalPlebsInCityPercent=30 };
}

namespace desirability
{
enum { normalInfluence=4, senateInfluence=10 };
}

namespace employements
{
enum { needMoreWorkers=20, hireDistance = 36 };
}

namespace migration
{
enum { uncomfortableWageDiff=5, normalTax=7, uncomfortableTax=10, highTax=15, insaneTax=20 };
}

namespace sentiment
{
enum { veryAngry=5, angry=10, veryUpset=15, upset=22,
       annoyed=30, indifferent=40, nonPleased=50,
       pleased=60, veryPleased=70, extrimelyPleased=80,
       loveYou=90, idolizeyou=95 };
}

}//end namespace city

namespace extentinfo
{
BEGIN_NAMEDTYPE(Type,none)
APPEND_NAMEDTYPE(celebrates)
APPEND_NAMEDTYPE(economy)
APPEND_NAMEDTYPE(population)
END_NAMEDTYPE(Type)
}
typedef extentinfo::Type ExtentInfo;

#endif //__CAESARIA_CITY_CONFIG_H_INCLUDED__
