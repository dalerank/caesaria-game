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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_GOODHELPER_H_INCLUDED__
#define __CAESARIA_GOODHELPER_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "gfx/picture.hpp"
#include "good.hpp"
#include "core/direction.hpp"

class GoodStock;

class GoodHelper
{
public:
  static GoodHelper& getInstance();

  static std::string name( Good::Type type );
  static gfx::Picture picture( Good::Type type, bool emp=false );
  static Good::Type getType( const std::string& name );
  static std::string getTypeName( Good::Type type );
  static gfx::Picture getCartPicture( const GoodStock& stock, constants::Direction direction );
  ~GoodHelper();
private:
  GoodHelper();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_GOODHELPER_H_INCLUDED__
