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
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;
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
    city::Helper helper( city );
    bool isSenatePresent = !helper.find<Building>(building::senate).empty();
    _d->errorStr = isSenatePresent ? _("##can_build_only_once##") : "";
    mayBuild &= !isSenatePresent;
  }

  return mayBuild;
}

void Senate::applyService(ServiceWalkerPtr walker)
{
  switch( walker->type() )
  {
  case walker::taxCollector:
  {
    TaxCollectorPtr txcl = ptr_cast<TaxCollector>( walker );
    if( txcl.isValid() )
    {
      float tax = txcl->getMoney();;
      _d->taxValue += tax;
      Logger::warning( "Senate: collect money %f. All money %d", tax, _d->taxValue );
    }
  }
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
  _updateRatings();
}

unsigned int Senate::walkerDistance() const {  return 26; }

void Senate::_updateRatings()
{
  _fgPicturesRef()[ 0 ].setOffset( 140, -30 + getStatus( Senate::culture ) / 2 );
  _fgPicturesRef()[ 1 ].setOffset( 170, -25 + getStatus( Senate::prosperity ) / 2 );
  _fgPicturesRef()[ 2 ].setOffset( 200, -15 + getStatus( Senate::peace ) / 2 );
  _fgPicturesRef()[ 3 ].setOffset( 230, -10 + getStatus( Senate::favour ) / 2 );
}

void Senate::timeStep(const unsigned long time)
{
  if( time % GameDate::ticksInMonth() == 1 )
  {
    _updateUnemployers();
    _updateRatings();
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

int Senate::collectTaxes()
{
  int save = _d->taxValue;
  _d->taxValue = 0;
  return save;
}

unsigned int Senate::getFunds() const {  return _city()->funds().treasury(); }
std::string Senate::getError() const {  return _d->errorStr; }

int Senate::getStatus(Senate::Status status) const
{
  switch(status)
  {
  case workless: return city::Statistic::getWorklessPercent( _city() );
  case culture: return _city()->culture();
  case prosperity: return _city()->prosperity();
  case peace: return _city()->peace();
  case favour: return _city()->favour();
  }

  return 0;
}

void Senate::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    TaxCollectorPtr walker = TaxCollector::create( _city() );
    walker->setMaxDistance( walkerDistance() );
    walker->send2City( this );

    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
  }
}
