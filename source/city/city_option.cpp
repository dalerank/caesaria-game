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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "city_option.hpp"
#include "core/variant_list.hpp"
#include "game/difficulty.hpp"
#include "core/enumerator.hpp"

namespace city
{

class OptionEnum : public EnumsHelper<int>
{
public:
  static OptionEnum& instance()
  {
    static OptionEnum inst;
    return inst;
  }

  OptionEnum() : EnumsHelper<int>(-1)
  {
#define _O(a) append( PlayerCity::a, CAESARIA_STR_EXT(a) );
    _O(adviserEnabled);
    _O(godEnabled)
    _O(fishPlaceEnabled)
    _O(updateRoads)
    _O(forceBuild)
    _O(warningsEnabled)
    _O(updateTiles)
    _O(zoomEnabled)
    _O(zoomInvert)
    _O(fireKoeff)
    _O(barbarianAttack)
    _O(c3gameplay)
    _O(difficulty)
    _O(legionAttack)
    _O(climateType)
    _O(collapseKoeff)
    _O(highlightBuilding)
    _O(destroyEpidemicHouses)
    _O(forestFire)
    _O(forestGrow)
    _O(warfNeedTimber)
    _O(showGodsUnhappyWarn)
    _O(claypitMayCollapse)
#undef _O
  }
};

PlayerCity::OptionType findOption(const std::string& name)
{
  return (PlayerCity::OptionType)OptionEnum::instance().findType( name );
}

VariantList Options::save() const
{
  VariantList ret;
  for(auto& it : *this)
  {
    ret << Point(it.first, it.second);
  }
  return ret;
}

void Options::load(const VariantList& stream)
{
  for (auto& it : stream)
  {
    Point tmp = it;
    (*this)[ (PlayerCity::OptionType)tmp.x() ] = tmp.y();
  }

  resetIfNot( PlayerCity::climateType, game::climate::central );
  resetIfNot( PlayerCity::adviserEnabled, 1 );
  resetIfNot( PlayerCity::fishPlaceEnabled, 1 );
  resetIfNot( PlayerCity::godEnabled, 1 );
  resetIfNot( PlayerCity::zoomEnabled, 1 );
  resetIfNot( PlayerCity::zoomInvert, 1 );
  resetIfNot( PlayerCity::fireKoeff, 100 );
  resetIfNot( PlayerCity::collapseKoeff, 100 );
  resetIfNot( PlayerCity::barbarianAttack, 1 );
  resetIfNot( PlayerCity::legionAttack, 1 );
  resetIfNot( PlayerCity::c3gameplay, 0 );
  resetIfNot( PlayerCity::warfNeedTimber, 1 );
  resetIfNot( PlayerCity::difficulty, game::difficulty::usual );
}

void Options::resetIfNot( int name, int value)
{
  if( !count( name ) )
    (*this)[ name ] = value;
}

}//end namespace city
