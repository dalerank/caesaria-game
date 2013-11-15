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

#ifndef __OPENCAESAR3_SENATE_H_INCLUDED_
#define __OPENCAESAR3_SENATE_H_INCLUDED_

#include "service.hpp"
#include "core/scopedptr.hpp"

class Senate : public ServiceBuilding
{
public:
  typedef enum { workless, culture, prosperity, peace, favour } Status;
  Senate();
  unsigned int getFunds() const;

  int collectTaxes();

  int getStatus( Status status ) const;

  virtual std::string getError() const;

  virtual void deliverService();  

  virtual bool canBuild(PlayerCityPtr city, const TilePos& pos )const;

  virtual void applyService(ServiceWalkerPtr walker);
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
