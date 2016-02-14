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

#include "nation.hpp"
#include "core/enumerator.hpp"
#include "core/logger.hpp"

namespace world
{

class NationHelper : public EnumsHelper<Nation>
{
public:
  static NationHelper& Instance() { static NationHelper inst; return inst; }
  NationHelper()
    : EnumsHelper<Nation>( nation::unknown )
  {
#define REG_NATION(a) append( nation::a, TEXT(a) );
   REG_NATION(roman)
   REG_NATION(etruscan)
   REG_NATION(barbarian)
   REG_NATION(numidian)
   REG_NATION(pict)
   REG_NATION(samnite)
   REG_NATION(selecid)
   REG_NATION(carthaginian)
   REG_NATION(celt)
   REG_NATION(eygptian)
   REG_NATION(goth)
   REG_NATION(graeci)
   REG_NATION(judaean)
   REG_NATION(native)
   REG_NATION(visigoth)
   REG_NATION(gaul)
   REG_NATION(iberian)
   REG_NATION(helveti)
   REG_NATION(count)
#undef REG_NATION
  }
};

Nation toNation(const std::string& name)
{
  Nation ret = NationHelper::Instance().findType( name );
  Logger::warningIf( ret == nation::unknown, "Can't find nation type for " + name );

  return ret;
}

std::string toString(Nation nation)
{
  return NationHelper::Instance().findName( nation );
}

std::string prettyName(Nation nation)
{
  return "##wn_" + toString( nation ) + "##";
}

}//end namespace world
