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
// Copyright 2012-2014 Gregoire Athanase, gathanase@gmail.com

#ifndef __CAESARIA_PLAYERCITY_H_INCLUDED__
#define __CAESARIA_PLAYERCITY_H_INCLUDED__

#include "core/serializer.hpp"
#include "core/signals.hpp"
#include "gfx/tile.hpp"
#include "core/position.hpp"
#include "game/player.hpp"
#include "objects/constants.hpp"
#include "world/city.hpp"
#include "walker/constants.hpp"
#include "game/climate.hpp"

namespace city
{
class Funds;
class VictoryConditions;

namespace trade { class Options; }
namespace development { class Options; }
}

struct BorderInfo
{
  TilePos roadEntry;
  TilePos roadExit;
  TilePos boatEntry;
  TilePos boatExit;
};

class PlayerCity : public world::City
{
public:  
  typedef enum { adviserEnabled=0, godEnabled, fishPlaceEnabled, updateRoads,
                 forceBuild, warningsEnabled, updateTiles, zoomEnabled, zoomInvert } OptionType;

  static PlayerCityPtr create( world::EmpirePtr empire, PlayerPtr player );
  virtual ~PlayerCity();

  virtual void timeStep(unsigned int time);  // performs one simulation step

  WalkerList walkers(constants::walker::Type type );
  const WalkerList& walkers(const TilePos& pos);
  const WalkerList& walkers() const;

  void addWalker( WalkerPtr walker );

  void addService( city::SrvcPtr service );
  city::SrvcPtr findService( const std::string& name ) const;

  const city::SrvcList& services() const;

  gfx::TileOverlayList& overlays();

  void setBorderInfo( const BorderInfo& info );
  const BorderInfo& borderInfo() const;

  virtual gfx::Picture picture() const;
  virtual bool isPaysTaxes() const;
  virtual bool haveOverduePayment() const;
  virtual DateTime lastAttack() const;
  virtual world::Nation nation() const;

  PlayerPtr player() const;
  
  void setCameraPos(const TilePos pos);
  TilePos cameraPos() const;
     
  ClimateType climate() const;
  void setClimate(const ClimateType);

  city::Funds& funds();

  unsigned int population() const;
  virtual int strength() const;
  int prosperity() const;
  int culture() const;
  int peace() const;
  int sentiment() const;
  int favour() const;

  gfx::Tilemap& tilemap();

  virtual void save( VariantMap& stream ) const;
  virtual void load( const VariantMap& stream );

  // add construction
  void addOverlay( gfx::TileOverlayPtr overlay);
  gfx::TileOverlayPtr getOverlay( const TilePos& pos ) const;

  const city::development::Options& buildOptions() const;
  void setBuildOptions( const city::development::Options& options );

  virtual unsigned int age() const;

  const city::VictoryConditions& victoryConditions() const;
  void setVictoryConditions( const city::VictoryConditions& targets );

  city::trade::Options& tradeOptions();

  virtual void delayTrade(unsigned int month);
  virtual void addObject( world::ObjectPtr object );
  virtual void empirePricesChanged( good::Product gtype, int bCost, int sCost);

  virtual const good::Store& importingGoods() const;
  virtual const good::Store& exportingGoods() const;
  virtual unsigned int tradeType() const;

  void setOption( OptionType opt, int value );
  int getOption( OptionType opt ) const;

  void clean();
  void resize(unsigned int size );
   
signals public:
  Signal1<int>& onPopulationChanged();
  Signal1<int>& onFundsChanged();
  Signal1<std::string>& onWarningMessage();
  Signal2<TilePos,std::string>& onDisasterEvent();
  Signal0<>& onChangeBuildingOptions();  

private:
  PlayerCity( world::EmpirePtr empire );
  void _initAnimation();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_PLAYERCITY_H_INCLUDED__
