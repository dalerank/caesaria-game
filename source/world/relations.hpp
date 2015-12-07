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

#ifndef __CAESARIA_WORLD_RELATIONS_H_INCLUDED__
#define __CAESARIA_WORLD_RELATIONS_H_INCLUDED__

#include "predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "core/signals.hpp"
#include "vfs/path.hpp"
#include "game/gift.hpp"

namespace world
{

class GiftHistory : public std::vector<Gift>
{
public:
  const Gift& last() const;
  int update( const Gift& gift );

  void load(const VariantMap& stream );
  VariantMap save() const;
private:
  int _lastSignedValue;
};

class Soldiers
{
public:
};

class Relation
{
public:
  static const Relation invalid;

  unsigned int soldiersSent;
  unsigned int lastSoldiersSent;

  DateTime lastTaxDate;
  unsigned int wrathPoint;
  unsigned int tryCount;

  int chastenerFailed;
  bool debtMessageSent;

  Relation();

  int value() const;
  void update( const Gift& gift );

  const GiftHistory& gifts() const;
  void change( float delta );
  void reset();
  void removeSoldier();

  VariantMap save() const;
  void load( const VariantMap& stream );
private:
  GiftHistory _gifts;
  float _value;
};

class Relations : public std::map< std::string, Relation >
{
public:
  VariantMap save() const;
  void load( const VariantMap& stream );

  const Relation& get( const std::string& name ) const;
};

}

#endif //__CAESARIA_WORLD_RELATIONS_H_INCLUDED__
