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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#ifndef __CAESARIA_PICTURE_BANK_H_INCLUDED__
#define __CAESARIA_PICTURE_BANK_H_INCLUDED__

#include "picture.hpp"
#include "good/good.hpp"
#include "core/scopedptr.hpp"
#include "core/variant.hpp"

// loads pictures from files
namespace gfx
{

class PictureBank
{
public:
  static PictureBank& instance();

  void reset();

  // set the current picture
  void setPicture(const std::string &name, const Picture& picture);

  void addAtlas(const std::string& filename);
  void loadAtlas(const std::string& filename);

  // show resource
  Picture& getPicture(const std::string &name);

  // show resource
  Picture& getPicture(const std::string &prefix, const int idx);
  bool present( const std::string& prefix,const int idx ) const;

  ~PictureBank();

private:
  PictureBank();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gfx
#endif //__CAESARIA_PICTURE_BANK_H_INCLUDED__
