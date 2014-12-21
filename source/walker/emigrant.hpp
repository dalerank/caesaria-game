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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_EMIGRANT_H_INCLUDED__
#define __CAESARIA_EMIGRANT_H_INCLUDED__

#include "human.hpp"
#include "core/predefinitions.hpp"
#include "game/citizen_group.hpp"

/** This is an emigrant coming with his stuff */
class Emigrant : public Human
{
public:
  static EmigrantPtr create( PlayerCityPtr city );
  static EmigrantPtr send2city( PlayerCityPtr city, const CitizenGroup& peoples,
                                 const gfx::Tile& startTile, std::string thoughts );

  bool send2city( const gfx::Tile& startTile );
  void leaveCity( const gfx::Tile& tile );

  void setPeoples( const CitizenGroup& peoples );
  const CitizenGroup& peoples() const;
  virtual void timeStep(const unsigned long time);
  virtual TilePos places(Place type) const;

  virtual ~Emigrant();

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);
  virtual bool die();
  virtual void initialize(const VariantMap &options);

protected:
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);
  virtual void _noWay();
  virtual bool _isCartBackward() const;
  virtual gfx::Animation& _cart();

  void _setCart( const gfx::Animation& anim );
  
  Emigrant( PlayerCityPtr city );

  HousePtr _findBlankHouse();
  Pathway _findSomeWay(TilePos startPoint );

  bool _checkNearestHouse();
  void _append2house(HousePtr house);
  void _checkHouses(HouseList &hlist);
  void _lockHouse(HousePtr house);
  void _splitHouseFreeRoom(HouseList& moreRooms, HouseList& lessRooms);
  void _findFinestHouses(HouseList& hlist);

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_EMIGRANT_H_INCLUDED__
