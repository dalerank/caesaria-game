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
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::senate, Senate)

class Senate::Impl
{
public:
  float taxValue;
  std::string errorStr;
};

Senate::Senate() : ServiceBuilding( Service::senate, objects::senate, Size(5) ), _d( new Impl )
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

bool Senate::canBuild( const CityAreaInfo& areaInfo ) const
{
  _d->errorStr = "";
  bool mayBuild = ServiceBuilding::canBuild( areaInfo );

  if( mayBuild )
  {
    city::Helper helper( areaInfo.city );
    bool isSenatePresent = !helper.find<Building>(objects::senate).empty();
    _d->errorStr = isSenatePresent ? _("##can_build_only_one_of_building##") : "";
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
      float tax = txcl->takeMoney();;
      _d->taxValue += tax;
      Logger::warning( "Senate: collect money %f. All money %f", tax, _d->taxValue );
    }
  }
  break;

  default:
  break;
  }

  ServiceBuilding::applyService( walker );
}

bool Senate::build( const CityAreaInfo& info )
{
  ServiceBuilding::build( info );
  _updateUnemployers();
  _updateRatings();

  return true;
}

unsigned int Senate::walkerDistance() const { return 26; }

void Senate::_updateRatings()
{
  _fgPicturesRef()[ 0 ].setOffset( 140, -30 + status( Senate::culture ) / 2 );
  _fgPicturesRef()[ 1 ].setOffset( 170, -25 + status( Senate::prosperity ) / 2 );
  _fgPicturesRef()[ 2 ].setOffset( 200, -15 + status( Senate::peace ) / 2 );
  _fgPicturesRef()[ 3 ].setOffset( 230, -10 + status( Senate::favour ) / 2 );
}

void Senate::timeStep(const unsigned long time)
{
  if( game::Date::isMonthChanged() )
  {
    _updateUnemployers();
    _updateRatings();
  }

  ServiceBuilding::timeStep( time );
}

void Senate::_updateUnemployers()
{
  Point offsets[] = { Point( 80, -15), Point( 90, -20), Point( 110, -30 ), Point( 120, -10 ) };
  int workless = status( Senate::workless );
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

float Senate::collectTaxes()
{
  int save = 0;

  if( _d->taxValue > 1 )
  {
    save = floor( _d->taxValue );
    _d->taxValue -= save;
  }
  return save;
}

unsigned int Senate::funds() const {  return _city()->funds().treasury(); }
std::string Senate::errorDesc() const {  return _d->errorStr; }

int Senate::status(Senate::Status status) const
{
  switch(status)
  {
  case workless: return city::statistic::getWorklessPercent( _city() );
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
    walker->send2City( this, TaxCollector::goLowerService|TaxCollector::anywayWhenFailed );

    if( !walker->isDeleted() )
    {
      addWalker( walker.object() );
    }
  }
}
