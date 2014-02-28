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

#include "infobox_construction.hpp"
#include "core/event.hpp"
#include "events/showtileinfo.hpp"

using namespace constants;

namespace gui
{

InfoboxConstruction::InfoboxConstruction( Widget* parent, Rect rect, Rect blackArea )
  : InfoboxSimple( parent, rect, blackArea )
{
}

InfoboxConstruction::~InfoboxConstruction() {}

bool InfoboxConstruction::onEvent(const NEvent& event)
{
  if( event.EventType == sEventKeyboard )
  {
    switch( event.keyboard.key )
    {
    case KEY_COMMA:
    case KEY_PERIOD:
    {
      if( _construction.isValid() )
      {
        events::GameEventPtr e = events::ShowTileInfo::create( getConstruction()->pos(), event.keyboard.key == KEY_PERIOD
                                                                                           ? events::ShowTileInfo::next
                                                                                           : events::ShowTileInfo::prew );
        deleteLater();
        e->dispatch();
      }
    }
    break;

    default:
    break;
    }
  }

  return InfoboxSimple::onEvent( event );
}

ConstructionPtr InfoboxConstruction::getConstruction() const { return _construction; }
void InfoboxConstruction::setConstruction(ConstructionPtr construction) { _construction = construction; }

}//end namespace gui
