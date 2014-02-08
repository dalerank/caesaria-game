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

#include "senate.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "city/funds.hpp"
#include "walker/taxcollector.hpp"
#include "city/helper.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "core/gettext.hpp"
#include "game/gamedate.hpp"

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

  _fgPicturesRef().resize( 8 );
  _fgPicturesRef()[ 0 ] = Picture::load( ResourceGroup::govt, 5 );
  _fgPicturesRef()[ 0 ].setOffset( 140, -30 );
  _fgPicturesRef()[ 1 ] = Picture::load( ResourceGroup::govt, 6 );
  _fgPicturesRef()[ 1 ].setOffset( 170, -25 );
  _fgPicturesRef()[ 2 ] = Picture::load( ResourceGroup::govt, 7 );
  _fgPicturesRef()[ 2 ].setOffset( 200, -15 );
  _fgPicturesRef()[ 3 ] = Picture::load( ResourceGroup::govt, 8 );
  _fgPicturesRef()[ 3 ].setOffset( 230, -10 );
}

bool Senate::canBuild( PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles ) const
{
  _d->errorStr = "";
  bool mayBuild = ServiceBuilding::canBuild( city, pos, aroundTiles );

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
    _d->taxValue += ptr_cast<TaxCollector>( walker )->getMoney();
  break;

  default:
  break;
  }

  ServiceBuilding::applyService( walker );
}

void Senate::build(PlayerCityPtr city, const TilePos& pos)
{
  ServiceBuilding::build( city, pos );
  _updateUnemployers();
}

unsigned int Senate::getWalkerDistance() const {  return 26; }

void Senate::timeStep(const unsigned long time)
{
  if( time % GameDate::getTickInMonth() == 0 )
  {
    _updateUnemployers();

    _fgPicturesRef()[ 0 ].setOffset( 140, -30 + getStatus( Senate::culture ) / 2 );
    _fgPicturesRef()[ 1 ].setOffset( 170, -25 + getStatus( Senate::prosperity ) / 2 );
    _fgPicturesRef()[ 2 ].setOffset( 200, -15 + getStatus( Senate::peace ) / 2 );
    _fgPicturesRef()[ 3 ].setOffset( 230, -10 + getStatus( Senate::favour ) / 2 );
  }

  ServiceBuilding::timeStep( time );
}

void Senate::_updateUnemployers()
{
  Point offsets[] = { Point( 80, -15), Point( 90, -20), Point( 110, -30 ), Point( 120, -10 ) };
  int workless = getStatus( Senate::workless );
  for( int k=0; k < 4; k++ )
  {
    Picture pic;
    if( k * 5 < workless )
    {
      pic = Picture::load( ResourceGroup::transport, 87 );
      pic.setOffset( offsets[ k ] );
    }
    _fgPicturesRef()[ 4 + k ] = pic;
  }
}

unsigned int Senate::getFunds() const {  return _getCity()->getFunds().getValue(); }
int Senate::collectTaxes() {  return _d->taxValue; }
std::string Senate::getError() const {  return _d->errorStr; }

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

void Senate::deliverService()
{
  if( getWorkersCount() > 0 && getWalkers().size() == 0 )
  {
    TaxCollectorPtr walker = TaxCollector::create( _getCity() );
    walker->setMaxDistance( getWalkerDistance() );
    walker->send2City( this );

    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
  }
}
