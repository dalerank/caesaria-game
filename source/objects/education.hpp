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

#ifndef _CAESARIA_BUILDING_EDUCATION_H_INCLUDE_
#define _CAESARIA_BUILDING_EDUCATION_H_INCLUDE_

#include "building/service.hpp"

class School : public ServiceBuilding
{
public:
  School();

  int getVisitorsNumber() const;

  virtual void deliverService();

  virtual unsigned int getWalkerDistance() const;
};

class Library : public ServiceBuilding
{
public:
   Library();

   int getVisitorsNumber() const;
};

class Academy : public ServiceBuilding
{
public:
   Academy();

   int getVisitorsNumber() const;
   virtual void deliverService();
   virtual std::string getSound() const;
   virtual unsigned int getWalkerDistance() const;
};


#endif
