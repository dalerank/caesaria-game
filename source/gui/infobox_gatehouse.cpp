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

#include "infobox_gatehouse.hpp"
#include "objects/gatehouse.hpp"
#include "core/gettext.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "dictionary.hpp"
#include "label.hpp"
#include "game/infoboxmanager.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(gatehouse,AboutGatehouse)

class AboutGatehouse::Impl
{
public:
  Label* lbText;
  GatehousePtr gates;
};

AboutGatehouse::AboutGatehouse(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 350 ), Rect( 20, 240, 490, 290 ) ), _d( new Impl )
{
  setupUI( ":/gui/gatehouseopts.gui" );

  _d->gates = tile.overlay<Gatehouse>();
  if( !_d->gates.isValid() )
  {
    deleteLater();
    Logger::warning( "AboutGatehouse: cant find fort for [%d,%d]", tile.i(), tile.j() );
    return;
  }

  std::string title = MetaDataHolder::findPrettyName( _d->gates->type() );
  setTitle( _( title ) );

  std::string text = _("##walls_need_a_gatehouse##");

  _d->lbText = new Label( this, Rect( 20, 20, width() - 20, 240 ), text );
  _d->lbText->setTextAlignment( align::upperLeft, align::center );
  _d->lbText->setWordwrap( true );

  _setWorkingVisible( true );
  _updateModeText();
}

AboutGatehouse::~AboutGatehouse()
{
}

void AboutGatehouse::_showHelp()
{
  DictionaryWindow::show( this, "gatehouse" );
}

void AboutGatehouse::_resolveToggleWorking()
{
  Gatehouse::Mode mode = _d->gates->mode();
  mode = Gatehouse::Mode( ((int)mode+1)%(Gatehouse::opened+1));
  _d->gates->setMode( mode );

  _updateModeText();
}

void AboutGatehouse::_updateModeText()
{
  Gatehouse::Mode mode = _d->gates->mode();
  std::string modeDesc[] = { "##gh_auto##", "##gh_closed##", "##gh_opened##", "##gh_unknown##" };
  _buttonToggleWorking()->setText( modeDesc[ math::clamp<int>( mode, 0, Gatehouse::opened) ] );
}

}//end namespace infobox

}//end namespace gui
