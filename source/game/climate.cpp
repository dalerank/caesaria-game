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

#include "climate.hpp"
#include <GameScript>
#include <GameCore>

namespace game
{

namespace climate
{

Type fromString(const std::string& value)
{
  if (value == TEXT(desert)) return desert;
  if (value == TEXT(central)) return central;
  if (value == TEXT(northen)) return northen;

  return central;
}

void initialize(Type type)
{
  VariantList vl; vl << (int)type;
  script::Core::execFunction("OnChangeClimate", vl);
}

}//end namespace climate

}//end namespace game

