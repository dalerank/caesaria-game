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
#include <vector>

namespace gui
{

class ListBox::Impl
{
public:
  gfx::Pictures background;
  std::vector< ListBoxItem > items;
  NColor itemDefaultColorText;
  NColor itemDefaultColorTextHighlight;
	Rect clientClip;
	Rect margin;
  bool dragEventSended;
  int hoveredItemIndex;
  int itemHeight;
  int itemHeightOverride;
  int totalItemHeight;
	Font font;
	int itemsIconWidth;
	//SpriteBank* iconBank;
	ScrollBar* scrollBar;
	unsigned int selectTime;
	int selectedItemIndex;
	unsigned int lastKeyTime;
  std::string keyBuffer;
	bool selecting;
  Point itemTextOffset;
  bool needItemsRepackTextures;

signals public:
	Signal1<int> indexSelected;
  Signal1<std::string> textSelected;
	Signal1<int> indexSelectedAgain;
  Signal1<std::string> onItemSelectedAgainSignal;
  Signal1<const ListBoxItem&> onItemSelectedSignal;

	void recalculateItemHeight( const Font& defaulFont, int height )
	{    
		if( !font.isValid() )
		{
			font = defaulFont;

			if ( itemHeightOverride != 0 )
				itemHeight = itemHeightOverride;
			else
				itemHeight = font.getTextSize("A").height() + 4;
		}

		int newLength = itemHeight * items.size();

		if( newLength != totalItemHeight )
		{
			totalItemHeight = newLength;
			scrollBar->setMaxValue( std::max<int>( 0, totalItemHeight - height ) );
			int minItemHeight = itemHeight > 0 ? itemHeight : 1;
			scrollBar->setSmallStep ( minItemHeight );
			scrollBar->setLargeStep ( 2*minItemHeight );

			scrollBar->setVisible( !( totalItemHeight <= height ) );
		}
	}

};

}//end namespace gui
#endif //_CAESARIA_LISTBOX_PRIVATE_H_INCLUDE_
