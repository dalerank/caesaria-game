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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef SCENARIO_HPP
#define SCENARIO_HPP

#include <string>

#include "oc3_predefinitions.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_filepath.hpp"

class CityWinTargets;
class Player;

class Scenario
{
public:
  static Scenario& instance();
  ~Scenario();

  bool save( const io::FilePath& filename ) const;
  bool load( const io::FilePath& filename );

  EmpirePtr getEmpire() const;
  CityPtr getCity();
  Player& getPlayer() const;
  const CityPtr getCity() const;
  CityWinTargets& getWinTargets();

  void reset();

  std::string getDescription() const;
private:
  Scenario();
  
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
