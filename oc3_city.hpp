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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef __OPENCAESAR3_CITY_H_INCLUDED__
#define __OPENCAESAR3_CITY_H_INCLUDED__

#include "oc3_walker.hpp"
#include "oc3_enums.hpp"
#include "oc3_serializer.hpp"
#include "oc3_signals.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_cityservice.hpp"
#include "oc3_tile.hpp"
#include "oc3_empire_city.hpp"
#include "oc3_positioni.hpp"
#include "oc3_foreach.hpp"

class DateTime;
class CityBuildOptions;
class CityTradeOptions;
class CityFunds;

class City : public EmpireCity
{
public:
  static CityPtr create( EmpirePtr empire );
  ~City();

  virtual void timeStep( unsigned int time );  // performs one simulation step

  void setLocation( const Point& location );
  Point getLocation() const;

  WalkerList getWalkerList( const WalkerType type );
  void addWalker( WalkerPtr walker );
  void removeWalker( WalkerPtr walker );

  void addService( CityServicePtr service );
  CityServicePtr findService( const std::string& name ) const;

  LandOverlayList& getOverlayList();

  void setRoadExit( const TilePos& pos );
  void setBoatEntry( const TilePos& pos );
  void setRoadEntry( const TilePos& pos );
  void setBoatExit( const TilePos& pos );
  
  TilePos getRoadExit() const;
  TilePos getBoatEntry() const;
  TilePos getBoatExit() const;

  int getLastMonthTax() const;
  int getLastMonthTaxpayer() const;

  TilePos getRoadEntry() const;
  
  void setCameraPos(const TilePos pos);
  TilePos getCameraPos() const;
     
  ClimateType getClimate() const;
  void setClimate(const ClimateType);

  int getTaxRate() const;
  void setTaxRate(const int taxRate);
  CityFunds& getFunds() const;

  int getPopulation() const;
  int getProsperity() const;
  int getCulture() const;

  Tilemap& getTilemap();

  std::string getName() const; 
  void setName( const std::string& name );

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

  // add construction
  void addOverlay(LandOverlayPtr overlay);  
  LandOverlayPtr getOverlay( const TilePos& pos ) const;

  CityBuildOptions& getBuildOptions();
  CityTradeOptions& getTradeOptions();

  void resolveMerchantArrived( EmpireMerchantPtr merchant );

  virtual const GoodStore& getSells() const;
  virtual const GoodStore& getBuys() const;

  virtual EmpirePtr getEmpire() const;

  void updateRoads();
   
oc3_signals public:
  Signal1<int>& onPopulationChanged();
  Signal1<int>& onFundsChanged();
  Signal1<std::string>& onWarningMessage();
  Signal2<const TilePos&, const std::string& >& onDisasterEvent();

protected:
  void monthStep( const DateTime& time );

private:
  City();

  class Impl;
  ScopedPtr< Impl > _d;
};

class CityHelper
{
public:
  CityHelper( CityPtr city ) : _city( city ) {}

  template< class T >
  std::list< SmartPtr< T > > getBuildings( const BuildingType type )
  {
    std::list< SmartPtr< T > > ret;
    LandOverlayList& buildings = _city->getOverlayList();
    foreach( LandOverlayPtr item, buildings )
    {
      SmartPtr< T > b = item.as<T>();
      if( b.isValid() && (b->getType() == type || type == B_MAX) )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > getBuildings( const BuildingClass type )
  {
    std::list< SmartPtr< T > > ret;
    LandOverlayList& overlays = _city->getOverlayList();
    foreach( LandOverlayPtr item, overlays )
    {
      SmartPtr< T > b = item.as<T>();
      if( b.isValid() && b->getClass() == type )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }

  template< class T >
  SmartPtr< T > getBuilding( const TilePos& pos )
  {
    LandOverlayPtr overlay = _city->getOverlay( pos );
    return overlay.as< T >();
  }

  template< class T >
  std::list< SmartPtr< T > > getProducers( const Good::Type goodtype )
  {
    std::list< SmartPtr< T > > ret;
    LandOverlayList& overlays = _city->getOverlayList();
    foreach( LandOverlayPtr item, overlays )
    {
      SmartPtr< T > b = item.as<T>();
      if( b.isValid() && b->getOutGoodType() == goodtype )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > getWalkers( const TilePos& pos )
  {
    std::list< SmartPtr< T > > ret;
    WalkerList walkers = _city->getWalkerList( WT_ALL );
    foreach( WalkerPtr walker, walkers )
    {
      if( walker->getIJ() == pos )
      {
        SmartPtr< T > b = walker.as<T>();

        if( b.isValid() )
        {
          ret.push_back( b );
        }
      }
    }

    return ret;
  }

  TilemapArea getArea( BuildingPtr building );

protected:
  CityPtr _city;
};

#endif //__OPENCAESAR3_CITY_H_INCLUDED__
