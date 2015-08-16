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

#include "relations.hpp"
#include "core/foreach.hpp"
#include "game/gift.hpp"
#include "config.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"

using namespace config;

namespace world
{

Relation::Relation()
  : soldiersSent( 0 ), lastSoldiersSent( 0 ),
    wrathPoint(0), tryCount( 0 ),
    debtMessageSent(false)
{
  _value = 0;
}

int Relation::value() const { return math::clamp<int>( _value, 0, emperor::maxFavor ); }

void Relation::update(const Gift &gift)
{
  int value = gift.value();
  if( value <= 0 )
  {
    Logger::warning( "!!! WARNING: Relation update with 0 value from " + gift.sender() );
    value = 1;
  }

  int monthFromLastGift = math::clamp<int>( lastGift.date().monthsTo( game::Date::current() ),
                                            0, (int)DateTime::monthsInYear );

  float timeKoeff = monthFromLastGift / (float)DateTime::monthsInYear;
  float affectMoney = (float)lastGift.value() / ( monthFromLastGift + 1 );
  float moneyKoeff = math::max<float>( value - affectMoney, 0.f ) / value;
  float favourUpdate = emperor::maxFavourUpdate * timeKoeff * moneyKoeff;
  favourUpdate = math::clamp<float>( favourUpdate, 0.f, emperor::maxFavourUpdate);

  Gift maxValueGift( "", "", math::max<int>( value, lastGift.value() ) );
  lastGift = maxValueGift;

  change( favourUpdate );
}

void Relation::change(float delta) { _value += delta; }

void Relation::reset()
{
  _value = emperor::defaultFavor;
  lastGift = Gift();
  lastTaxDate = game::Date::current();
}

void Relation::removeSoldier()
{
  if( soldiersSent > 0)
    --soldiersSent;
}

VariantMap Relation::save() const
{
  VariantMap ret;
  VARIANT_SAVE_CLASS(ret, lastGift )
  VARIANT_SAVE_ANY  (ret, lastTaxDate )
  VARIANT_SAVE_ANY  (ret, _value)
  VARIANT_SAVE_ANY  (ret, soldiersSent )
  VARIANT_SAVE_ANY  (ret, lastSoldiersSent)
  VARIANT_SAVE_ANY  (ret, debtMessageSent )
  VARIANT_SAVE_ANY  (ret, chastenerFailed )
  VARIANT_SAVE_ANY  (ret, wrathPoint)
  VARIANT_SAVE_ANY  (ret, tryCount)

  return ret;
}

void Relation::load(const VariantMap &stream)
{
  VARIANT_LOAD_CLASS(lastGift,         stream)
  VARIANT_LOAD_ANY  (_value,            stream)
  VARIANT_LOAD_ANY  (lastSoldiersSent, stream)
  VARIANT_LOAD_ANY  (soldiersSent,     stream)
  VARIANT_LOAD_ANY  (debtMessageSent,  stream)
  VARIANT_LOAD_ANY  (chastenerFailed,  stream)
  VARIANT_LOAD_ANY  (wrathPoint,       stream)
  VARIANT_LOAD_ANY  (tryCount,         stream)
}

VariantMap Relations::save() const
{
  VariantMap ret;
  for( auto it : *this )
    ret[ it.first ] = it.second.save();

  return ret;
}

const Relation& Relations::get(const std::string &name) const
{
  static const Relation invalid;

  Relations::const_iterator it = find( name );
  return  it != end() ? it->second : invalid;
}

void Relations::load(const VariantMap &stream)
{
  for( auto item : stream )
  {
    Relation r;
    r.load( item.second.toMap() );

    (*this)[ item.first ] = r;
  }
}

}//end namespace world
