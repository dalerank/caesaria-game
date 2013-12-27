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

#ifndef _CAESARIA_WALKER_H_INCLUDE_
#define _CAESARIA_WALKER_H_INCLUDE_

#include <string>
#include <memory>

#include "objects/building.hpp"
#include "walker/action.hpp"
#include "objects/service.hpp"
#include "gfx/picture.hpp"
#include "game/enums.hpp"
#include "core/serializer.hpp"
#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "gfx/constants.hpp"

typedef unsigned int UniqueId;
class Pathway;

class Walker : public Serializable, public ReferenceCounted
{
public:
  typedef enum { acNone, acMove, acFight, acDie, acMax } Action;

  Walker( PlayerCityPtr city );
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

  virtual void setPathway( const Pathway& pathWay);
  const Pathway& getPathway() const;

  virtual void turn( TilePos pos );

  //void setDestinationIJ( const TilePos& pos );
  void setSpeed(const float speed);  
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

  virtual void getPictureList(PicturesArray& oPics);
  virtual const Picture& getMainPicture();

  // state
  bool isDeleted() const;  // returns true if the walker should be forgotten
  void deleteLater();

protected:
  void _walk();
  void _computeDirection();
  const Tile& _getNextTile() const;

  virtual void _changeTile();  // called when the walker is on a new tile
  virtual void _centerTile();  // called when the walker is on the middle of a tile
  virtual void _reachedPathway();  // called when the walker is at his destination
  virtual void _changeDirection(); // called when the walker changes direction
  virtual void _brokePathway(TilePos pos);

protected:
  Pathway& _pathwayRef();
  Animation& _animationRef();
  virtual void _updatePathway( const Pathway& pathway );
  void _setAction( Walker::Action action );
  void _setDirection( constants::Direction direction );
  void _setAnimation( constants::gfx::Type type );
  constants::gfx::Type _getAnimationType() const;
  void _setType( constants::walker::Type type );
  PlayerCityPtr _getCity() const;
  void _setHealth( double value );
  void _updateAnimation(const unsigned int time);
  void _setPosOnMap( Point pos );
  Point _getPosOnMap() const;

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

class WalkerHelper
{
public:
  static WalkerHelper& instance();

  static std::string getTypename( constants::walker::Type type );
  static constants::walker::Type getType( const std::string& name );
  static std::string getPrettyTypeName( constants::walker::Type type );
  static Picture getBigPicture( constants::walker::Type type );

  virtual ~WalkerHelper();
private:
  WalkerHelper();

  class Impl;
  ScopedPtr< Impl > _d;
};
#endif //_CAESARIA_WALKER_H_INCLUDE_
