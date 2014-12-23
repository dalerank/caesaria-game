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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_GAME_LOADER_H_INCLUDE_
#define _CAESARIA_GAME_LOADER_H_INCLUDE_

#include "core/scopedptr.hpp"
#include "vfs/entries.hpp"

class Game;

namespace game
{

class Loader
{
public:
  Loader();
  ~Loader();

  bool load(vfs::Path filename, Game& game);
  std::string restartFile() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace game

#endif //_CAESARIA_GAME_LOADER_H_INCLUDE_
