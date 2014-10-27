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

#ifndef __CAESARIA_CITYSERVICE_SHORELINE_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_SHORELINE_H_INCLUDED__

#include "cityservice.hpp"
#include "game/predefinitions.hpp"
#include "core/scopedptr.hpp"

namespace  city {

class Shoreline : public city::Srvc
{
public:
  static city::SrvcPtr create(PlayerCityPtr city);
  static std::string defaultName();
  virtual void timeStep(const unsigned int time );

private:
  Shoreline(PlayerCityPtr city);

  class Impl;
  ScopedPtr< Impl > _d;
};


}//end namespace city
#endif //__CAESARIA_CITYSERVICE_SHORELINE_H_INCLUDED__
