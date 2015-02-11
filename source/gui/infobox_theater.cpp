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

#include "infobox_theater.hpp"
#include "gfx/tile.hpp"
#include "objects/constants.hpp"
#include "objects/theater.hpp"
#include "core/gettext.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"
#include "label.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutTheater::AboutTheater(Widget *parent, PlayerCityPtr city, const Tile &tile)
  : AboutWorkingBuilding( parent, ptr_cast<WorkingBuilding>( tile.overlay() ) )
{
  setupUI( ":/gui/infoboxtheater.gui" );

  TheaterPtr theater = ptr_cast<Theater>( _getBuilding() );
  setTitle( _( theater->name() ) );

  _lbTextRef()->setTextAlignment( align::upperLeft, align::center);
  _updateWorkersLabel( Point( 40, 150), 542, theater->maximumWorkers(), theater->numberWorkers() );
  
  if( theater->showsCount() == 0 )
  {
    setText( "##theater_no_have_any_shows##" );
  }
  else
  {
    if( theater->isShow() )
    {
      VariantMap shows = config::load( ":/theater_shows.model" );
      VariantMap::iterator currentShowIt = shows.begin();

      std::advance( currentShowIt, theater->showsCount() % shows.size() );

      std::string text;
      if( currentShowIt != shows.end() )
      {
        text = currentShowIt->second.toMap().get( "text" ).toString();
      }

      if( text.empty() )
      {
        text = "##theater_now_local_show##";
      }
      setText( _(text) );
    }
    else
    {
      setText( "##theater_need_actors##" );
    }
  }
}

AboutTheater::~AboutTheater(){}

}

}//end namespace gui
