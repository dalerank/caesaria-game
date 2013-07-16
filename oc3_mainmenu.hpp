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

#ifndef __OC3_MAINMENU_H_INCLUDED__
#define __OC3_MAINMENU_H_INCLUDED__

#include "oc3_contextmenu.hpp"

//! GUI menu interface.
class MainMenu : public ContextMenu
{
public:

  //! constructor
  MainMenu( Widget* parent, const Rect& rectangle, const int id=-1);

  //! draws the element and its children
  //virtual void onPaint( AbstractPainter* painter );

  //! called if an event happened.
  virtual bool onEvent(const NEvent& event);

  ContextMenuItem* addItem( const std::string& text, int commandId, 
                            bool enabled, bool hasSubMenu, 
                            bool checked, bool autoChecking );

protected:
  //! Updates the absolute position.
  virtual void recalculateSize_();
};

#endif