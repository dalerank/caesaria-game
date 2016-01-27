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

#include "empireprices.hpp"
#include "city/trade_options.hpp"
#include "city/city.hpp"
#include "good/helper.hpp"
#include "image.hpp"
#include "core/utils.hpp"
#include "label.hpp"
#include "core/event.hpp"
#include "widgetescapecloser.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

class EmpirePrices::Impl
{
public:
  Point startPos = { 140, 50 };
  Point importOffset = { 0, 34 };
  Point exportOffset = { 0, 58 };
  Point nextOffset = { 30, 0 };
  Size  iconSize = { 24, 24 };
};

EmpirePrices::EmpirePrices(Widget *parent, int id, const Rect &rectangle, PlayerCityPtr city)
  : Window( parent, rectangle, "", id ), __INIT_IMPL(EmpirePrices)
{
  Window::setupUI( ":/gui/empireprices.gui" );

  __D_REF(d,EmpirePrices)
  Font fontBuy = Font::create( FONT_1 );
  Font fontSell = Font::create( FONT_1, ColorList::darkOrange );
  WidgetClose::insertTo( this, KEY_RBUTTON );

  good::Products goods = good::tradable();
  Point startPos = d.startPos;
  for( auto gtype : goods )
  {
    good::Info info( gtype );
    Image& img = add<Image>( startPos, info.picture() );
    img.setTooltipText( info.name() );

    add<Label>( Rect( startPos + d.importOffset, d.iconSize ),
                utils::i2str( info.price( city, good::Info::importing ) ),
                fontBuy );

    add<Label>( Rect( startPos + d.exportOffset, d.iconSize ),
                utils::i2str( info.price( city, good::Info::exporting ) ),
                fontSell );

    startPos += d.nextOffset;
  }
}

void EmpirePrices::draw(Engine &painter)
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void EmpirePrices::setupUI(const VariantMap& ui)
{
  Window::setupUI( ui );

  __D_IMPL(_d,EmpirePrices)
  VARIANT_LOAD_ANYDEF_D( _d, startPos, _d->startPos, ui )
  VARIANT_LOAD_ANYDEF_D( _d, importOffset, _d->importOffset, ui )
  VARIANT_LOAD_ANYDEF_D( _d, exportOffset, _d->exportOffset, ui )
  VARIANT_LOAD_ANYDEF_D( _d, nextOffset, _d->nextOffset, ui )
  VARIANT_LOAD_ANYDEF_D( _d, iconSize, _d->iconSize, ui )
}

}

}//end namespace gui
