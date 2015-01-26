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

#include "infobox_barracks.hpp"
#include "core/gettext.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "objects/barracks.hpp"
#include "label.hpp"
#include "core/utils.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

class AboutBarracks::Impl
{
public:
  Label* lbWeaponQty;
};

AboutBarracks::AboutBarracks(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 350 ), Rect() ), _d( new Impl )
{
  setupUI( ":/gui/barracsopts.gui" );

  BarracksPtr barracks = ptr_cast<Barracks>( tile.overlay() );

  if( !barracks.isValid() )
  {
    deleteLater();
    Logger::warning( "AboutBarracs: cant find barracks at %d,%d", tile.i(), tile.j() );
    return;
  }

  setBase( ptr_cast<Construction>( barracks ) );
  _setWorkingVisible( true );

  GET_DWIDGET_FROM_UI( _d, lbWeaponQty )

  setTitle( MetaDataHolder::findPrettyName( base()->type() ) );
  setText( _("##barracks_info##") );

  if( _d->lbWeaponQty )
  {
    _lbTextRef()->setHeight( height() / 2 );
    _d->lbWeaponQty->setTop( _lbTextRef()->bottom() + 5 );
    _d->lbWeaponQty->setText( _("##weapon_store_of##") + utils::i2str( barracks->goodQty( good::weapon ) ) );
  }
}

AboutBarracks::~AboutBarracks()
{
}

}

}//end namespace gui
