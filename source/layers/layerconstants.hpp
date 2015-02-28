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
#include "core/scopedptr.hpp"
#include "core/variant.hpp"

namespace citylayer
{

typedef enum { simple=0, water, fire, damage, desirability,
       entertainments, entertainment, theater, amphitheater, colloseum, hippodrome,
       health, doctor, hospital, barber, baths,
       food,
       religion,
       risks, crime, aborigen, troubles,
       educations, education, school, library, academy,
       commerce, tax, market,
       build, destroyd,
       all, count=0xff
     } Type;

class Helper : public EnumsHelper<Type>
{
public:
  static Helper& instance();
  static std::string prettyName( Type t );
  static const VariantMap& getConfig( Type t );
  
protected:
  Helper();

  class Impl;
  ScopedPtr<Impl> _d;
};

}

#endif //__CAESARIA_LAYERCONSTANTS_H_INCLUDED__
