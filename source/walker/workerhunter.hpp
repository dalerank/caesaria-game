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

#ifndef __CAESARIA_WORKERSHUNTER_H_INCLUDE_
#define __CAESARIA_WORKERSHUNTER_H_INCLUDE_

#include "serviceman.hpp"

class Recruter;
typedef SmartPtr<Recruter> RecruterPtr;

class Recruter : public ServiceWalker
{
public:
  static RecruterPtr create( PlayerCityPtr city );

  int getWorkersNeeded() const;
  void hireWorkers( const int workers );

  void send2City( WorkingBuildingPtr building, const int workersNeeded );

  virtual void die();

protected:
  virtual void _centerTile();

private:
  Recruter( PlayerCityPtr city );

  int _workersNeeded;
};

#endif//__CAESARIA_WORKERSHUNTER_H_INCLUDE_
