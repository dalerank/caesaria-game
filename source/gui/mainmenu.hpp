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

#ifndef __CAESARIA_MAINMENU_H_INCLUDED__
#define __CAESARIA_MAINMENU_H_INCLUDED__

#include "contextmenu.hpp"

namespace gui
{

//! GUI menu interface.
class MainMenu : public ContextMenu
{
public:

  //! constructor
  MainMenu( Widget* parent, const Rect& rectangle, const int id=-1);

  //! called if an event happened.
  virtual bool onEvent(const NEvent& event);

  virtual ContextMenuItem* addItem( const std::string& text, int commandId,
                            bool enabled, bool hasSubMenu, 
                            bool checked, bool autoChecking );

protected:
  //! Updates the absolute position.
  virtual void _recalculateSize();
};

}//end namespace gui
#endif
