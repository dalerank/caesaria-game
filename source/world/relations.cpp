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
#include "core/variant_list.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"

using namespace config;

namespace world
{

const Relation Relation::invalid;

Relation::Relation()
  : wrathPoint(0), tryCount( 0 ),
    debtMessageSent(false)
{
  _value = 0;
}

int Relation::value() const { return math::clamp<int>( _value, 0, emperor::maxFavor ); }

void Relation::update(const Gift &gift)
{
  int favourUpdate = _gifts.update( gift );
  _gifts.push_back( gift );
  change( favourUpdate );
}

void Relation::update(const RelationAbility& ability)
{
  _abilities.push_back( ability );
  if( _abilities.back().influenceMonth < 0 )
  {
    _abilities.back().influenceMonth = 5 * DateTime::monthsInYear;
  }
}

const GiftHistory& Relation::gifts() const { return _gifts; }
const RelationAbilities& Relation::abilities() const { return _abilities; }

void Relation::change(float delta) { _value += delta; }

void Relation::reset()
{
  _value = emperor::defaultFavor;
  lastTaxDate = game::Date::current();
}

void Relation::removeSoldier()
{
  if( soldiers.sent > 0)
    --soldiers.sent;
}

VariantMap Relation::save() const
{
  VariantMap ret;
  VARIANT_SAVE_CLASS(ret, _gifts )
  VARIANT_SAVE_ANY  (ret, lastTaxDate )
  VARIANT_SAVE_ANY  (ret, _value)
  VARIANT_SAVE_ANY  (ret, soldiers.sent )
  VARIANT_SAVE_ANY  (ret, soldiers.last)
  VARIANT_SAVE_ANY  (ret, debtMessageSent )
  VARIANT_SAVE_ANY  (ret, chastenerFailed )
  VARIANT_SAVE_ANY  (ret, wrathPoint)
  VARIANT_SAVE_ANY  (ret, tryCount)
  VARIANT_SAVE_CLASS(ret, _abilities)

  return ret;
}

void Relation::load(const VariantMap &stream)
{
  VARIANT_LOAD_CLASS(_gifts,           stream)
  VARIANT_LOAD_ANY  (_value,           stream)
  VARIANT_LOAD_ANY  (soldiers.last,    stream)
  VARIANT_LOAD_ANY  (soldiers.sent,    stream)
  VARIANT_LOAD_ANY  (debtMessageSent,  stream)
  VARIANT_LOAD_ANY  (chastenerFailed,  stream)
  VARIANT_LOAD_ANY  (wrathPoint,       stream)
  VARIANT_LOAD_ANY  (tryCount,         stream)
  VARIANT_LOAD_CLASS_LIST(_abilities,  stream)
}

VariantMap Relations::save() const
{
  VariantMap ret;
  for( const auto& it : *this )
    ret[ it.first ] = it.second.save();

  return ret;
}

const Relation& Relations::get(const std::string &name) const
{
  static const Relation invalid;

  auto it = find( name );
  return  it != end() ? it->second : invalid;
}

void Relations::load(const VariantMap &stream)
{
  for( auto& item : stream )
  {
    Relation r;
    r.load( item.second.toMap() );

    (*this)[ item.first ] = r;
  }
}

const Gift& GiftHistory::last() const
{
  return empty() ? Gift::invalid : back();
}

int GiftHistory::update(const Gift& gift)
{
  int value = gift.value();
  if( value <= 0 )
  {
    Logger::warning( "!!! WARNING: Relation update with 0 value from " + gift.sender() );
    value = 1;
  }

  int monthFromLastGift = math::clamp<int>( last().date().monthsTo( game::Date::current() ),
                                            0, (int)DateTime::monthsInYear );

  float timeKoeff = monthFromLastGift / (float)DateTime::monthsInYear;
  float affectMoney = (float)last().value() / ( monthFromLastGift + 1 );
  float moneyKoeff = math::max<float>( value - affectMoney, 0.f ) / value;
  float favourUpdate = emperor::maxFavourUpdate * timeKoeff * moneyKoeff;
  favourUpdate = math::clamp<float>( favourUpdate, 0.f, emperor::maxFavourUpdate);

  _lastSignedValue = math::max<int>( value, last().value() );
  return favourUpdate;
}

void GiftHistory::load(const VariantMap& stream)
{

  VARIANT_LOAD_ANY( _lastSignedValue, stream )
  VariantList gifts = stream.get( "gifts" ).toList();

  for( const auto& item : gifts )
  {
    push_back( Gift() );
    back().load( item.toMap() );
  }
}

VariantMap GiftHistory::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY( ret, _lastSignedValue )

  VariantList gifts;
  for( const auto& gift : *this )
    gifts << gift.save();

  ret[ "gifts" ] = gifts;
  return ret;
}

VariantList RelationAbilities::save() const
{
  VariantList ret;
  for( const auto& item : *this )
    ret << item.save();

  return ret;
}

void RelationAbilities::load(const VariantList& stream)
{
  for( const auto& item : stream )
  {
    RelationAbility state;
    state.load( item.toList() );
    push_back( state );
  }
}

VariantList RelationAbility::save() const
{
  VariantList ret;
  ret << start << finished
      << successed << relation
      << message << (int)type
      << influenceMonth;
  return ret;
}

void RelationAbility::load(const VariantList& stream)
{
  VariantListReader reader( stream );
  start = reader.next().toDateTime();
  finished = reader.next().toDateTime();
  successed = reader.next();
  relation = reader.next();
  message = reader.next().toString();
  type = reader.next().toEnum<Type>();
  influenceMonth = reader.next();
}

}//end namespace world
