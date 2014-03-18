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

#include "infobox_theater.hpp"
#include "gfx/tile.hpp"
#include "objects/constants.hpp"
#include "objects/theater.hpp"
#include "core/gettext.hpp"
#include "label.hpp"

using namespace constants;

namespace gui
{

InfoboxTheater::InfoboxTheater(Widget *parent, const Tile &tile)
  : InfoboxWorkingBuilding( parent, ptr_cast<WorkingBuilding>( tile.overlay() ) )
{
  TheaterPtr theater = ptr_cast<Theater>( _getBuilding() );
  setTitle( _( theater->name() ) );

  _getInfo()->setTextAlignment( alignUpperLeft, alignCenter);
  _updateWorkersLabel( Point( 40, 150), 542, theater->maxWorkers(), theater->numberWorkers() );
  
  if( theater->showsCount() == 0 )
  {
    setText( "##theater_no_have_any_shows##" );
  }
  else
  {
    setText( theater->isShow() ? "##theater_now_local_show##" : "##theater_need_actors##" );
  }
}

InfoboxTheater::~InfoboxTheater(){}

}//end namespace gui
