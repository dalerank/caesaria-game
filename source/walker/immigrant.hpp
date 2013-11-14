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

#ifndef __OPENCAESAR3_IMMIGRANT_H_INCLUDED__
#define __OPENCAESAR3_IMMIGRANT_H_INCLUDED__

#include "walker.hpp"
#include "core/predefinitions.hpp"
#include "game/citizen_group.hpp"

/** This is an immigrant coming with his stuff */
class Immigrant : public Walker
{
public:
  static ImmigrantPtr create( PlayerCityPtr city );
  static bool send2City( PlayerCityPtr city, const CitizenGroup& peoples, Tile& startTile );

  void send2City( Tile& startTile );
  void setPeoples( const CitizenGroup& peoples );
  virtual void timeStep(const unsigned long time);

  ~Immigrant();

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);
  virtual void die();

protected:
  virtual void _reachedPathway();
  virtual void _brokePathway();

  void setCartPicture( const Picture& pic );
  virtual const Picture& getCartPicture();
  
  Immigrant( PlayerCityPtr city );

  HousePtr _findBlankHouse();
  void _findPath2blankHouse( Tile& startPoint );

protected:
  const CitizenGroup& _getPeoples() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
