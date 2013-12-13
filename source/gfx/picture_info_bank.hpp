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

#ifndef __CAESARIA_PICTURE_INFO_BANK_H_INCLUDED__
#define __CAESARIA_PICTURE_INFO_BANK_H_INCLUDED__

#include "core/position.hpp"
#include "core/scopedptr.hpp"

// contains data needed for loading pictures
class PictureInfoBank
{
public:
  static PictureInfoBank& instance();
  ~PictureInfoBank();

  Point getOffset(const std::string &resource_name);   // image name ("Govt_00005")

private:
  PictureInfoBank();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_PICTURE_INFO_BANK_H_INCLUDED__
