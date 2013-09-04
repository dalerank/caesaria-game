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

#ifndef __OPENCAESAR3_SERVICE_H_INCLUDED__
#define __OPENCAESAR3_SERVICE_H_INCLUDED__

#include "oc3_enums_helper.hpp"

class Service
{
public:
  typedef enum
  {
    S_WELL, S_FOUNTAIN,
    S_MARKET,
    S_ENGINEER,
    S_SENATE, S_FORUM,
    S_PREFECT,
    S_TEMPLE_NEPTUNE, S_TEMPLE_CERES, S_TEMPLE_VENUS,  S_TEMPLE_MARS, S_TEMPLE_MERCURE, S_TEMPLE_ORACLE,
    S_DOCTOR, S_BARBER, S_BATHS, S_HOSPITAL,
    S_SCHOOL, S_LIBRARY, S_COLLEGE,
    S_THEATER, S_AMPHITHEATER, S_COLLOSSEUM,
    S_HIPPODROME,
    S_BURNING_RUINS,
    S_WORKERS_HUNTER,
    S_MAX
  } Type;
};

class ServiceHelper : public EnumsHelper<Service::Type>
{
public:
  static ServiceHelper& instance();

  static Service::Type getType(const std::string& name);
  static std::string getName( Service::Type type );

private:
  ServiceHelper();
};

#endif //__OPENCAESAR3_SERVICE_H_INCLUDED__
