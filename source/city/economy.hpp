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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_CITYECONOMY_INCLUDE_H_
#define _CAESARIA_CITYECONOMY_INCLUDE_H_

#include "game/funds.hpp"

namespace city
{

class Economy : public econ::Treasury
{
public:
  Economy();
  virtual ~Economy();

  void payWages(PlayerCityPtr city);

  void collectTaxes(PlayerCityPtr city );

  void payMayorSalary( PlayerCityPtr city );

  void resolveIssue( econ::Issue issue );

  void checkIssue(econ::Issue::Type type);
};

}//end namespace city


#endif  //_CAESARIA_CITYECONONMY_INCLUDE_H_
