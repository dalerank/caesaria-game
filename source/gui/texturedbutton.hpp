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

#ifndef __CAESARIA_TEXTURED_BUTTON_H_INCLUDED__
#define __CAESARIA_TEXTURED_BUTTON_H_INCLUDED__

#include "pushbutton.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"

namespace gui
{

class TexturedButton : public PushButton
{
public:
  TexturedButton( Widget* parent, const Point& pos, const Size& size, int id, 
                  int normalTxIndex, int hovTxIndex=-1, int prsTxIndex=-1, int dsbTxIndex=-1 ) 
    : PushButton( parent, Rect( pos, size ), "", id, false, noBackground )
  {
    setPicture( ResourceGroup::panelBackground, normalTxIndex, stNormal );
    setPicture( ResourceGroup::panelBackground, (hovTxIndex == -1) ? normalTxIndex+1 : hovTxIndex , stHovered );
    setPicture( ResourceGroup::panelBackground, (prsTxIndex == -1) ? normalTxIndex+2 : prsTxIndex , stPressed );
    setPicture( ResourceGroup::panelBackground, (dsbTxIndex == -1) ? normalTxIndex+3 : dsbTxIndex , stDisabled );
  }

  TexturedButton( Widget* parent, const Point& pos, const Size& size, int id,
                  const char* resourceGroup,
                  int normalTxIndex, int hovTxIndex=-1, int prsTxIndex=-1, int dsbTxIndex=-1 )
    : PushButton( parent, Rect( pos, size ), "", id, false, noBackground )
  {
    setPicture( resourceGroup, normalTxIndex , stNormal );
    setPicture( resourceGroup, (hovTxIndex == -1) ? normalTxIndex+1 : hovTxIndex, stHovered );
    setPicture( resourceGroup, (prsTxIndex == -1) ? normalTxIndex+2 : prsTxIndex, stPressed );
    setPicture( resourceGroup, (dsbTxIndex == -1) ? normalTxIndex+3 : dsbTxIndex, stDisabled );
  }

  TexturedButton( Widget* parent ) : PushButton( parent )
  {}
};

}//end namespace gui
#endif //__CAESARIA_TEXTURED_BUTTON_H_INCLUDED__
