// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _OPENCAESAR3_CONTEXT_MENU_ITEM_PRIVATE_H_INCLUDE_
#define _OPENCAESAR3_CONTEXT_MENU_ITEM_PRIVATE_H_INCLUDE_

#include "oc3_signals.hpp"
#include "oc3_size.hpp"
#include "oc3_flagholder.hpp"

class ContextMenu;

class ContextMenuItem::Impl : public FlagHolder<int>
{
public: 
	Size dim;
	int offset;
	int luaFunction;	
	int commandId;
	bool isAutoChecking;
	bool checked;
	bool isSeparator;   
	bool isHovered;
	ContextMenu* subMenu;
	SubMenuAlign subMenuAlignment;

oc3_signals public:
	Signal1<bool> onCheckedSignal;
};

#endif //_OPENCAESAR3_CONTEXT_MENU_ITEM_PRIVATE_H_INCLUDE_