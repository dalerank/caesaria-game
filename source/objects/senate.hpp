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

#ifndef __CAESARIA_SENATE_H_INCLUDED_
#define __CAESARIA_SENATE_H_INCLUDED_

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

  virtual bool canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& aroundTiles)const;

  virtual void applyService(ServiceWalkerPtr walker);

  virtual void build(PlayerCityPtr city, const TilePos &pos);

  virtual unsigned int getWalkerDistance() const;

  virtual void timeStep(const unsigned long time);
private:
  void _updateUnemployers();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_SENATE_H_INCLUDED_
