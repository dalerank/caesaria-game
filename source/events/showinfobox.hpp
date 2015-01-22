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

#ifndef _CAESARIA_INFOBOX_EVENT_H_INCLUDE_
#define _CAESARIA_INFOBOX_EVENT_H_INCLUDE_

#include "event.hpp"

namespace events
{

class ShowInfobox : public GameEvent
{
public:
  static const bool send2scribe = true;
  static GameEventPtr create();
  static GameEventPtr create( const std::string& title, const std::string& text,
                              good::Product type, bool send2scribe=false );

  static GameEventPtr create( const std::string& title, const std::string& text,
                              bool send2scribe=false, const vfs::Path& video="" );

  virtual void load(const VariantMap &);
  virtual VariantMap save() const;

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  ShowInfobox();

  class Impl;
  ScopedPtr<Impl> _d;
};

} //end namespace events
#endif //_CAESARIA_INFOBOX_EVENT_H_INCLUDE_
