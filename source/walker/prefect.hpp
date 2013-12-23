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

#ifndef __CAESARIA_WALKER_PREFECT_H_INCLUDED__
#define __CAESARIA_WALKER_PREFECT_H_INCLUDED__

#include "serviceman.hpp"
#include "objects/prefecture.hpp"

class Prefect : public ServiceWalker
{
public:
  static PrefectPtr create( PlayerCityPtr city );

  virtual void timeStep(const unsigned long time);
  float getServiceValue() const;

  virtual void load( const VariantMap& stream );
  virtual void save( VariantMap& stream ) const;

  virtual void send2City( PrefecturePtr prefecture, int water=0 );
  virtual void die();

  ~Prefect();

protected:
  virtual void _centerTile();
  virtual void _changeTile();
  virtual void _reachedPathway();

protected:
  Prefect( PlayerCityPtr city );

  bool _looks4Protestor(TilePos& pos);
  bool _looks4Fire( ReachedBuildings& buildings, TilePos& pos );
  bool _checkPath2NearestFire( const ReachedBuildings& buildings );
  void _serveBuildings( ReachedBuildings& reachedBuildings );
  void _back2Prefecture();
  void _back2Patrol();
  bool _findFire();
  virtual void _brokePathway(TilePos pos);

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_WALKER_PREFECT_H_INCLUDED__
