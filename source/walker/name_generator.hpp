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

#ifndef __CAESARIA_NAME_GENERATOR_H_INCLUDED__
#define __CAESARIA_NAME_GENERATOR_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "vfs/path.hpp"

class NameGenerator
{
public:
  typedef enum { male=0, female,
                 patricianMale, patricianFemale } NameType;
  static NameGenerator& instance();

  ~NameGenerator();

  static std::string rand( NameType type );
  void initialize(vfs::Path filename );
  void setLanguage( const std::string& language );

private:
  NameGenerator();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_NAME_GENERATOR_H_INCLUDED__
