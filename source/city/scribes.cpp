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

#include "scribes.hpp"
#include "city.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "gfx/tile.hpp"
#include "city/helper.hpp"
#include "good/helper.hpp"
#include "core/utils.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "game/funds.hpp"
#include "core/foreach.hpp"
#include "sentiment.hpp"
#include "cityservice_peace.hpp"
#include "core/variant_map.hpp"
#include "statistic.hpp"
#include "cityservice_military.hpp"
#include "cityservice_factory.hpp"

using namespace constants;

namespace city
{

class Scribes::Impl
{
public:
  Scribes::Messages messages;
};

Scribes::Scribes() : _d( new Impl )
{
}

Scribes::~Scribes()
{
}

const Scribes::Messages& Scribes::messages() const { return _d->messages; }

VariantMap Scribes::save() const
{
  VariantMap ret;
  ret[ "messages" ] = _d->messages.save();
  return ret;
}

void Scribes::load(const VariantMap& stream)
{
  _d->messages.load( stream.get( "messages" ).toMap() );
}

const Scribes::Message& Scribes::getMessage(unsigned int index) const
{
  return _d->messages.at( index );
}

const Scribes::Message& Scribes::readMessage(unsigned int index)
{
  Message& m = _d->messages.at( index );
  m.opened = true;
  return m;
}

void Scribes::changeMessage(int index, Message& message)
{
  Messages::iterator it = _d->messages.begin();
  std::advance( it, index );
  if( it != _d->messages.end() )
    *it = message;
}

void Scribes::removeMessage(int index)
{
  Messages::iterator it = _d->messages.begin();
  std::advance( it, index );
  if( it != _d->messages.end() )
    _d->messages.erase( it );
}

void Scribes::addMessage(const Message& message)
{
  _d->messages.push_front( message );
}

namespace {
CAESARIA_LITERALCONST(gtype)
CAESARIA_LITERALCONST(ext)
}

VariantMap Scribes::Message::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY( ret, text )
  VARIANT_SAVE_ANY( ret, title )
  ret[ literals::gtype ] = good::Helper::getTypeName( gtype );
  VARIANT_SAVE_ANY( ret, position )
  VARIANT_SAVE_ANY( ret, type )
  VARIANT_SAVE_ANY( ret, date )
  VARIANT_SAVE_ANY( ret, opened )
  ret[ literals::ext ] = ext;

  return ret;
}

void Scribes::Message::load(const VariantMap& stream)
{
  VARIANT_LOAD_STR( text, stream )
  VARIANT_LOAD_STR( title, stream )
  gtype = good::Helper::getType( stream.get( literals::gtype ).toString() );
  VARIANT_LOAD_ANY( position, stream )
  VARIANT_LOAD_ANY( type, stream )
  VARIANT_LOAD_TIME( date, stream )
  VARIANT_LOAD_ANY( opened, stream )
      ext = stream.get( literals::ext );
}

VariantMap Scribes::Messages::save() const
{
  VariantMap ret;
  int step=0;
  std::string stepName;
  foreach( i, *this )
  {
    stepName = utils::format( 0xff, "%04d", step++ );
    ret[ stepName ] = i->save();
  }

  return ret;
}

void Scribes::Messages::load(const VariantMap &vm)
{
  foreach( i, vm )
  {
    push_back( Message() );
    back().load( i->second.toMap() );
    }
}

Scribes::Message& Scribes::Messages::at(unsigned int index)
{
  static Message invalidMessage;
  Messages::iterator it = begin();
  std::advance( it, index );
  if( it != end() )
    return *it;

  return invalidMessage;
}

}//end namespace city
