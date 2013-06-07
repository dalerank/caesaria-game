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

#ifndef __OPENCAESAR3_STANDARTBUTTONS_H_INCLUDED__
#define __OPENCAESAR3_STANDARTBUTTONS_H_INCLUDED__

#include "oc3_pushbutton.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"

class TexturedButton : public PushButton
{
public:
  TexturedButton( Widget* parent, const Point& pos, const Size& size, int id, 
                  int normalTxIndex, int hovTxIndex=-1, int prsTxIndex=-1, int dsbTxIndex=-1 ) 
    : PushButton( parent, Rect( pos, size ), "", id )
  {
    setPicture( &Picture::load( ResourceGroup::panelBackground, normalTxIndex ), stNormal );
    setPicture( &Picture::load( ResourceGroup::panelBackground, (hovTxIndex == -1) ? normalTxIndex+1 : hovTxIndex ), stHovered );
    setPicture( &Picture::load( ResourceGroup::panelBackground, (prsTxIndex == -1) ? normalTxIndex+2 : prsTxIndex ), stPressed );
    setPicture( &Picture::load( ResourceGroup::panelBackground, (dsbTxIndex == -1) ? normalTxIndex+3 : dsbTxIndex ), stDisabled );
  }
};

#endif //__OPENCAESAR3_STANDARTBUTTONS_H_INCLUDED__