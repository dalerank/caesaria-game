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

namespace city
{

class Scribes::Impl
{
public:
  bool unreadMessage_n;
  Scribes::Messages messages;

  Signal1<int> onChangeMessageNumberSignal;

  void checkUnreadMessages();
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
  VARIANT_SAVE_CLASS_D( ret, _d, messages );
  return ret;
}

void Scribes::load(const VariantMap& stream)
{
  VARIANT_LOAD_CLASS_D( _d, messages, stream );
}

const Scribes::Message& Scribes::getMessage(unsigned int index) const
{
  return _d->messages.at( index );
}

Signal1<int>& Scribes::onChangeMessageNumber()
{
  return _d->onChangeMessageNumberSignal;
}

const Scribes::Message& Scribes::readMessage(unsigned int index)
{
  Message& m = _d->messages.at( index );
  m.opened = true;
  _d->checkUnreadMessages();
  return m;
}

bool Scribes::haveUnreadMessage() const { return _d->unreadMessage_n > 0; }

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

  _d->checkUnreadMessages();
}

void Scribes::addMessage(const Message& message)
{
  _d->messages.push_front( message );
  _d->checkUnreadMessages();
}

namespace {
GAME_LITERALCONST(gtype)
GAME_LITERALCONST(ext)
}

VariantMap Scribes::Message::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY( ret, text )
  VARIANT_SAVE_ANY( ret, title )
  ret[ literals::gtype ] = good::Helper::name( gtype );
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
  gtype = good::Helper::type( stream.get( literals::gtype ).toString() );
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
  stepName.reserve( 256 );
  for( const auto& i : *this )
  {
    stepName = utils::format( 0xff, "%04d", step++ );
    ret[ stepName ] = i.save();
  }

  return ret;
}

void Scribes::Messages::load(const VariantMap &vm)
{
  for( const auto& i : vm )
  {
    push_front( Message() );
    back().load( i.second.toMap() );
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

void Scribes::Impl::checkUnreadMessages()
{
  unreadMessage_n = 0;
  for( const auto& i : messages )
    unreadMessage_n += i.opened ? 0 : 1;

  emit onChangeMessageNumberSignal( unreadMessage_n );
}

}//end namespace city
