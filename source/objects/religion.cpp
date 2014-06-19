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

using namespace constants;
using namespace religion;

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

DivinityPtr Temple::getDivinity() const {  return _td->divinity; }

void Temple::deliverService()
{
  if( walkers().empty() && numberWorkers() > 0 )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int Temple::walkerDistance() const {  return 26;}

Temple::~Temple(){}

TempleCeres::TempleCeres() : SmallTemple( rome::Pantheon::ceres(), building::templeCeres, 45 )
{
}

BigTempleCeres::BigTempleCeres() : BigTemple( rome::Pantheon::ceres(), building::cathedralCeres, 46 )
{
}

TempleNeptune::TempleNeptune() : SmallTemple( rome::Pantheon::neptune(), building::templeNeptune, 47 )
{
}

BigTempleNeptune::BigTempleNeptune() : BigTemple( rome::Pantheon::neptune(), building::cathedralNeptune, 48 )
{
}

TempleMars::TempleMars() : SmallTemple( rome::Pantheon::mars(), building::templeMars, 51 )
{
}

BigTempleMars::BigTempleMars() : BigTemple( rome::Pantheon::mars(), building::cathedralMars, 52 )
{
}

TempleVenus::TempleVenus() : SmallTemple( rome::Pantheon::venus(), building::templeVenus, 53 )
{
}

BigTempleVenus::BigTempleVenus() : BigTemple( rome::Pantheon::venus(), building::cathedralVenus, 54 )
{
}

TempleMercure::TempleMercure() : SmallTemple( rome::Pantheon::mercury(), building::templeMercury, 49 )
{
}

BigTempleMercure::BigTempleMercure() : BigTemple( rome::Pantheon::mercury(), building::cathedralMercury, 50 )
{
}

unsigned int BigTempleMercure::parishionerNumber() const
{
  return 300;
}

TempleOracle::TempleOracle() : BigTemple( DivinityPtr(), building::oracle, 55 )
{
  setSize( Size( 2 ) );
  _animationRef().load( ResourceGroup::security, 56, 6);
  _animationRef().setOffset( Point( 9, 30 ) );
  _fgPicturesRef().resize(1);
}

unsigned int TempleOracle::parishionerNumber() const {  return 500; }

void TempleOracle::build(PlayerCityPtr city, const TilePos& pos)
{
  BigTemple::build( city, pos );

  if( isDeleted() )
  {
    _setError( "##oracle_need_2_cart_marble##" );
  }
}

SmallTemple::SmallTemple( DivinityPtr divinity, TileOverlay::Type type, int imgId )
  : Temple( divinity, type, imgId, Size(2) )
{
  setMaximumWorkers( 2 );
}

unsigned int SmallTemple::parishionerNumber() const {  return 150;}

BigTemple::BigTemple( DivinityPtr divinity, TileOverlay::Type type, int imgId )
  : Temple( divinity, type, imgId, Size(3) )
{
  setMaximumWorkers( 8 );
}

unsigned int BigTemple::parishionerNumber() const {  return 300;}

void BigTemple::build(PlayerCityPtr city, const TilePos& pos)
{
  city::Statistic::GoodsMap goods = city::Statistic::getGoodsMap( city );
  if( goods[ Good::marble ] >= 2 )
  {
    Temple::build( city, pos );
  }
  else
  {
    _setError( "##need_marble_for_large_temple##" );
    deleteLater();
  }
}
