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

#ifndef __CAESARIA_GOVERNORRANK_H_INCLUDED__
#define __CAESARIA_GOVERNORRANK_H_INCLUDED__

#include <string>
#include <vector>

class VariantMap;

namespace world
{

struct GovernorRank
{
  typedef enum { citizen=0, clerk, engineer,
                 architect, questor, procurate,
                 aedil, preator, consul, proconsul,
                 caesar } Level;
  std::string rankName;
  std::string pretty;
  unsigned int salary = 0;
  Level level = citizen;

  void load( const std::string& name, const VariantMap& vm );
};

typedef std::vector<GovernorRank> GovernorRanks;

}//end namespace world

#endif //__CAESARIA_GOVERNORRANK_H_INCLUDED__
