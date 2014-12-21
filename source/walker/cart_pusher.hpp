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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_CART_PUSHER_H_INCLUDE_
#define _CAESARIA_CART_PUSHER_H_INCLUDE_

#include "human.hpp"
#include "core/predefinitions.hpp"

/** This walker delivers goods */
class CartPusher : public Human
{
public:
  typedef enum { simpleCart = 100,
                 bigCart = 200,
                 megaCart = 400
               } CartCapacity;

  static CartPusherPtr create( PlayerCityPtr city, CartCapacity cap=simpleCart );
  virtual ~CartPusher();

  void setProducerBuilding( BuildingPtr building );
  void setConsumerBuilding( BuildingPtr building );
  BuildingPtr producerBuilding();
  BuildingPtr consumerBuilding();
  good::Stock& stock();

  virtual void getPictures( gfx::Pictures& oPics);

  void send2city( BuildingPtr building, good::Stock& carry );

  virtual void timeStep(const unsigned long time);

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);
  virtual bool die();
  virtual std::string thoughts( Thought th ) const;
  virtual TilePos places(Place type) const;

protected:
  CartPusher( PlayerCityPtr city );

  virtual gfx::Animation& getCartPicture();
  virtual void _changeDirection();
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);

  void _computeWalkerDestination();
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_CART_PUSHER_H_INCLUDE_
