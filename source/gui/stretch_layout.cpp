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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "stretch_layout.hpp"

namespace gui
{

void Layout::updateLayout( Widget* parent, bool vertical, bool drawBorder, s32 margin )
{
    if( parent->getChildren().size() == 0 )
        return;

    u32 side = (( vertical ? parent->getHeight() : parent->getWidth() ) ) / parent->getChildren().size();
    Widget::Widgets pChildrens( parent->getChildren() );

    u32 lastPos = drawBorder ? 10 : 0;
    while( pChildrens.size() > 0 )
    {
        Widget* elm = *pChildrens.begin();
        core::RectI rectangle( core::Point( vertical ? 0 : lastPos, vertical ? lastPos : 0 ), 
                               core::NSizeU( vertical ? parent->getWidth() : side, 
                                                   vertical ? side : parent->getHeight() ) );

        rectangle.UpperLeftCorner += style.GetMargin().getRect().UpperLeftCorner;
        rectangle.LowerRightCorner -= style.GetMargin().getRect().LowerRightCorner;
        elm->setGeometry( rectangle );

        lastPos += ( vertical ? elm->getHeight() : elm->getWidth() ) + margin / 2;

		Widget::ChildIterator delIt = pChildrens.begin();
        pChildrens.erase( delIt );
        if( pChildrens.size() )
        {
            Widget::Widgets fixSizeElms( pChildrens );

            u32 elmsWithothMaxsizeNumber = 0;
            u32 afterSide = vertical ? parent->getHeight() : parent->getWidth();
            while( fixSizeElms.size() > 0 )
            {
                Widget* adapter = *(fixSizeElms.begin());
                afterSide -= ( vertical ? adapter->getMaxSize().Height : adapter->getMaxSize().Width );
                elmsWithothMaxsizeNumber += (( vertical ? adapter->getMaxSize().Height : adapter->getMaxSize().Width ) == 0 ? 1 : 0);

                delIt = fixSizeElms.begin();
                fixSizeElms.erase( delIt );
            }

            side = elmsWithothMaxsizeNumber > 0 ? (( afterSide - lastPos ) / elmsWithothMaxsizeNumber) : 9999;
        }
    }
}

void Layout::updateLayout()
{
    if( isNeedUpdate_() )
	    StretchLayoutImpl::updateLayout( this, ElementStyle::invalid(), vertical_, _drawBorder, _margin );

    resetUpdate_();
}

Layout::Layout( Widget* parent, const Rect& rectangle, bool vertical, int id )
: Widget( parent, rectangle, id ), vertical_( vertical ? maxColumnInVertMode : maxColumnInHorizMode )
{
}

Layout::~Layout()
{

}

}//end namespac gui
