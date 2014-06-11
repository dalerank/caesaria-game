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

#ifndef __CAESARIA_LAYERCONSTANTS_H_INCLUDED__
#define __CAESARIA_LAYERCONSTANTS_H_INCLUDED__

#include "core/enumerator.hpp"

namespace citylayer
{

typedef enum { simple=0, water, fire, damage, desirability,
       entertainments, entertainment, theater, amphitheater, colloseum, hippodrome,
       health, doctor, hospital, barber, baths,
       food,
       religion,
       risks, crime, aborigen, troubles,
       educations, education, school, library, academy,
       commerce, tax,
       build, destroy,
       all, count=0xff
     } Type;

class Helper : public EnumsHelper<Type>
{
public:
  static Helper& instance()
  {
    static Helper inst;
    return inst;
  }

protected:
  Helper() : EnumsHelper<Type>( count )
  {
#define __REG_LAYER(a) append( citylayer::a, "##ovrm_"CAESARIA_STR_EXT(a)"##" );
    __REG_LAYER(simple)
    __REG_LAYER(water)
    __REG_LAYER(fire)
    __REG_LAYER(damage)
    __REG_LAYER(desirability)
    __REG_LAYER(entertainments)
    __REG_LAYER(entertainment)
    __REG_LAYER(theater)
    __REG_LAYER(amphitheater)
    __REG_LAYER(colloseum)
    __REG_LAYER(hippodrome)
    __REG_LAYER(health)
    __REG_LAYER(doctor)
    __REG_LAYER(hospital)
    __REG_LAYER(barber)
    __REG_LAYER(baths)
    __REG_LAYER(food)
    __REG_LAYER(religion)
    __REG_LAYER(risks)
    __REG_LAYER(crime)
    __REG_LAYER(aborigen)
    __REG_LAYER(troubles)
    __REG_LAYER(educations)
    __REG_LAYER(education)
    __REG_LAYER(school)
    __REG_LAYER(library)
    __REG_LAYER(academy)
    __REG_LAYER(commerce)
    __REG_LAYER(tax)
    __REG_LAYER(build)
    __REG_LAYER(destroy)
    __REG_LAYER(all)
#undef __REG_LAYER
  }
};

}

#endif //__CAESARIA_LAYERCONSTANTS_H_INCLUDED__
