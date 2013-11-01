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


#include "religion.hpp"
#include "game/divinity.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "constants.hpp"

using namespace constants;

class Temple::Impl
{
public:
  RomeDivinityPtr divinity;
};

Temple::Temple( RomeDivinityPtr divinity, TileOverlay::Type type, int imgId, const Size& size )
: ServiceBuilding( divinity->getServiceType(), type, size ), _td( new Impl )
{
  _td->divinity = divinity;
  setPicture( Picture::load( ResourceGroup::security, imgId ));
}

RomeDivinityPtr Temple::getDivinity() const
{
  return _td->divinity;
}

void Temple::deliverService()
{
  if( getWalkerList().empty() && getWorkers() > 0 )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int Temple::getWalkerDistance() const
{
  return 26;
}

Temple::~Temple()
{

}

TempleCeres::TempleCeres() : SmallTemple( DivinePantheon::ceres(), building::B_TEMPLE_CERES, 45 )
{
}

BigTempleCeres::BigTempleCeres() : BigTemple( DivinePantheon::ceres(), building::B_BIG_TEMPLE_CERES, 46 )
{
}

TempleNeptune::TempleNeptune() : SmallTemple( DivinePantheon::neptune(), building::B_TEMPLE_NEPTUNE, 47 )
{
}

BigTempleNeptune::BigTempleNeptune() : BigTemple( DivinePantheon::neptune(), building::B_BIG_TEMPLE_NEPTUNE, 48 )
{
}

TempleMars::TempleMars() : SmallTemple( DivinePantheon::mars(), building::B_TEMPLE_MARS, 51 )
{
}

BigTempleMars::BigTempleMars() : BigTemple( DivinePantheon::mars(), building::B_BIG_TEMPLE_MARS, 52 )
{
}

TempleVenus::TempleVenus() : SmallTemple( DivinePantheon::venus(), building::B_TEMPLE_VENUS, 53 )
{
}

BigTempleVenus::BigTempleVenus() : BigTemple( DivinePantheon::venus(), building::B_BIG_TEMPLE_VENUS, 54 )
{
}

TempleMercure::TempleMercure() : SmallTemple( DivinePantheon::mercury(), building::B_TEMPLE_MERCURE, 49 )
{
}

BigTempleMercure::BigTempleMercure() : BigTemple( DivinePantheon::mercury(), building::B_BIG_TEMPLE_MERCURE, 50 )
{
}

unsigned int BigTempleMercure::getParishionerNumber() const
{
  return 300;
}

TempleOracle::TempleOracle() : Temple( RomeDivinityPtr(), building::B_TEMPLE_ORACLE, 55, Size(2) )
{
  _getAnimation().load( ResourceGroup::security, 56, 6);
  _getAnimation().setOffset( Point( 9, 30 ) );
  _getForegroundPictures().resize(1);
}

unsigned int TempleOracle::getParishionerNumber() const
{
  return 500;
}

SmallTemple::SmallTemple( RomeDivinityPtr divinity, TileOverlay::Type type, int imgId )
  : Temple( divinity, type, imgId, Size(2) )
{
  setMaxWorkers( 2 );
}

unsigned int SmallTemple::getParishionerNumber() const
{
  return 150;
}

BigTemple::BigTemple( RomeDivinityPtr divinity, TileOverlay::Type type, int imgId )
  : Temple( divinity, type, imgId, Size(3) )
{
  setMaxWorkers( 8 );
}

unsigned int BigTemple::getParishionerNumber() const
{
  return 300;
}
