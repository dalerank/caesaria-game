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

#ifndef __CAESARIA_FONT_COLLECTION_H_INCLUDED__
#define __CAESARIA_FONT_COLLECTION_H_INCLUDED__

#include <string>
#include "core/scopedptr.hpp"
#include "vfs/predefenitions.hpp"

class Font;
class NColor;

class FontCollection
{
public:
  static FontCollection& instance();

  void initialize(const vfs::Directory& resourcePath, const std::string& family );

  Font getFont(const std::string& name);  // get a saved font
  Font getFont(const std::string& family, int size, bool italic, bool bold);
  Font getDefault(int size, bool italic, bool bold);
private:
  std::string _getName(const std::string& family, int size, bool italic, bool bold);
  void _setFont(const std::string& name, Font font);
  Font _addFont(const std::string& name, vfs::Path filename, const int size, bool italic, bool bold, const NColor& color);

  FontCollection();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_FONT_COLLECTION_H_INCLUDED__
