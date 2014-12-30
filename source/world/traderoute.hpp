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

#ifndef _CAESARIA_EMPIRE_TRADEROUTE_INCLUDE_H_
#define _CAESARIA_EMPIRE_TRADEROUTE_INCLUDE_H_

#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "predefinitions.hpp"
#include "core/signals.hpp"
#include "core/position.hpp"
#include "good/good.hpp"
#include "gfx/picturesarray.hpp"

namespace world
{

class Traderoute : public ReferenceCounted
{
public:
  Traderoute(EmpirePtr empire, std::string begin, std::string end );
  ~Traderoute();

  CityPtr beginCity() const;
  CityPtr endCity() const;
  CityPtr partner( const std::string& name ) const;
  std::string getName() const;

  void update( unsigned int time );
  PointsArray points( bool reverse=false ) const;
  bool containPoint( Point pos, int devianceDistance=10 );
  void setPoints(const PointsArray& points , bool seaRoute);
  const gfx::Pictures& pictures() const;

  bool isSeaRoute() const;

  MerchantPtr addMerchant( const std::string& begin, good::Store& sell, good::Store& buy );
  MerchantPtr merchant( unsigned int index );
  MerchantList merchants() const;

  VariantMap save() const;
  void load( const VariantMap& stream );

signals public:
  Signal1<MerchantPtr>& onMerchantArrived();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //_CAESARIA_EMPIRE_TRADEROUTE_INCLUDE_H_
