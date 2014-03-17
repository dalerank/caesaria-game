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

#include "hippodrome.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "gfx/picture.hpp"

using namespace constants;

Hippodrome::Hippodrome() : EntertainmentBuilding(Service::hippodrome, building::hippodrome, Size(5) )
{
  setPicture( ResourceGroup::hippodrome, 5 );
  Picture logo = Picture::load( ResourceGroup::hippodrome, 3);
  Picture logo1 = Picture::load( ResourceGroup::hippodrome, 1);
  logo.setOffset( Point( 150,181 ) );
  logo1.setOffset( Point( 300,310 ) );
  _fgPicturesRef().resize(5);
  _fgPicturesRef()[0] = logo;
  _fgPicturesRef()[1] = logo1;

  _addNecessaryWalker( walker::charioter );
}

std::string Hippodrome::troubleDesc() const
{
  std::string ret = EntertainmentBuilding::troubleDesc();

  if( ret.empty() )
  {
    ret = isRacesCarry() ? "##trouble_hippodrome_full_work##" : "##trouble_hippodrome_no_charioters##";
  }

  return ret;
}

bool Hippodrome::isRacesCarry() const{ return getTraineeValue( walker::charioter ) > 0; }
