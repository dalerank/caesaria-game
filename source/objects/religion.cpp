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

#include "religion.hpp"
#include "city/helper.hpp"
#include "religion/pantheon.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace religion;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::small_ceres_temple, TempleCeres)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::small_mars_temple, TempleMars)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::small_mercury_temple, TempleMercury)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::small_neptune_temple, TempleNeptune)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::small_venus_temple, TempleVenus)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::big_ceres_temple, BigTempleCeres)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::big_mars_temple, BigTempleMars)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::big_mercury_temple, BigTempleMercury)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::big_neptune_temple, BigTempleNeptune)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::big_venus_temple, BigTempleVenus)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::oracle, TempleOracle)

class Temple::Impl
{
public:
  DivinityPtr divinity;
};

Temple::Temple( DivinityPtr divinity, TileOverlay::Type type, int imgId, const Size& size )
: ServiceBuilding( divinity.isValid()
                    ? divinity->serviceType()
                    : Service::srvCount, type, size ), _td( new Impl )
{
  _td->divinity = divinity;
  setPicture( ResourceGroup::security, imgId );
  _fgPicturesRef().resize( 1 );
}

DivinityPtr Temple::divinity() const {  return _td->divinity; }

void Temple::deliverService()
{
  if( walkers().empty() && numberWorkers() > 0 )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int Temple::walkerDistance() const { return 26;}

Temple::~Temple(){}

TempleCeres::TempleCeres() : SmallTemple( rome::Pantheon::ceres(), objects::small_ceres_temple, 45 )
{
}

BigTempleCeres::BigTempleCeres() : BigTemple( rome::Pantheon::ceres(), objects::big_ceres_temple, 46 )
{
}

TempleNeptune::TempleNeptune() : SmallTemple( rome::Pantheon::neptune(), objects::small_neptune_temple, 47 )
{
}

BigTempleNeptune::BigTempleNeptune() : BigTemple( rome::Pantheon::neptune(), objects::big_neptune_temple, 48 )
{
}

TempleMars::TempleMars() : SmallTemple( rome::Pantheon::mars(), objects::small_mars_temple, 51 )
{
}

BigTempleMars::BigTempleMars() : BigTemple( rome::Pantheon::mars(), objects::big_mars_temple, 52 )
{
}

TempleVenus::TempleVenus() : SmallTemple( rome::Pantheon::venus(), objects::small_venus_temple, 53 )
{
}

BigTempleVenus::BigTempleVenus() : BigTemple( rome::Pantheon::venus(), objects::big_venus_temple, 54 )
{
}

TempleMercury::TempleMercury() : SmallTemple( rome::Pantheon::mercury(), objects::small_mercury_temple, 49 )
{
}

BigTempleMercury::BigTempleMercury() : BigTemple( rome::Pantheon::mercury(), objects::big_mercury_temple, 50 )
{
}

TempleOracle::TempleOracle() : BigTemple( DivinityPtr(), objects::oracle, 55 )
{
  setSize( Size( 2 ) );
  _animationRef().load( ResourceGroup::security, 56, 6);
  _animationRef().setOffset( Point( 9, 30 ) );
  _fgPicturesRef().resize(1);
}

unsigned int TempleOracle::parishionerNumber() const {  return 500; }

bool TempleOracle::build( const CityAreaInfo& info )
{
  BigTemple::build( info );

  if( isDeleted() )
  {
    _setError( "##oracle_need_2_cart_marble##" );
    return false;
  }

  return true;
}

SmallTemple::SmallTemple( DivinityPtr divinity, TileOverlay::Type type, int imgId )
  : Temple( divinity, type, imgId, Size(2) )
{
  setMaximumWorkers( 2 );
}

unsigned int SmallTemple::parishionerNumber() const {  return 750; }

BigTemple::BigTemple( DivinityPtr divinity, TileOverlay::Type type, int imgId )
  : Temple( divinity, type, imgId, Size(3) )
{
  setMaximumWorkers( 8 );
}

unsigned int BigTemple::parishionerNumber() const {  return 1500; }

bool BigTemple::build( const CityAreaInfo& info )
{  
  if( info.city->getOption( PlayerCity::forceBuild ) > 0 )  //load from savefiles
  {
    Temple::build( info );
    return true;
  }

  city::statistic::GoodsMap goods = city::statistic::getGoodsMap( info.city, false );
  if( goods[ good::marble ] >= 2 )
  {
    Temple::build( info );
  }
  else
  {
    _setError( "##need_marble_for_large_temple##" );
    deleteLater();
    return false;
  }

  return true;
}
