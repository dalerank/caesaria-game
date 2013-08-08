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


#include "oc3_building_military.hpp"
#include "oc3_resourcegroup.hpp"

Barracks::Barracks() : WorkingBuilding( B_BARRACKS, Size( 3 ) )
{
  setMaxWorkers(5);
  setWorkers(0);  
  setPicture( Picture::load( ResourceGroup::security, 17) );
}

FortLegionnaire::FortLegionnaire() : Building( B_FORT_LEGIONNAIRE, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 16);
  logo -> setOffset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;  
}

FortMounted::FortMounted() : Building( B_FORT_MOUNTED, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 15);
  logo -> setOffset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;
}

FortJaveline::FortJaveline() : Building( B_FORT_JAVELIN, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 14);
  //std::cout << logo->get_xoffset() << " " << logo->get_yoffset() << " " << logo->get_width() << " " << logo->get_height() << std::endl;
  logo -> setOffset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;  
}
