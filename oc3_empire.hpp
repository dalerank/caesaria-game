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
#include "core/predefinitions.hpp"
#include "oc3_filepath.hpp"
#include "core/referencecounted.hpp"
#include "oc3_serializer.hpp"

class Empire : public ReferenceCounted, public Serializable
{
public:
  static EmpirePtr create();
  ~Empire();
  EmpireCityList getCities() const;
  EmpireCityPtr getCity( const std::string& name ) const;
  EmpireCityPtr addCity( EmpireCityPtr city );
  EmpireCityPtr initPlayerCity( EmpireCityPtr city );

  void initialize( const io::FilePath& filename );
  void timeStep( unsigned int time );

  void createTradeRoute( const std::string& start, const std::string& stop );
  EmpireTradeRoutePtr getTradeRoute( unsigned int index ); 
  EmpireTradeRoutePtr getTradeRoute( const std::string& start, const std::string& stop );
  EmpireTradeRouteList getTradeRoutes( const std::string& startCity );
  EmpireTradeRouteList getTradeRoutes();

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

#endif //__OPENCAESAR3_EMPIRE_H_INCLUDED__
