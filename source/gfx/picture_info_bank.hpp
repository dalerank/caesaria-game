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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_PICTURE_INFO_BANK_H_INCLUDED__
#define __CAESARIA_PICTURE_INFO_BANK_H_INCLUDED__

#include "core/position.hpp"
#include "core/scopedptr.hpp"
#include "core/singleton.hpp"
#include "vfs/path.hpp"

// contains data needed for loading pictures
class PictureInfoBank : public StaticSingleton<PictureInfoBank>
{
  friend class StaticSingleton;
public:
  typedef enum { walkerOffset=0, tileOffset } OffsetType;
  ~PictureInfoBank();

  void initialize( vfs::Path filename );
  Point getDefaultOffset( OffsetType type ) const;
  Point getOffset(const std::string &resource_name);
  void setOffset(const std::string &preffix, const int index, const Point& data);
  void setOffset(const std::string &preffix, const int index, const int count, const Point& data);

private:
  PictureInfoBank();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_PICTURE_INFO_BANK_H_INCLUDED__
