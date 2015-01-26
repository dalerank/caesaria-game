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

#include "goverment.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "objects_factory.hpp"

using namespace constants;
REGISTER_CLASS_IN_OVERLAYFACTORY( objects::governorHouse, GovernorsHouse)
REGISTER_CLASS_IN_OVERLAYFACTORY( objects::governorVilla, GovernorsVilla)
REGISTER_CLASS_IN_OVERLAYFACTORY( objects::governorPalace, GovernorsPalace)

// housng1a 46 - governor's house    3 x 3
// housng1a 47 - governor's villa    4 x 4
// housng1a 48 - governor's palace   5 x 5

GovernorsHouse::GovernorsHouse() : WorkingBuilding( objects::governorHouse, Size(3) )
{
  setMaximumWorkers(5);
  setPicture( ResourceGroup::housing, 46 );
}

GovernorsVilla::GovernorsVilla() : WorkingBuilding( objects::governorVilla, Size(4) )
{
  setMaximumWorkers(10);
  setWorkers(0);    
  setPicture( ResourceGroup::housing, 47 );
}

GovernorsPalace::GovernorsPalace() : WorkingBuilding( objects::governorPalace, Size( 5 ) )
{
  setMaximumWorkers(15);
  setWorkers(0);  
  setPicture( ResourceGroup::housing, 48 );
}
