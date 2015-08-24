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
#include "walker/helper.hpp"
#include "game/difficulty.hpp"

namespace city
{

void Services::timeStep(PlayerCityPtr city, unsigned int time)
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
      Logger::warning( "!!! WARNING: Cant initialize service %s on city create", it.first.c_str() );
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
  int walkedId = 0;
  for (auto w : *this)
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
      Logger::warning( "!!! WARNING: Can't save walker type " + WalkerHelper::getTypename( wtype ));
    }

    walkedId++;
  }

  return ret;
}

void Walkers::update(PlayerCityPtr, unsigned int time)
{
  WalkerList::iterator it = begin();
  while( it != end() )
  {
    WalkerPtr walker = *it;
    walker->timeStep( time );
    if( walker->isDeleted() )
    {
      // remove the walker from the walkers list
      //grid.remove( *it );
      it = erase(it);
    }
    else { ++it; }
  }

  merge();

  grid.update( *this );
  grid.sort();
}

VariantList Options::save() const
{
  VariantList ret;
  for (auto it : *this)
  {
    ret << Point(it.first, it.second);
  }
  return ret;
}

void Options::load(const VariantList& stream)
{
  for (auto it : stream)
  {
    Point tmp = it;
    (*this)[ (PlayerCity::OptionType)tmp.x() ] = tmp.y();
  }

  resetIfNot( PlayerCity::climateType, game::climate::central );
  resetIfNot( PlayerCity::adviserEnabled, 1 );
  resetIfNot( PlayerCity::fishPlaceEnabled, 1 );
  resetIfNot( PlayerCity::godEnabled, 1 );
  resetIfNot( PlayerCity::zoomEnabled, 1 );
  resetIfNot( PlayerCity::zoomInvert, 1 );
  resetIfNot( PlayerCity::fireKoeff, 100 );
  resetIfNot( PlayerCity::collapseKoeff, 100 );
  resetIfNot( PlayerCity::barbarianAttack, 1 );
  resetIfNot( PlayerCity::legionAttack, 1 );
  resetIfNot( PlayerCity::c3gameplay, 0 );
  resetIfNot( PlayerCity::warfNeedTimber, 1 );
  resetIfNot( PlayerCity::difficulty, game::difficulty::usual );
}

void Options::resetIfNot( int name, int value)
{
  if( !count( name ) )
    (*this)[ name ] = value;
}

}//end namespace city
