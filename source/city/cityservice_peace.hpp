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

#ifndef __CAESARIA_CITYSERVICE_PEACE_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_PEACE_H_INCLUDED__

#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"

namespace city
{

class Peace : public city::Srvc
{
public:
  static city::SrvcPtr create();

  virtual void timeStep( PlayerCityPtr city, const unsigned int time );
  void addProtestor();
  void addRioter();
  void buildingDestroyed( gfx::TileOverlayPtr overlay );
  int value() const;
  static std::string getDefaultName();

  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream);
private:
  Peace();

  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr<Peace> PeacePtr;

}//end namespace city

#endif //__CAESARIA_CITYSERVICE_PEACE_H_INCLUDED__
