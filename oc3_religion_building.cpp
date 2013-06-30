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


#include "oc3_religion_building.hpp"
#include "oc3_divinity.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_positioni.hpp"

class Temple::Impl
{
public:
  RomeDivinityPtr divinity;
};

Temple::Temple( RomeDivinityPtr divinity, BuildingType type, int imgId, const Size& size )
: ServiceBuilding( divinity->getServiceType(), type, size ), _td( new Impl )
{
  _td->divinity = divinity;
  setPicture( Picture::load( ResourceGroup::security, imgId ));
}

RomeDivinityPtr Temple::getDivinity() const
{
  return _td->divinity;
}

unsigned int Temple::getWalkerDistance() const
{
  return 26;
}

TempleCeres::TempleCeres() 
: SmallTemple( DivinePantheon::ceres(), B_TEMPLE_CERES, 45 )
{
}

BigTempleCeres::BigTempleCeres() : ServiceBuilding(S_TEMPLE_CERES, B_BIG_TEMPLE_CERES, Size(3))
{
  setPicture( Picture::load( ResourceGroup::security, 46));
}

TempleNeptune::TempleNeptune() : ServiceBuilding(S_TEMPLE_NEPTUNE, B_TEMPLE_NEPTUNE, Size(2) )
{
  setPicture( Picture::load( ResourceGroup::security, 47));
}

BigTempleNeptune::BigTempleNeptune() : ServiceBuilding(S_TEMPLE_NEPTUNE, B_BIG_TEMPLE_NEPTUNE, Size(3))
{
  setPicture(Picture::load( ResourceGroup::security, 48));
}

TempleMars::TempleMars() : ServiceBuilding(S_TEMPLE_MARS, B_TEMPLE_MARS, Size(2))
{
  setPicture( Picture::load( ResourceGroup::security, 51));
}

BigTempleMars::BigTempleMars() : ServiceBuilding(S_TEMPLE_MARS, B_BIG_TEMPLE_MARS, Size(3))
{
  setPicture( Picture::load( ResourceGroup::security, 52));
}

TempleVenus::TempleVenus() : ServiceBuilding(S_TEMPLE_VENUS, B_TEMPLE_VENUS, Size(2))
{
  setPicture(Picture::load(ResourceGroup::security, 53));
}

BigTempleVenus::BigTempleVenus() : ServiceBuilding(S_TEMPLE_VENUS, B_BIG_TEMPLE_VENUS, Size(3))
{
  setPicture(Picture::load( ResourceGroup::security, 54));
}

TempleMercure::TempleMercure() : ServiceBuilding(S_TEMPLE_MERCURE, B_TEMPLE_MERCURE, Size(2))
{
  setPicture( Picture::load( ResourceGroup::security, 49));
}

BigTempleMercure::BigTempleMercure() : ServiceBuilding(S_TEMPLE_MERCURE, B_BIG_TEMPLE_MERCURE, Size(3))
{
  setPicture(Picture::load( ResourceGroup::security, 50));
}

TempleOracle::TempleOracle() : ServiceBuilding(S_TEMPLE_ORACLE, B_TEMPLE_ORACLE, Size(2) )
{
  setPicture( Picture::load( ResourceGroup::security, 55));

  _getAnimation().load( ResourceGroup::security, 56, 6);
  _getAnimation().setOffset( Point( 9, 30 ) );
  _fgPictures.resize(1);   
}

SmallTemple::SmallTemple( RomeDivinityPtr divinity, BuildingType type, int imgId ) 
  : Temple( divinity, type, imgId, Size(2) )
{
  setMaxWorkers( 2 );
}