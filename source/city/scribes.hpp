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

#ifndef __CAESARIA_CITY_SCRIBES_H_INCLUDED__
#define __CAESARIA_CITY_SCRIBES_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "good/good.hpp"
#include "core/position.hpp"
#include "core/signals.hpp"
#include "core/time.hpp"
#include "core/variant.hpp"

namespace city
{

class Scribes
{
public:
  struct Message
  {
    std::string text;
    std::string title;
    good::Product gtype;
    Point position;
    int type;
    DateTime date;
    bool opened;
    Variant ext;

    VariantMap save() const;
    void load( const VariantMap& stream );
  };

  class Messages : public std::list<Message>
  {
  public:
    VariantMap save() const;
    void load( const VariantMap& vm );
    Message& at( unsigned int index );
  };

  const Messages& messages() const;
  const Message& getMessage( unsigned int index ) const;
  const Message& readMessage( unsigned int index );
  bool haveUnreadMessage() const;
  void changeMessage( int index, Message& message );
  void removeMessage( int index );
  void addMessage( const Message& message );

  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream);

  Scribes();
  ~Scribes();

public signals:
  Signal1<int>& onChangeMessageNumber();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace city

#endif //__CAESARIA_CITY_SCRIBES_H_INCLUDED__
