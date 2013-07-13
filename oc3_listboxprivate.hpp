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

#ifndef _NRP_LISTBOX_PRIVATE_H_INCLUDE_
#define _NRP_LISTBOX_PRIVATE_H_INCLUDE_

#include "oc3_listboxitem.hpp"
#include "oc3_signals.hpp"
#include "oc3_scrollbar.hpp"
#include "oc3_color.hpp"
#include <vector>

class ListBox::Impl
{
public:
  PictureRef background;
  PictureRef picture;
  std::vector< ListBoxItem > items;
  NColor itemDefaultColorText;
  NColor itemDefaultColorTextHighlight;
	Rect clientClip;
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
	bool needItemsRepackTextures;

oc3_signals public:
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
				itemHeight = font.getSize("A").getHeight() + 4;
		}

		int newLength = itemHeight * items.size();

		if( newLength != totalItemHeight )
		{
			totalItemHeight = newLength;
			scrollBar->setMax( std::max<int>( 0, totalItemHeight - height ) );
			int minItemHeight = itemHeight > 0 ? itemHeight : 1;
			scrollBar->setSmallStep ( minItemHeight );
			scrollBar->setLargeStep ( 2*minItemHeight );

			scrollBar->setVisible( !( totalItemHeight <= height ) );
		}
	}

};

#endif
