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
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.7//
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_OC3SAVE_LOADER_H_INCLUDED__
#define __CAESARIA_OC3SAVE_LOADER_H_INCLUDED__

#include "abstractloader.hpp"
#include "core/scopedptr.hpp"

class Game;

namespace game
{

namespace loader
{

class OC3 : public Base
{
public:
  OC3();

  virtual bool load(const std::string& filename, Game &game);
  virtual int  climateType(const std::string& filename);
  virtual bool isLoadableFileExtension( const std::string& filename );
  virtual std::string restartFile() const;

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace loader

}//end namespace game
#endif //__CAESARIA_OC3SAVE_LOADER_H_INCLUDED__
