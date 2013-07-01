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

#include "oc3_factory_pottery.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_walker_cart_supplier.hpp"
#include "oc3_scenario.hpp"

FactoryPottery::FactoryPottery() : Factory(G_CLAY, G_POTTERY, B_POTTERY, Size(2))
{
  setPicture( Picture::load(ResourceGroup::commerce, 132) );

  _getAnimation().load(ResourceGroup::commerce, 133, 7);
  _getAnimation().setFrameDelay( 3 );
  _fgPictures.resize(2);

  setMaxWorkers( 10 );
  setWorkers( 10 );
}

void FactoryPottery::timeStep( const unsigned long time )
{
  Factory::timeStep( time );

  if( time % 22 == 1 && getWorkers() > 0 && getWalkerList().size() == 0 )
  {
    GoodStock& stock = getInGood();

    if( stock._currentQty < 100 )
    {
      CartSupplierPtr walker = CartSupplier::create( Scenario::instance().getCity() );
      walker->send2City( this, stock._goodType, stock._maxQty - stock._currentQty );

      if( !walker->isDeleted() )
      {
        addWalker( walker.as<Walker>() );
      }
    }
  }
}