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

#include "texturedbutton.hpp"
#include "game/resourcegroup.hpp"
#include "game/resourcegroup.hpp"
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(TexturedButton)

TexturedButton::TexturedButton(Widget *parent, const Point &pos, const Size &size, int id, int normalTxIndex, int hovTxIndex, int prsTxIndex, int dsbTxIndex)
  : PushButton( parent, Rect( pos, size ), "", id, false, noBackground )
{
  setPicture( ResourceGroup::panelBackground, normalTxIndex, stNormal );
  setPicture( ResourceGroup::panelBackground, (hovTxIndex == -1) ? normalTxIndex+1 : hovTxIndex , stHovered );
  setPicture( ResourceGroup::panelBackground, (prsTxIndex == -1) ? normalTxIndex+2 : prsTxIndex , stPressed );
  setPicture( ResourceGroup::panelBackground, (dsbTxIndex == -1) ? normalTxIndex+3 : dsbTxIndex , stDisabled );
  setTextVisible( false );
}

TexturedButton::TexturedButton(Widget *parent, const Point &pos, const Size &size, int id, const char *resourceGroup, int normalTxIndex, int hovTxIndex, int prsTxIndex, int dsbTxIndex)
  : PushButton( parent, Rect( pos, size ), "", id, false, noBackground )
{
  setPicture( resourceGroup, normalTxIndex , stNormal );
  setPicture( resourceGroup, (hovTxIndex == -1) ? normalTxIndex+1 : hovTxIndex, stHovered );
  setPicture( resourceGroup, (prsTxIndex == -1) ? normalTxIndex+2 : prsTxIndex, stPressed );
  setPicture( resourceGroup, (dsbTxIndex == -1) ? normalTxIndex+3 : dsbTxIndex, stDisabled );
  setTextVisible( false );
}

gui::TexturedButton::TexturedButton(gui::Widget *parent) : PushButton( parent )
{
  setTextVisible( false );
}

}//end namespace gui
