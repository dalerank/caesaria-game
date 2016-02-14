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

#ifndef _CAESARIA_CONTEXT_MENU_ITEM_PRIVATE_H_INCLUDE_
#define _CAESARIA_CONTEXT_MENU_ITEM_PRIVATE_H_INCLUDE_

#include "core/signals.hpp"
#include "core/size.hpp"
#include "core/flagholder.hpp"

namespace gui
{

class ContextMenu;

class ContextMenuItem::Impl : public FlagHolder<int>
{
public: 
	Size dim;
	int offset;
	int commandId;

  struct {
    bool autoChecking;
    bool hovered;
    bool separator;
    bool checked;
  } is;

  struct {
    ContextMenu* widget;
    bool iconVisible;
    SubMenuAlign align;
  } submenu;

signals public:
	Signal1<bool> onCheckedSignal;
	Signal1<int> onActionSignal;
};

}//end namespace gui
#endif //_CAESARIA_CONTEXT_MENU_ITEM_PRIVATE_H_INCLUDE_
