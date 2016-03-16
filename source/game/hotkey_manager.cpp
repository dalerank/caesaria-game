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

#include "hotkey_manager.hpp"
#include "core/event.hpp"
#include "core/variant_map.hpp"
#include "core/enumerator.hpp"
#include "core/saveadapter.hpp"
#include "core/logger.hpp"

namespace game
{

class HotkeyMapper : public EnumsHelper<KeyCode>
{
public:
  HotkeyMapper() : EnumsHelper<KeyCode>( KEY_KEY_CODES_COUNT )
  {
#define _HK(a) append(a, TEXT(a) );
    _HK(KEY_F1) _HK(KEY_F2) _HK(KEY_F3)
    _HK(KEY_F4) _HK(KEY_F5) _HK(KEY_F6)
    _HK(KEY_F7) _HK(KEY_F8) _HK(KEY_F9)
    _HK(KEY_F10) _HK(KEY_F11) _HK(KEY_F12)
    _HK(KEY_PLUS) _HK(KEY_MINUS) _HK(KEY_KEY_0)
    _HK(KEY_KEY_1) _HK(KEY_KEY_2) _HK(KEY_KEY_3)
    _HK(KEY_KEY_4) _HK(KEY_KEY_5) _HK(KEY_KEY_6)
    _HK(KEY_KEY_7) _HK(KEY_KEY_8) _HK(KEY_KEY_9)
    _HK(KEY_KEY_A) _HK(KEY_KEY_H) _HK(KEY_KEY_F)
    _HK(KEY_KEY_D) _HK(KEY_KEY_C) _HK(KEY_KEY_T)
    _HK(KEY_KEY_W) _HK(KEY_KEY_G) _HK(KEY_EQUALS)
    _HK(KEY_BACK)
#undef _HK
  }
};

typedef std::map<KeyCode, VariantMap> HotkeyScripts;
class HotkeyManager::Impl
{
public:
  HotkeyMapper hkMapper;
  HotkeyScripts scripts;

public signals:
  Signal1<const VariantMap&> onHotkeySignal;
};

void HotkeyManager::execute( int keyCode )
{
  HotkeyScripts::iterator it = _d->scripts.find( (KeyCode)keyCode );
  if( it != _d->scripts.end() )
  {
    emit _d->onHotkeySignal( it->second );
  }
}

Signal1<const VariantMap&>& HotkeyManager::onHotkey() { return _d->onHotkeySignal; }

void HotkeyManager::load(vfs::Path file)
{
  VariantMap stream = config::load( file );
  for( auto& it : stream )
  {
    KeyCode hotkey = _d->hkMapper.findType( it.first );
    if( hotkey == KEY_KEY_CODES_COUNT )
    {
      Logger::warning( "HotkeyManager: cant find equale for " + it.first );
      continue;
    }

    HotkeyScripts::iterator presentIt = _d->scripts.find( hotkey );
    if( presentIt != _d->scripts.end() )
    {
      Logger::warning( "HotkeyManager: duplicate hotkey for " + it.first );
      continue;
    }

    _d->scripts[ hotkey ] = it.second.toMap();
  }
}

HotkeyManager::HotkeyManager() : _d(new Impl) {}
HotkeyManager::~HotkeyManager(){}

} //end namespace game

