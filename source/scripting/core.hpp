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

#ifndef _CAESARIA_SCRIPTING_INCLUDE_H_
#define _CAESARIA_SCRIPTING_INCLUDE_H_

#include "core/namedtype.hpp"
#include <string>

class Game;
class VariantList;

namespace script
{

class Core
{
public:
  static Core& instance();
  static void loadModule( const std::string& path );
  static void execFunction(const std::string& funcname);
  static void execFunction(const std::string& funcname,
                           const VariantList& params);                           
  static void registerFunctions(Game& game);
  static void unref(const std::string& ref);

private:
  Core();
};

} //end namespace game

#endif  //_CAESARIA_SCRIPTING_INCLUDE_H_
