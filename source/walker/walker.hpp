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


#ifndef WALKER_HPP
#define WALKER_HPP

#include <string>
#include <memory>

#include "building/building.hpp"
#include "walker/action.hpp"
#include "building/service.hpp"
#include "gfx/picture.hpp"
#include "game/enums.hpp"
#include "core/serializer.hpp"
#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"

typedef unsigned int UniqueId;
class Pathway;

class Walker : public Serializable, public ReferenceCounted
{
public:
  typedef enum { acNone, acMove, acFight, acDie, acMax } Action;

  Walker( CityPtr city );
  virtual ~Walker();

  virtual void timeStep(const unsigned long time);  // performs one simulation step
  virtual constants::walker::Type getType() const;
  // position and movement
  int getI() const;
  int getJ() const;

  TilePos getIJ() const;
  void setIJ( const TilePos& pos );

  virtual Point getPosition() const;
  Point getSubPosition() const;

  void setPathway( const Pathway& pathWay);
  const Pathway& getPathway() const;

  virtual void turn( TilePos pos );

  //void setDestinationIJ( const TilePos& pos );
  void setSpeed(const float speed);
  virtual void onNewTile();  // called when the walker is on a new tile
  virtual void onMidTile();  // called when the walker is on the middle of a tile
  virtual void onDestination();  // called when the walker is at his destination
  virtual void onNewDirection(); // called when the walker changes direction
  void computeDirection();
  void walk();
  void setUniqueId( const UniqueId uid );

  constants::Direction getDirection();
  Walker::Action getAction();

  virtual double getHealth() const;
  virtual void updateHealth(double value);
  virtual void acceptAction( Action action, TilePos pos );

  virtual void setName( const std::string& name );
  virtual const std::string& getName() const;

  virtual std::string getThinks() const;

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void addAbility( AbilityPtr ability );

  virtual void go();
  virtual void die();

  // graphic
  WalkerGraphicType getWalkerGraphic() const;
  virtual void getPictureList(std::vector<Picture> &oPics);
  virtual const Picture& getMainPicture();

  // state
  bool isDeleted() const;  // returns true if the walker should be forgotten
  void deleteLater();

protected:
   Pathway& _getPathway();
   Animation& _getAnimation();
   void _updatePathway( const Pathway& pathway );
   void _setAction( Walker::Action action );
   void _setDirection( constants::Direction direction );
   void _setGraphic( WalkerGraphicType type );
   WalkerGraphicType _getGraphic() const;
   void _setType( constants::walker::Type type );
   CityPtr _getCity() const;
   void _setHealth( double value );
   void _updateAnimation(const unsigned int time);

private:
   /* useful method for subtile movement computation
      si   = subtile coordinate in the current tile
      i    = tile coordinate
      amount = amount of the increase, returns remaining movement if any
      midPos = position of the midtile (so that walkers are not all exactly on the middle of the tile)
      newTile = return true if we are now on a new tile
      midTile = return true if we got above the treshold
    */
  void inc(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile);
  void dec(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile);

private:   
  class Impl;
  ScopedPtr< Impl > _d;
};

/** Soldier, friend or enemy */
class Soldier : public Walker
{
public:
   Soldier(CityPtr city);
private:

};

class WalkerHelper
{
public:
  static WalkerHelper& instance();

  static std::string getName( constants::walker::Type type );
  static constants::walker::Type getType( const std::string& name );
  static std::string getPrettyTypeName( constants::walker::Type type );
  static Picture getBigPicture( constants::walker::Type type );

  virtual ~WalkerHelper();
private:
  WalkerHelper();

  class Impl;
  ScopedPtr< Impl > _d;
};
#endif
