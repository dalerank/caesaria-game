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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_EMPERORLINE_H_INCLUDED__
#define __CAESARIA_EMPERORLINE_H_INCLUDED__

#include "predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "vfs/path.hpp"

namespace world
{

class EmperorLine
{
public:
  static EmperorLine& instance();

  std::string getEmperor( DateTime time );
  VariantMap getInfo( const std::string& name ) const;

  void load(vfs::Path filename );

private:
  EmperorLine();

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace world

#endif //__CAESARIA_EMPERORLINE_H_INCLUDED__
