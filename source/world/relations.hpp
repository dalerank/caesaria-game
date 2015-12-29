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

class RelationAbility
{
public:
  typedef enum { request=0 } Type;
  DateTime start;
  DateTime finished;
  bool successed;
  int relation;
  std::string message;
  Type type;
  int influenceMonth;

  VariantList save() const;
  void load( const VariantList& stream );
};

class RelationAbilities : public std::vector<RelationAbility>
{
public:
  VariantList save() const;
  void load(const VariantList& stream );
};

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

class Relation
{
  friend class Emperor;
public:
  static const Relation invalid;

  Relation();

  DateTime lastTaxDate;
  unsigned int wrathPoint;
  unsigned int tryCount;

  int chastenerFailed;
  bool debtMessageSent;

  int value() const;
  void update( const Gift& gift );
  void update( const RelationAbility& ability );

  const GiftHistory& gifts() const;
  const RelationAbilities& abilities() const;
  void change( float delta );
  void reset();
  void removeSoldier();

  VariantMap save() const;
  void load( const VariantMap& stream );

private:
  struct {
    unsigned int sent = 0;
    unsigned int last = 0;
  } soldiers;

  GiftHistory _gifts;
  RelationAbilities _abilities;
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
