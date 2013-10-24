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


#ifndef WALKER_CART_PUSHER_HPP
#define WALKER_CART_PUSHER_HPP

#include "oc3_walker.hpp"
#include "core/predefinitions.hpp"

/** This walker delivers goods */
class CartPusher : public Walker
{
public:
  static CartPusherPtr create( CityPtr city );

  void setProducerBuilding( BuildingPtr building );
  void setConsumerBuilding( BuildingPtr building );
  BuildingPtr getProducerBuilding();
  BuildingPtr getConsumerBuilding();
  GoodStock& getStock();

  Picture& getCartPicture();
  virtual void getPictureList(std::vector<Picture> &oPics);
  virtual void onNewDirection();
  virtual void onDestination();

  void send2City( BuildingPtr building, GoodStock& carry );

  void computeWalkerDestination();
 
  void timeStep(const unsigned long time);

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

protected:
   CartPusher( CityPtr city );

private:
   class Impl;
   ScopedPtr< Impl > _d;
};

#endif
