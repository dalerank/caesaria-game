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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "info.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"

using namespace gfx;

namespace walker
{

const gfx::Picture& Info::bigPicture() const
{
  int index = -1;
  switch( _type )
  {
  case walker::immigrant: index=4; break;
  case walker::emigrant: index=9; break;
  case walker::doctor: index = 2; break;
  case walker::cartPusher: index=51; break;
  case walker::marketLady: index=12; break;
  case walker::marketKid: index=38; break;
  case walker::merchant: index=25; break;
  case walker::prefect: index=19; break;
  case walker::engineer: index=7; break;
  case walker::taxCollector: index=6; break;
  case walker::sheep: index = 54; break;
  case walker::seaMerchant: index = 61; break;
  case walker::merchantCamel : index = 25; break;
  case walker::recruter: index=13; break;
  case walker::lionTamer: index=11; break;
  default: index=8; break;
  break;
  }

  static Picture pic;
  pic.load( config::rc.bigpeople, index )
     .withFallback( config::rc.bigpeople, 1 );

  return pic;
}

}//end namespace walker
