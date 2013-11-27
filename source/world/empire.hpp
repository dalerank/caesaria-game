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

#ifndef __OPENCAESAR3_EMPIRE_H_INCLUDED__
#define __OPENCAESAR3_EMPIRE_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "vfs/path.hpp"
#include "core/referencecounted.hpp"
#include "core/serializer.hpp"

namespace world
{

class Empire : public ReferenceCounted, public Serializable
{
public:
  static EmpirePtr create();
  ~Empire();
  CityList getCities() const;
  CityPtr getCity( const std::string& name ) const;
  CityPtr addCity( CityPtr city );
  CityPtr initPlayerCity( CityPtr city );

  void initialize( const vfs::Path& filename );
  void timeStep( unsigned int time );

  void createTradeRoute( const std::string& start, const std::string& stop );
  TradeRoutePtr getTradeRoute( unsigned int index );
  TradeRoutePtr getTradeRoute( const std::string& start, const std::string& stop );
  TradeRouteList getTradeRoutes( const std::string& startCity );
  TradeRouteList getTradeRoutes();

  void save( VariantMap& stream ) const;
  void load( const VariantMap& stream );

  void setCitiesAvailable( bool value );
  unsigned int getWorkersSalary() const;

private:
  Empire();

  class Impl;
  ScopedPtr< Impl > _d;
};

class EmpireHelper 
{
public:
  static unsigned int getTradeRouteOpenCost( EmpirePtr empire, const std::string& start, const std::string& stop );
};

}//end namespace world

#endif //__OPENCAESAR3_EMPIRE_H_INCLUDED__
