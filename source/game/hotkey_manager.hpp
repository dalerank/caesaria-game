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

#ifndef _CAESARIA_HOTKEY_MANAGER_INCLUDE_H_
#define _CAESARIA_HOTKEY_MANAGER_INCLUDE_H_

#include <string>
#include "vfs/path.hpp"
#include "core/signals.hpp"
#include "core/variant.hpp"

namespace game
{

class HotkeyManager
{
public:
  static HotkeyManager& instance();

  void load( vfs::Path file );
  void execute( int keyCode );

public signals:
  Signal1<const VariantMap&>& onHotkey();

private:
  HotkeyManager();

  class Impl;
  ScopedPtr<Impl> _d;
};

} //end namespace game

#endif  //_CAESARIA_HOTKEY_MANAGER_INCLUDE_H_
