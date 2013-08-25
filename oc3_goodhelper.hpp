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


#ifndef __OPENCAESAR3_GOODHELPER_H_INCLUDED__
#define __OPENCAESAR3_GOODHELPER_H_INCLUDED__

#include "oc3_scopedptr.hpp"
#include "oc3_enums.hpp"
#include "oc3_picture.hpp"
#include "oc3_good.hpp"

class GoodStock;

class GoodHelper
{
public:
  static GoodHelper& getInstance();

  static std::string getName( Good::Type type );
  static Picture getPicture( Good::Type type, bool emp=false );
  static Good::Type getType( const std::string& name );
  static std::string getTypeName( Good::Type type );
  static const Picture& getCartPicture( const GoodStock& stock, const DirectionType& direction );
  ~GoodHelper();
private:
  GoodHelper();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
