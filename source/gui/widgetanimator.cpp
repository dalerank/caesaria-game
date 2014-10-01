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

#include "widgetanimator.hpp"
#include "gfx/engine.hpp"

namespace gui
{

WidgetAnimator::WidgetAnimator( Widget* parent, int flags ) 
    : Widget( parent, -1, Rect( 0, 0, 1, 1 ) )
{
  setFlags( flags );
  setFlag( isActive );
}

WidgetAnimator::~WidgetAnimator( void )
{
}

void WidgetAnimator::beforeDraw(gfx::Engine& painter )
{
  //! draw self area in debug mode
  if( isFlag( debug ) )
  {
     // painter->drawRectangleOutline( getAbsoluteRect(), Color( 0xff, 0xff, 0xff, 0 ), &getAbsoluteClippingRectRef() );

      //Font font( Font::builtinName );
      //if( font.available() )
      //    font.Draw( getInternalName(), getAbsoluteRect(), getResultColor(), true, true, NULL );
  }

  Widget::beforeDraw( painter );
}

void WidgetAnimator::_afterFinished()
{
	if( isFlag( removeParent ) )
	{
		parent()->deleteLater();
	}
  else if( isFlag( removeSelf ) )
    deleteLater();

  emit _onFinishSignal();
}

}//end namespace gui
