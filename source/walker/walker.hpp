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
#include "pathway/predefinitions.hpp"
#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"
#include "predefinitions.hpp"

typedef unsigned int UniqueId;
class Pathway;

class Walker : public Serializable, public ReferenceCounted
{
public:
  typedef enum { acNone, acMove, acFight, acDie, acWork, acMax } Action;

  Walker( PlayerCityPtr city );
  virtual ~Walker();

  virtual void timeStep(const unsigned long time);  // performs one simulation step
  virtual constants::walker::Type type() const;
  // position and movement

  TilePos pos() const;
  void setPos( const TilePos& pos );

  virtual Point getMappos() const;
  Point getSubpos() const;

  virtual void setPathway(const Pathway& pathway);
  const Pathway& getPathway() const;

  virtual void turn( TilePos pos );

  void setSpeed(const float speed);
  void setSpeedMultiplier( float koeff );
  void setUniqueId( const UniqueId uid );

  constants::Direction getDirection() const;
  Walker::Action getAction() const;

  virtual double getHealth() const;
  virtual void updateHealth(double value);
  virtual void acceptAction( Action action, TilePos pos );

  virtual void setName( const std::string& name );
  virtual const std::string& getName() const;

  virtual std::string getThinks() const;
  virtual void setThinks( std::string newThinks );

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void addAbility( AbilityPtr ability );

  virtual void go();
  virtual void die();

  virtual void getPictureList(PicturesArray& oPics);

  bool isDeleted() const;  // returns true if the walker should be forgotten
  void deleteLater();

  virtual void initialize( const VariantMap& options );
  virtual int agressive() const;

protected:
  void _walk();
  void _computeDirection();
  const Tile& _getNextTile() const;

  virtual void _changeTile();  // called when the walker is on a new tile
  virtual void _centerTile();  // called when the walker is on the middle of a tile
  virtual void _reachedPathway();  // called when the walker is at his destination
  virtual void _changeDirection(); // called when the walker changes direction
  virtual void _brokePathway(TilePos pos);
  virtual const Picture& getMainPicture();

protected:
  Pathway& _pathwayRef();
  virtual void _updatePathway(const Pathway& pathway );

  Animation& _animationRef();
  const Animation &_animationRef() const;
  void _setAction( Walker::Action action );
  void _setDirection( constants::Direction direction );

  void _setType( constants::walker::Type type );
  PlayerCityPtr _city() const;
  void _setHealth( double value );
  void _updateAnimation(const unsigned int time);
  void _setWpos( Point pos );
  virtual void _updateThinks();
  Point _wpos() const;

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
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_WALKER_H_INCLUDE_
