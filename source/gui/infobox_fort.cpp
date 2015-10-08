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

#include "infobox_fort.hpp"
#include "objects/fort.hpp"
#include "objects/extension.hpp"
#include "core/gettext.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "dictionary.hpp"
#include "walker/soldier.hpp"
#include "label.hpp"
#include "game/infoboxmanager.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(fort_legionaries,AboutFort)
REGISTER_OBJECT_BASEINFOBOX(fort_javelin,AboutFort)
REGISTER_OBJECT_BASEINFOBOX(fort_horse,AboutFort)
REGISTER_OBJECT_BASEINFOBOX(fortArea,AboutFort)

class AboutFort::Impl
{
public:
  Label* lbWeaponQty;
  Label* lbText;

  FortPtr fort;

public:
  void update( Widget* parent );
};

AboutFort::AboutFort(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 350 ), Rect() ), _d( new Impl )
{
  setupUI( ":/gui/fortopts.gui" );

  GET_DWIDGET_FROM_UI( _d, lbWeaponQty )

  OverlayPtr overlay = tile.overlay();
  if( overlay.is<Fort>() ) { _d->fort = overlay.as<Fort>(); }
  else if( overlay.is<FortArea>() )
  {
    _d->fort = overlay.as<FortArea>()->base();
  }
  else
  {
    deleteLater();
    Logger::warning( "AboutFort: cant find fort for [{0},{1}]", tile.i(), tile.j() );
    return;
  }

  setTitle( MetaDataHolder::findPrettyName( _d->fort->type() ) );

  std::string text = _("##fort_info##");

  if( _d->fort.isValid() )
  {
    int fortCursed = _d->fort->extensions().count<FortCurseByMars>() ;
    if( fortCursed > 0 )
        text = "##fort_has_been_cursed_by_mars##";
  }

  _d->lbText = new Label( this, Rect( 20, 20, width() - 20, 120 ), text );
  _d->lbText->setTextAlignment( align::upperLeft, align::center );
  _d->lbText->setWordwrap( true );

  _d->update( this );
}

AboutFort::~AboutFort()
{
}

void AboutFort::_showHelp()
{
  DictionaryWindow::show( this, "fort" );
}

void AboutFort::Impl::update(Widget* parent)
{
  Point startPos = lbText->leftbottom();
  const SoldierList& soldiers = fort->soldiers();
  SoldierList::size_type k=0;
  int lbWidth = (parent->width() - 40) / 2;
  foreach( it, soldiers )
  {
    Label* lbSoldierName = new Label( parent, Rect( Point( k < 8 ? 0 : lbWidth, 0), Size( lbWidth, 24 ) ), (*it)->name() );
    lbSoldierName->setTextAlignment( align::center, align::center );
    lbSoldierName->move( startPos + Point( 0, 25 * k ) );
    k++;
  }
}

}

}//end namespace gui
