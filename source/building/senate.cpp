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

#include "senate.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "game/cityfunds.hpp"
#include "walker/taxcollector.hpp"
#include "game/city.hpp"
#include "constants.hpp"
#include "core/gettext.hpp"

using namespace constants;
// govt 4  - senate
// govt 9  - advanced senate
// govt 5 ~ 8 - senate flags

class Senate::Impl
{
public:
  int taxValue;
  std::string errorStr;
};

Senate::Senate() : ServiceBuilding( Service::senate, building::senate, Size(5) ), _d( new Impl )
{
  setPicture( ResourceGroup::govt, 4 );
  _d->taxValue = 0;
}

bool Senate::canBuild(PlayerCityPtr city, const TilePos& pos ) const
{
  _d->errorStr = "";
  bool mayBuild = ServiceBuilding::canBuild( city, pos );

  if( mayBuild )
  {
    CityHelper helper( city );
    bool isSenatePresent = !helper.find<Building>(building::senate).empty();
    _d->errorStr = isSenatePresent ? _("##can_build_only_once##") : "";
    mayBuild &= !isSenatePresent;
  }

  return mayBuild;
}

void Senate::applyService(ServiceWalkerPtr walker)
{
  switch( walker->getType() )
  {
  case walker::taxCollector:
    _d->taxValue += walker.as<TaxCollector>()->getMoney();
  break;

  default:
  break;
  }

  ServiceBuilding::applyService( walker );
}

unsigned int Senate::getFunds() const
{
  return _getCity()->getFunds().getValue();
}

int Senate::collectTaxes()
{
  return _d->taxValue;
}

int Senate::getStatus(Senate::Status status) const
{
  switch(status)
  {
  case workless: return CityStatistic::getWorklessPercent( _getCity() );
  case culture: return _getCity()->getCulture();
  case prosperity: return _getCity()->getProsperity();
  case peace: return _getCity()->getPeace();
  case favour: return _getCity()->getFavour();
  }

  return 0;
}

std::string Senate::getError() const
{
  return _d->errorStr;
}

void Senate::deliverService()
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
