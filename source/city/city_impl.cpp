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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "city_impl.hpp"
#include "cityservice.hpp"
#include "desirability.hpp"
#include "objects/overlay.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "walker/walker.hpp"
#include "cityservice_factory.hpp"
#include "city.hpp"
#include "core/logger.hpp"
#include "objects/construction.hpp"
#include "walker/helper.hpp"
#include "game/difficulty.hpp"

namespace city
{

void Services::update(PlayerCityPtr, unsigned int time)
{
  iterator serviceIt = begin();
  while( serviceIt != end() )
  {
    (*serviceIt)->timeStep( time );

    if( (*serviceIt)->isDeleted() )
    {
      (*serviceIt)->destroy();
      serviceIt = erase(serviceIt);
    }
    else { ++serviceIt; }
  }
}

void Services::initialize(PlayerCityPtr city, const std::string& model)
{
  VariantMap services = config::load( model );

  for (auto it : services)
  {
    SrvcPtr service = ServiceFactory::instance().create( city, it.first );
    if( service.isValid() )
      city->addService( service );
    else
      Logger::warning( "!!! WARNING: Cant initialize service %s on city create {0}", it.first );
  }
}

void Overlays::update(PlayerCityPtr city, unsigned int time)
{
  iterator overlayIt = begin();
  while( overlayIt != end() )
  {
    (*overlayIt)->timeStep( time );

    if( (*overlayIt)->isDeleted() )
    {
      onDestroyOverlay( city, *overlayIt );
      // remove the overlay from the overlay list
      (*overlayIt)->destroy();
      overlayIt = erase(overlayIt);
    }
    else
    {
      ++overlayIt;
    }
  }

  merge();
}

void Overlays::recalcRoadAccess()
{
  // for each overlay
  this->select<Construction>()
        .for_each( [](ConstructionPtr ptr){ ptr->computeRoadside();} );
}

void Overlays::onDestroyOverlay(PlayerCityPtr city, OverlayPtr overlay)
{
  Desirability::update( city, overlay, Desirability::off );
}

void Walkers::postpone(WalkerPtr w)
{
  ++idCount;
  w->setUniqueId( idCount );
  FlowList::postpone( w );
}

void Walkers::clear()
{
  FlowList::clear();
  grid.clear();
}

VariantMap Walkers::save() const
{
  VariantMap ret;
  Walker::UniqueId walkedId = 0;
  for(auto& w : *this)
  {
    VariantMap vm_walker;
    walker::Type wtype = walker::unknown;
    try
    {
      wtype = w->type();
      w->save( vm_walker );
      ret[ utils::i2str( walkedId ) ] = vm_walker;
    }
    catch(...)
    {
      Logger::warning( "!!! WARNING: Can't save walker type {0}", WalkerHelper::getTypename( wtype ));
    }

    walkedId++;
  }

  return ret;
}

void Walkers::update(PlayerCityPtr, unsigned int time)
{
  auto wlkIt = begin();
  while( wlkIt != end() )
  {
    WalkerPtr walker = *wlkIt;
    walker->timeStep( time );
    if( walker->isDeleted() )
    {
      // remove the walker from the walkers list
      //grid.remove( *it );
      wlkIt = erase(wlkIt);
    }
    else { ++wlkIt; }
  }

  merge();

  grid.update( *this );
  grid.sort();
}

void city::Services::destroyAll()
{
  for (auto srvc : *this)
    srvc->destroy();
}

}//end namespace city
