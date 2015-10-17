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
// Copyright 2012-2015 Gregoire Athanase, gathanase@gmail.com

#ifndef __CAESARIA_PLAYERCITY_H_INCLUDED__
#define __CAESARIA_PLAYERCITY_H_INCLUDED__

#include "core/signals.hpp"
#include "core/position.hpp"
#include "objects/constants.hpp"
#include "world/city.hpp"
#include "walker/constants.hpp"
#include "game/climate.hpp"

namespace city
{
class VictoryConditions;
class Scribes;
class ActivePoints;
class Statistic;
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
                 forceBuild, warningsEnabled, updateTiles, zoomEnabled, zoomInvert,
                 fireKoeff, barbarianAttack, c3gameplay, difficulty, legionAttack, climateType,
                 collapseKoeff, highlightBuilding, destroyEpidemicHouses, forestFire,
                 forestGrow, warfNeedTimber, showGodsUnhappyWarn, constructorMode } OptionType;

  static PlayerCityPtr create( world::EmpirePtr empire, PlayerPtr mayor );
  virtual ~PlayerCity();

  /** Call every step */
  virtual void timeStep(unsigned int time);  // performs one simulation step

  /** Return array of walkers in current tile */
  const WalkerList& walkers(const TilePos& pos);

  /** Return all walkers in city */
  const WalkerList& walkers() const;

  /** Add walker to city */
  void addWalker( WalkerPtr walker );

  /** Add service to city */
  void addService( city::SrvcPtr service );
  city::SrvcPtr findService( const std::string& name ) const;

  /** Return all services in city */
  const city::SrvcList& services() const;

  /** Set road/river enter/exit points for city */
  void setBorderInfo( const BorderInfo& info );
  const BorderInfo& borderInfo() const;

  /** Return city's icon for empire map */
  virtual gfx::Picture picture() const;
  virtual bool isPaysTaxes() const;
  virtual bool haveOverduePayment() const;
  virtual DateTime lastAttack() const;

  PlayerPtr mayor() const;
  
  /** Set/get current camera position last frame */
  void setCameraPos(const TilePos pos);
  TilePos cameraPos() const;
     
  econ::Treasury& treasury();

  virtual int strength() const;
  int prosperity() const;
  int culture() const;
  int peace() const;
  int sentiment() const;
  int favour() const;

  /** Return city's objects map */
  gfx::Tilemap& tilemap();

  virtual void save( VariantMap& stream ) const;
  virtual void load( const VariantMap& stream );

  /** Add static object to city */
  void addOverlay( OverlayPtr overlay);

  /** Get static object from current position */
  OverlayPtr getOverlay( const TilePos& pos ) const;

  /** Get all static objects in city */
  const OverlayList& overlays() const;

  city::ActivePoints& activePoints();
  city::Scribes& scribes();

  const city::development::Options& buildOptions() const;
  void setBuildOptions( const city::development::Options& options );

  /** Return current information about city */
  virtual const city::States& states() const;

  const city::VictoryConditions& victoryConditions() const;
  void setVictoryConditions( const city::VictoryConditions& targets );

  city::trade::Options& tradeOptions();

  virtual void delayTrade(unsigned int month);
  virtual void addObject( world::ObjectPtr object );
  virtual void empirePricesChanged( good::Product gtype, const world::PriceInfo& prices );
  virtual std::string about(Object::AboutType type);

  /** What city sells */
  virtual const good::Store& sells() const;

  /** What city buys */
  virtual const good::Store& buys() const;
  virtual ClimateType climate() const;

  /** Return city's trade type land,sea or both */
  virtual unsigned int tradeType() const;

  /** Set dynamic property for city */
  void setOption( OptionType opt, int value );

  /** Return dynamic property by name */
  int getOption( OptionType opt ) const;

  void clean();

  /** Change tile map in city */
  void resize(unsigned int size );

  const city::Statistic& statistic() const;
   
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
