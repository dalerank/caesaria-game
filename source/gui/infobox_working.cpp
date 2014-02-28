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

#include "infobox_working.hpp"
#include "core/gettext.hpp"
#include "label.hpp"
#include "dictionary.hpp"
#include "environment.hpp"

using namespace constants;

namespace gui
{

InfoboxWorkingBuilding::InfoboxWorkingBuilding( Widget* parent, WorkingBuildingPtr building)
  : InfoboxConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 136, 510 - 16, 136 + 62 ) )
{
  _working = building;

  setConstruction( ptr_cast<Construction>( _working ) );

  std::string title = MetaDataHolder::getPrettyName( _working->getType() );
  setTitle( _(title) );

  _updateWorkersLabel( Point( 32, 150 ), 542, _working->getMaxWorkers(), _working->getWorkersCount() );

  Label* lb = new Label( this, Rect( 16, 50, getWidth() - 16, 130 ), "", false, Label::bgNone, lbHelpId );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setWordwrap( true );

  setText( "" );

  std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire",
                                           (int)_working->getState( Construction::damage ),
                                           (int)_working->getState( Construction::fire ));
  if( is_kind_of<ServiceBuilding>( _working ) )
  {
    ServiceBuildingPtr srvc = ptr_cast<ServiceBuilding>( _working );
    DateTime time = srvc->getLastSendService();
    text += StringHelper::format( 0xff, " Srvc: %04d.%02d.%02d", time.year(), time.month(), time.day() );
  }
  new Label( this, Rect( 50, getHeight() - 30, getWidth() - 50, getHeight() - 10 ), text );
}

void InfoboxWorkingBuilding::setText(const std::string& text)
{  
  if( Widget* lb = findChild( lbHelpId ) )
  {
    StringArray messages;
    if( !text.empty() )
      messages.push_back( text );

    if( _working->getMaxWorkers() > 0 )
    {
      messages.push_back( _working->getWorkersProblem() );
    }

    lb->setText( _( messages.rand() ) );
  }
}

void InfoboxWorkingBuilding::showDescription()
{
  DictionaryWindow::show( getEnvironment()->getRootWidget(), _working->getType() );
}

WorkingBuildingPtr InfoboxWorkingBuilding::_getBuilding()
{
  return _working;
}

}//end namespace gui
