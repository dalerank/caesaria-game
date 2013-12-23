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

#include "forum.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/picture.hpp"
#include "walker/taxcollector.hpp"
#include "game/city.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "walker/constants.hpp"

using namespace constants;

class Forum::Impl
{
public:
  int taxValue;
};

Forum::Forum() : ServiceBuilding(Service::forum, building::forum, Size(2)), _d( new Impl )
{
  _d->taxValue = 0;
  setPicture( ResourceGroup::govt, 10 );
}

void Forum::deliverService()
{
  if( getWorkersCount() > 0 && getWalkers().size() == 0 )
  {
    TaxCollectorPtr walker = TaxCollector::create( _getCity() );
    walker->send2City( this );

    if( !walker->isDeleted() )
    {
      addWalker( walker.as<Walker>() );
    }
  }
}

void Forum::applyService(ServiceWalkerPtr walker)
{
  switch( walker->getType() )
  {
  case walker::taxCollector:
    _d->taxValue += walker.as<TaxCollector>()->getMoney();
  break;

  default:
  break;
  }

  ServiceBuilding::evaluateService( walker );
}

int Forum::collectTaxes()
{
  int taxes = _d->taxValue;
  _d->taxValue = 0;
  return taxes;
}
