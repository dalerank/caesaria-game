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

#include "infobox_working.hpp"
#include "core/gettext.hpp"
#include "label.hpp"
#include "dictionary.hpp"
#include "environment.hpp"
#include "core/utils.hpp"

using namespace constants;

namespace gui
{

namespace infobox
{

AboutWorkingBuilding::AboutWorkingBuilding( Widget* parent, WorkingBuildingPtr building)
  : AboutConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 136, 510 - 16, 136 + 62 ) )
{
  _working = building;

  setBase( ptr_cast<Construction>( _working ) );
  _setWorkingVisible( true );

  std::string title = MetaDataHolder::findPrettyName( _working->type() );
  setTitle( _(title) );

  _updateWorkersLabel( Point( 32, 150 ), 542, _working->maximumWorkers(), _working->numberWorkers() );

  Label* lb = new Label( this, Rect( 16, 50, width() - 16, 130 ), "", false, Label::bgNone, lbHelpId );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setWordwrap( true );

  setText( "" );

  std::string text = utils::format( 0xff, "%d%% damage - %d%% fire",
                                           (int)_working->state( Construction::damage ),
                                           (int)_working->state( Construction::fire ));
  if( is_kind_of<ServiceBuilding>( _working ) )
  {
    ServiceBuildingPtr srvc = ptr_cast<ServiceBuilding>( _working );
    DateTime time = srvc->lastSendService();
    text += utils::format( 0xff, " Srvc: %04d.%02d.%02d", time.year(), time.month(), time.day() );
  }

  new Label( this, Rect( 50, height() - 30, width() - 50, height() - 10 ), text );
}

void AboutWorkingBuilding::setText(const std::string& text)
{  
  if( Widget* lb = findChild( lbHelpId ) )
  {
    StringArray messages;
    if( !text.empty() )
      messages.push_back( text );

    if( _working->maximumWorkers() > 0 )
    {
      messages.push_back( _working->workersProblemDesc() );
    }

    lb->setText( _( messages.random() ) );
  }
}

void AboutWorkingBuilding::_showHelp()
{
  DictionaryWindow::show( this, _working->type() );
}

WorkingBuildingPtr AboutWorkingBuilding::_getBuilding()
{
  return _working;
}

}

}//end namespace gui
