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
#include "core/utils.hpp"

namespace game
{

class CodeMapper : public EnumsHelper<KeyCode>
{
public:
  CodeMapper() : EnumsHelper<KeyCode>( KEY_KEY_CODES_COUNT )
  {
#define _HK(a,t) append(a,t);
    _HK(KEY_F1,"F1") _HK(KEY_F2,"F2") _HK(KEY_F3,"F3")
    _HK(KEY_F4,"F4") _HK(KEY_F5,"F5") _HK(KEY_F6,"F6")
    _HK(KEY_F7,"F7") _HK(KEY_F8,"F8") _HK(KEY_F9,"F9")
    _HK(KEY_LALT,"lalt") _HK(KEY_RALT, "ralt")
    _HK(KEY_LSHIFT,"lshift") _HK(KEY_RSHIFT,"rshift")
    _HK(KEY_LCONTROL, "lctrl") _HK(KEY_RCONTROL, "rctrl")
    _HK(KEY_SHIFT,"shift") _HK(KEY_CONTROL, "ctrl")
    _HK(KEY_F10,"F10") _HK(KEY_F11,"F11") _HK(KEY_F12,"F12")
    _HK(KEY_PLUS,"plus") _HK(KEY_MINUS,"minus")
    _HK(KEY_SUBTRACT,"sutract") _HK(KEY_ADD,"sum")
    _HK(KEY_TILDA,"tilda") _HK(KEY_SPACE,"space")
    _HK(KEY_COMMA, "comma") _HK(KEY_PERIOD, "period")
    _HK(KEY_KEY_0,"0")
    _HK(KEY_KEY_1,"1") _HK(KEY_KEY_2,"2") _HK(KEY_KEY_3,"3")
    _HK(KEY_KEY_4,"4") _HK(KEY_KEY_5, "5") _HK(KEY_KEY_6, "6")
    _HK(KEY_KEY_7, "7") _HK(KEY_KEY_8, "8") _HK(KEY_KEY_9, "9")
    _HK(KEY_KEY_A, "a") _HK(KEY_KEY_H, "h") _HK(KEY_KEY_F, "f")
    _HK(KEY_KEY_D, "d") _HK(KEY_KEY_C, "c") _HK(KEY_KEY_T, "t")
    _HK(KEY_KEY_W, "w") _HK(KEY_KEY_G, "g") _HK(KEY_EQUALS, "=")
    _HK(KEY_BACK, "backspace")
#undef _HK
  }
};

struct HotkeyAction
{
  KeyCode code;
  bool shift;
  bool ctrl;
  bool alt;
};

typedef std::map<std::string, HotkeyAction> HotkeyActions;
class HotkeyManager::Impl
{
public:
  CodeMapper hkMapper;
  HotkeyActions actions;

public signals:
  Signal1<std::string> onExecSignal;
};

bool HotkeyManager::execute(int keyCode, bool ctrl, bool shift, bool alt)
{
  for (const auto& item : _d->actions)
  {
    const HotkeyAction& a = item.second;
    if (a.code == keyCode && a.ctrl == ctrl
      && a.shift == shift && a.alt == alt)
    {
      emit _d->onExecSignal(item.first);
      return true;
    }
  }
  return false;
}

void HotkeyManager::clear()
{
  _d->actions.clear();
}

void HotkeyManager::add(const std::string & name, const std::string& config)
{
  std::string rconfig = utils::trim(config);
  StringArray items = utils::split(config, "+");

  KeyCode code = _d->hkMapper.findType(items.valueOrEmpty(0));
  if (!items.empty())
    items.erase(items.begin());

  if (code != KEY_KEY_CODES_COUNT)
  {
    HotkeyAction action;
    action.code = code;
    action.alt = items.contains("alt");
    action.ctrl = items.contains("ctrl");
    action.shift = items.contains("shift");

    _d->actions[name] = action;
  }
}

Signal1<std::string>& HotkeyManager::onExec() { return _d->onExecSignal; }

HotkeyManager::HotkeyManager() : _d(new Impl) {}
HotkeyManager::~HotkeyManager(){}

} //end namespace game

