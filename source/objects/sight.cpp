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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "sight.hpp"

#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "core/exception.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "walker/trainee.hpp"
#include "core/utils.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::statue_small, SmallStatue)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::statue_middle, MediumStatue)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::statue_big, BigStatue)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::triumphal_arch, TriumphalArch)

// govt     1  - small statue        1 x 1
// govt     2  - medium statue       2 x 2
// govt     3  - big statue          3 x 3

// land3a 43 44 - triumphal arch
// land3a 45 46 - triumphal arch

// transport 93 - missionaire post   2 x 2
// circus    1 ~ 18 hippodrome    5x(5 x 5)

SmallStatue::SmallStatue() : Building( objects::statue_small, Size(1) )
{
  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  setPicture( ResourceGroup::govt, 1 );
}

bool SmallStatue::isNeedRoadAccess() const {  return false; }

MediumStatue::MediumStatue() : Building( objects::statue_middle, Size(2) )
{
  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  setPicture( ResourceGroup::govt, 2);
}

bool MediumStatue::isNeedRoadAccess() const {  return false; }

BigStatue::BigStatue() : Building( objects::statue_big, Size(3))
{
  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  setPicture( ResourceGroup::govt, 3 );
}

bool BigStatue::isNeedRoadAccess() const {  return false;}

// second arch pictures is land3a 45 + 46	
TriumphalArch::TriumphalArch() : Building( objects::triumphal_arch, Size(3) )
{
  setPicture( ResourceGroup::land3a, 43 );
  _animationRef().load("land3a", 44, 1);
  _animationRef().setOffset( Point( 63, 97 ) );
  _fgPicturesRef().resize(1);
  _fgPicturesRef()[0] = _animationRef().currentFrame();
}
