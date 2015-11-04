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

#ifndef __CAESARIA_EMPIRE_H_INCLUDED__
#define __CAESARIA_EMPIRE_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "vfs/path.hpp"
#include "core/referencecounted.hpp"
#include "core/serializer.hpp"
#include "good/good.hpp"
#include "price_info.hpp"
#include "governor_rank.hpp"

namespace world
{

class TradeRoutes;

class Empire : public ReferenceCounted, public Serializable
{
public:
  static EmpirePtr create();
  virtual ~Empire();

  CityList cities() const;

  CityPtr findCity( const std::string& name ) const;
  CityPtr addCity( CityPtr city );
  CityPtr initPlayerCity( CityPtr city );

  const ObjectList& objects() const;
  ObjectPtr findObject( const std::string& name ) const;
  ObjectList findObjects( Point location, int deviance ) const;
  void addObject( ObjectPtr obj );

  void initialize(vfs::Path citiesPath, vfs::Path objectsPath, vfs::Path filemap );
  void timeStep( unsigned int time );

  const EmpireMap& map() const;
  Emperor& emperor();
  CityPtr rome() const;

  TradeRoutes& troutes();

  TraderoutePtr createTradeRoute( std::string start, std::string stop );

  virtual void save( VariantMap& stream ) const;
  virtual void load( const VariantMap& stream );

  void setCitiesAvailable( bool value );
  unsigned int workerSalary() const;
  void setWorkerSalary( unsigned int value );

  bool isAvailable() const;
  void setAvailable( bool value );

  void setPrice(good::Product gtype, const PriceInfo &prices );
  void changePrice(good::Product gtype, const PriceInfo &delta);
  PriceInfo getPrice( good::Product gtype ) const;

  void clear();
private:
  Empire();
  void _loadObjects(const VariantMap& objects );
  void _initializeObjects(vfs::Path filename);
  void _initializeCities( vfs::Path filename );
  void _initializeCapital();

  class Impl;
  ScopedPtr< Impl > _d;
};

class EmpireHelper 
{
public:
  static unsigned int getTradeRouteOpenCost( EmpirePtr empire, const std::string& start, const std::string& stop );
  static float governorSalaryKoeff( CityPtr city );
  static GovernorRanks ranks();
  static GovernorRank getRank(GovernorRank::Level level);
};

}//end namespace world

#endif //__CAESARIA_EMPIRE_H_INCLUDED__
