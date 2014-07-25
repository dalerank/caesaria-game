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
  typedef enum { acNone=0, acMove, acFight, acDie, acWork, acMax } Action;
  typedef enum { showDebugInfo=1 } Flag;

  Walker( PlayerCityPtr city );
  virtual ~Walker();

  virtual void timeStep(const unsigned long time);  // performs one simulation step
  virtual constants::walker::Type type() const;
  // position and movement

  TilePos pos() const;
  void setPos( const TilePos& pos );

  virtual Point mappos() const;
  Point tilesubpos() const;

  virtual void setPathway(const Pathway& pathway);
  const Pathway& pathway() const;

  virtual void turn( TilePos pos );

  void setSpeed(const float speed);
  float speed() const;
  void setSpeedMultiplier( float koeff );

  void setUniqueId( const UniqueId uid );
  UniqueId uniqueId() const;

  void setFlag( Flag flag, bool value );
  bool getFlag( Flag flag ) const;

  constants::Direction direction() const;
  Walker::Action action() const;

  virtual double health() const;
  virtual void updateHealth(double value);
  virtual void acceptAction( Action action, TilePos pos );

  virtual void setName( const std::string& name );
  virtual const std::string& name() const;

  virtual std::string currentThinks() const;
  virtual void setThinks( std::string newThinks );

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void addAbility( AbilityPtr ability );

  virtual void go( float speed = 1.0 );
  virtual void wait( int ticks = 0 );
  virtual int  waitInterval() const;
  virtual bool die();

  virtual void getPictures( gfx::Pictures& oPics);

  bool isDeleted() const;  // returns true if the walker should be forgotten
  void deleteLater();

  virtual void initialize( const VariantMap& options );
  virtual int agressive() const;

protected:
  void _walk();
  void _computeDirection();
  const gfx::Tile& _nextTile() const;

  virtual void _changeTile();  // called when the walker is on a new tile
  virtual void _centerTile();  // called when the walker is on the middle of a tile
  virtual void _reachedPathway();  // called when the walker is at his destination
  virtual void _changeDirection(); // called when the walker changes direction
  virtual void _brokePathway(TilePos pos);
  virtual void _noWay();
  virtual void _waitFinished();
  virtual const gfx::Picture& getMainPicture();

protected:
  Pathway& _pathwayRef();
  virtual void _updatePathway(const Pathway& pathway );

  gfx::Animation& _animationRef();
  const gfx::Animation &_animationRef() const;
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
  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_WALKER_H_INCLUDE_
