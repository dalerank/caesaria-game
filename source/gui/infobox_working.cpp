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
#include "widget_helper.hpp"
#include "game/datetimehelper.hpp"
#include "texturedbutton.hpp"
#include "core/logger.hpp"
#include "core/common.hpp"
#include "dialogbox.hpp"

namespace gui
{

namespace infobox
{

AboutWorkingBuilding::AboutWorkingBuilding( Widget* parent, WorkingBuildingPtr building)
  : AboutConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 136, 510 - 16, 136 + 62 ) )
{
  if( building.isNull() )
  {
    deleteLater();
    return;
  }

  _working = building;

  setBase( _working  );
  _setWorkingVisible( true );

  setTitle( _( _working->info().prettyName() ) );

  _updateWorkersLabel( Point( 32, 150 ), 542, _working->maximumWorkers(), _working->numberWorkers() );

  const int id = lbHelpId;
  auto& lb = add<Label>( Rect( 16, 50, width() - 16, 130 ), "", false, Label::bgNone, id );
  lb.setFont( Font::create( "FONT_2" ) );
  lb.setWordwrap( true );

  setText( "" );

  INIT_WIDGET_FROM_UI( TexturedButton*, btnHelp )
  if( btnHelp )
  {
    Rect rect = btnHelp->relativeRect();
    rect += Point( btnHelp->width() + 5, 0 );
    rect.rright() += 60;
    PushButton& btn = add<PushButton>( rect, "Adv.Info", -1, false, PushButton::whiteBorderUp );
    CONNECT_LOCAL( &btn, onClicked(), AboutWorkingBuilding::_showAdvInfo )
  }
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

void AboutWorkingBuilding::_showHelp() { ui()->add<DictionaryWindow>( _working->type() ); }

WorkingBuildingPtr AboutWorkingBuilding::_getBuilding() { return _working; }

void AboutWorkingBuilding::_showAdvInfo()
{
  auto serviceBuilding = _working.as<ServiceBuilding>();
  std::string timeText;
  if( serviceBuilding.isValid() )
  {
    DateTime time = serviceBuilding->lastSendService();
    timeText = utils::date2str( time, true );
  }

  std::string workerState = fmt::format( "Damage={}\nFire={}\nService={}\n",
                                         utils::objectState( _working, pr::damage ),
                                         utils::objectState( _working, pr::fire ),
                                         timeText );

  dialog::Information( ui(), "Information", workerState );
}

}//end namespace infobox

}//end namespace gui
