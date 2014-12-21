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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_IMMIGRANT_H_INCLUDE_
#define __CAESARIA_IMMIGRANT_H_INCLUDE_

#include "emigrant.hpp"
#include "core/predefinitions.hpp"

class Immigrant;
typedef SmartPtr< Immigrant > ImmigrantPtr;

/** This is an immigrant coming with his stuff */
class Immigrant : public Emigrant
{
public:
  typedef enum { G_EMIGRANT_CART1=0, G_EMIGRANT_CART2, CT_MAX } CartType;

  static ImmigrantPtr create( PlayerCityPtr city);

  virtual void getPictures( gfx::Pictures &oPics);
  virtual void timeStep(const unsigned long time);

  virtual bool die();

  virtual ~Immigrant();
protected:
  virtual void _changeDirection();
  virtual void _updateThoughts();

protected:
  gfx::Animation& _cart();

  Immigrant( PlayerCityPtr city );
};

#endif //__CAESARIA_IMMIGRANT_H_INCLUDE_
