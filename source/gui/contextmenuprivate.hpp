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

#ifndef _CAESARIA_CONTEXT_MENU_PRIVATE_H_INLCUDE_
#define _CAESARIA_CONTEXT_MENU_PRIVATE_H_INLCUDE_

#include <vector>

#include "gfx/picture.hpp"
#include "core/signals.hpp"

namespace gui
{

class ContextMenuItem;

class ContextMenu::Impl
{
public:
	std::vector< ContextMenuItem* > items;

  int highlihted;
	int lastHighlihted;
	unsigned int changeTime;
	bool allowFocus;

	Point pos;
	Widget* eventParent;
	Font lastFont;
	CloseMode closeHandling;
	bool needRecalculateItems;

signals public:
  Signal1<int> onItemActionSignal;
};

}//end namespace gui
#endif //_CAESARIA_CONTEXT_MENU_PRIVATE_H_INLCUDE_
