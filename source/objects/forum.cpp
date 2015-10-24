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

#include "forum.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "walker/taxcollector.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "game/funds.hpp"
#include "walker/constants.hpp"
#include "senate.hpp"
#include "core/logger.hpp"
#include "events/fundissue.hpp"
#include "city/statistic.hpp"
#include "objects_factory.hpp"

REGISTER_CLASS_IN_OVERLAYFACTORY(object::forum, Forum)

class Forum::Impl
{
public:
  float taxValue;

  void removeMoney( PlayerCityPtr city );
};

Forum::Forum() : ServiceBuilding(Service::forum, object::forum, Size(2)), _d( new Impl )
{
  _d->taxValue = 0;
  _picture().load( ResourceGroup::govt, 10 );
}

void Forum::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    TaxCollectorPtr walker = TaxCollector::create( _city() );
    walker->send2City( this, ServiceWalker::goServiceMaximum|ServiceWalker::anywayWhenFailed );

    addWalker( walker.object() );
  }
}

unsigned int Forum::walkerDistance() const { return 26; }

void Forum::applyService(ServiceWalkerPtr walker)
{
  switch( walker->type() )
  {
  case walker::taxCollector:
  {
    auto taxCollector = walker.as<TaxCollector>();
    if( taxCollector.isValid() )
    {
      float tax = taxCollector->takeMoney();;
      _d->taxValue += tax;
      Logger::warning( "Forum: collect money {0}. All money {1}", tax, _d->taxValue );
    }
  }
  break;

  default:
  break;
  }

  ServiceBuilding::applyService( walker );
}

void Forum::burn()
{
  _d->removeMoney( _city() );
  ServiceBuilding::burn();
}

void Forum::collapse()
{
  _d->removeMoney( _city() );
  ServiceBuilding::collapse();
}

float Forum::collectTaxes()
{
  int save = 0;

  if( _d->taxValue > 1 )
  {
    save = floor( _d->taxValue );
    _d->taxValue -= save;
  }

  return save;
}

void Forum::Impl::removeMoney(PlayerCityPtr city)
{
  int senates_n = city->statistic().objects.count<Senate>();

  int maxMoney = city->treasury().money();
  if( maxMoney > 0 )
  {
    int forums_n = city->statistic().objects.count<Forum>();

    if( senates_n > 0 )
      maxMoney /= 2;

    maxMoney /= math::clamp( forums_n, 1, 99 );

    events::dispatch<events::Payment>( econ::Issue::moneyStolen, -maxMoney );
  }
}
