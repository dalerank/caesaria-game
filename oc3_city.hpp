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


#ifndef CITY_HPP
#define CITY_HPP

#include "oc3_tilemap.hpp"
#include "oc3_walker.hpp"
#include "oc3_building.hpp"
#include "oc3_service_building.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_house.hpp"
#include "oc3_enums.hpp"
#include "oc3_serializer.hpp"
#include "oc3_signals.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_cityservice.hpp"

class TilePos;

class City : public Serializable
{
public:
  typedef std::list< LandOverlay* > LandOverlays;
   City();
   ~City();

   void timeStep();  // performs one simulation step
   void monthStep();

  Walkers getWalkerList( const WalkerType type );
  void addWalker( WalkerPtr walker );
  void removeWalker( WalkerPtr walker );

  void addService( CityServicePtr service );
  CityServicePtr findService( const std::string& name );

  LandOverlays& getOverlayList();
  LandOverlays getBuildingList( const BuildingType buildingType );

   void setRoadExit( const TilePos& pos );
   void setBoatEntryIJ(const unsigned int i, const unsigned int j);
   void setBoatExitIJ(const unsigned int i, const unsigned int j);

   void setRoadEntry( const TilePos& pos );
   TilePos getRoadEntry() const;
   
   void setCameraStartIJ(const unsigned int i, const unsigned int j);
   void setCameraStartIJ(const TilePos pos);
   unsigned int getCameraStartI() const;
   unsigned int getCameraStartJ() const;
   TilePos getCameraStartIJ() const;
      
   TilePos getRoadExitIJ() const;
   
   unsigned int getBoatEntryI() const;
   unsigned int getBoatEntryJ() const;
   unsigned int getBoatExitI() const;
   unsigned int getBoatExitJ() const;

   ClimateType getClimate() const;
   void setClimate(const ClimateType);

   int getTaxRate() const;
   void setTaxRate(const int taxRate);
   long getFunds() const;
   void setFunds(const long funds);
   long getPopulation() const;
   unsigned long getMonth() const;

   Tilemap& getTilemap();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

   // add construction
   void build( const BuildingType type, const TilePos& pos );

   //
   void disaster( const TilePos& pos, DisasterType type );
   // remove construction
   void clearLand( const TilePos& pos );
   // collect taxes from all houses
   void collectTaxes();

   unsigned long getTime();
   
   // only for DEBUG
   
   
   short int     *pGraphicGrid ;
   unsigned char *pEdgeGrid    ;
   short int     *pTerrainGrid ;
   unsigned char *pRndmTerGrid ;
   unsigned char *pRandomGrid  ;
   unsigned char *pZeroGrid    ;   

oc3_signals public:
   Signal1<int>& onPopulationChanged();
   Signal1<int>& onFundsChanged();
   Signal1<int>& onMonthChanged();

private:
   unsigned int _boatEntryI, _boatEntryJ;
   unsigned int _boatExitI, _boatExitJ;
   unsigned int _cameraStartI, _cameraStartJ;

   ClimateType _climate;
   Tilemap _tilemap;
   
   void _calculatePopulation();

   class Impl;
   ScopedPtr< Impl > _d;
};

class CityHelper
{
public:
  CityHelper( City& city ) : _city( city ) {}

  template< class T >
  std::list< T > getBuildings( const BuildingType type )
  {
    std::list< T > ret;
    City::LandOverlays buildings = _city.getBuildingList( type );
    for( City::LandOverlays::iterator it = buildings.begin(); 
          it != buildings.end(); it++  )
    {
      if( T b = safety_cast< T >( *it ) )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }

protected:
  City& _city;
};

#endif
