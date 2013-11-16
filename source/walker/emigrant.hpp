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

#ifndef __OPENCAESAR3_EMIGRANT_H_INCLUDE_
#define __OPENCAESAR3_EMIGRANT_H_INCLUDE_

#include "immigrant.hpp"
#include "core/predefinitions.hpp"

class Emigrant;
typedef SmartPtr< Emigrant > EmigrantPtr;

/** This is an immigrant coming with his stuff */
class Emigrant : public Immigrant
{
public:
  typedef enum { G_EMIGRANT_CART1 = Good::goodCount, G_EMIGRANT_CART2, CT_MAX } CartType;

  static EmigrantPtr create( PlayerCityPtr city);

  virtual void getPictureList(std::vector<Picture> &oPics);
  virtual void timeStep(const unsigned long time);

  virtual void die();

  ~Emigrant();
protected:
  virtual void _changeDirection();

protected:
  const Picture& getCartPicture();

  Emigrant( PlayerCityPtr city );
};

#endif
