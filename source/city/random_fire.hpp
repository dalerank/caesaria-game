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

#ifndef _CAESARIA_RANDOMFIRE_H_INCLUDE_
#define _CAESARIA_RANDOMFIRE_H_INCLUDE_

#include "cityservice.hpp"
#include "city.hpp"

namespace events
{

class RandomFire : public CityService
{
public:
  static CityServicePtr create( PlayerCityPtr city, const VariantMap& options );
  virtual bool update(unsigned int time) const;

  virtual void load(const VariantMap &stream);
  virtual VariantMap save() const;

private:
  VariantMap _options;
  PlayerCityPtr _city;
  int _minPopulation, _maxPopulation;
};

}

#endif //_CAESARIA_RANDOMFIRE_H_INCLUDE_
