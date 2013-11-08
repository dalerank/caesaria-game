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

#ifndef __OPENCAESAR3_CORPSE_H_INCLUDED__
#define __OPENCAESAR3_CORPSE_H_INCLUDED__

#include "walker.hpp"
#include "core/predefinitions.hpp"

/** This is an immigrant coming with his stuff */
class Corpse : public Walker
{
public:
  static WalkerPtr create( CityPtr city ); //need for walker manager
  static void create( CityPtr city, TilePos pos,
                      std::string rcGroup, int startIndex, int stopIndex,
                      bool loop=false);
  ~Corpse();

  virtual void timeStep(const unsigned long time);

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

  virtual const Picture& getMainPicture();

protected:
  Corpse( CityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_CORPSE_H_INCLUDED__
