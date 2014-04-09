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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_IMMIGRANT_H_INCLUDED__
#define __CAESARIA_IMMIGRANT_H_INCLUDED__

#include "walker.hpp"
#include "core/predefinitions.hpp"
#include "game/citizen_group.hpp"

/** This is an immigrant coming with his stuff */
class Immigrant : public Walker
{
public:
  static ImmigrantPtr create( PlayerCityPtr city );
  static ImmigrantPtr send2city( PlayerCityPtr city, const CitizenGroup& peoples,
                                 const gfx::Tile& startTile, std::string thinks );

  void send2city( const gfx::Tile& startTile );
  void leaveCity( const gfx::Tile& tile );

  void setPeoples( const CitizenGroup& peoples );
  virtual void timeStep(const unsigned long time);

  virtual ~Immigrant();

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);
  virtual void die();

protected:
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);

  void _setCartPicture( const gfx::Picture& pic );
  virtual const gfx::Picture& _cartPicture();
  
  Immigrant( PlayerCityPtr city );

  HousePtr _findBlankHouse();
  Pathway _findSomeWay(TilePos startPoint );

protected:
  const CitizenGroup& _getPeoples() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_IMMIGRANT_H_INCLUDED__
