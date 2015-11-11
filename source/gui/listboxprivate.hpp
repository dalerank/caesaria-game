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

#ifndef _CAESARIA_LISTBOX_PRIVATE_H_INCLUDE_
#define _CAESARIA_LISTBOX_PRIVATE_H_INCLUDE_

#include "listboxitem.hpp"
#include "core/signals.hpp"
#include "scrollbar.hpp"
#include "core/color.hpp"
#include "gfx/picturesarray.hpp"
#include "gfx/batch.hpp"
#include <vector>

namespace gui
{

class ListBox::Impl
{
public:
  gfx::Batch background;
  gfx::Pictures backgroundNb;

  std::vector< ListBoxItem > items;

  struct {
    NColor text;
    NColor textHighlight;
  } color;

  struct {
    int item;
    int total;
    int override;
  } height;

	Rect clientClip;
	Rect margin;

  struct {
    int selected;
    int hovered;
  } index;

  bool dragEventSended;
	Font font;
	int itemsIconWidth;
	//SpriteBank* iconBank;
	ScrollBar* scrollBar;

  struct {
    unsigned int select;
    unsigned int lastKey;
  } time;

  std::string keyBuffer;
	bool selecting;
  Point itemTextOffset;
  bool needItemsRepackTextures;

  struct {
    Signal1<int> onIndexSelected;
    Signal1<int> onIndexSelectedAgain;

    Signal1<std::string> onTextSelected;

    Signal1<const ListBoxItem&> onItemSelectedAgain;
    Signal1<const ListBoxItem&> onItemSelected;
  } signal;
};

}//end namespace gui
#endif //_CAESARIA_LISTBOX_PRIVATE_H_INCLUDE_
