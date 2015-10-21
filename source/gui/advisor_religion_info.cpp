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

#include "advisor_religion_info.hpp"
#include "gfx/picture.hpp"
#include "core/gettext.hpp"
#include "widget_helper.hpp"
#include "core/utils.hpp"
#include "religion/romedivinity.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/engine.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "religion/pantheon.hpp"

using namespace religion;
using namespace gfx;

namespace gui
{

namespace advisorwnd
{

class ReligionInfoLabel::Impl
{
public:
  DivinityPtr divinity;
  int lastFestival;
  int mood;

  struct {
    int small_n;
    int big_n;
  } temples;

  struct {
    int xOffset;
    Picture image;
  } wrath;
};

ReligionInfoLabel::ReligionInfoLabel(Widget* parent, const Rect& rect, DivinityPtr divinity, int smallTempleCount, int bigTempleCount)
  : Label( parent, rect ), __INIT_IMPL(ReligionInfoLabel)
{
  __D_REF(d,ReligionInfoLabel)
  d.divinity = divinity;
  d.temples.small_n = smallTempleCount;
  d.temples.big_n = bigTempleCount;
  d.mood = 0;
  d.wrath.xOffset = 400;

  d.wrath.image = Picture( ResourceGroup::panelBackground, 334 );
  setFont( Font::create( FONT_1_WHITE ) );
}

void ReligionInfoLabel::_updateTexture(Engine& painter)
{
  __D_REF(d,ReligionInfoLabel)
  Label::_updateTexture( painter );

  if( d.divinity.isValid() )
  {
    d.lastFestival = d.divinity->lastFestivalDate().monthsTo( game::Date::current() );

    canvasDraw( d.divinity->name(), Point() );
    canvasDraw( fmt::format( "({})", _( d.divinity->shortDescription() ) ), Point( 80, 0 ), Font::create( FONT_1 ) );
    canvasDraw( utils::i2str( d.temples.small_n ), Point( 220, 0 ) );
    canvasDraw( utils::i2str( d.temples.big_n ), Point( 280, 0 ) );

#ifdef DEBUG
    canvasDraw( fmt::format( "f{}/r{}", d.lastFestival, (int)d.divinity->relation() ), Point( 320, 0 ) );
#else
    canvasDraw( utils::i2str( d.lastFestival ), Point( 350, 0 ) );
#endif

    int moodOffsetX = d.wrath.xOffset + d.divinity->wrathPoints() / 15 * 15;
    canvasDraw( _( d.divinity->moodDescription() ), Point( moodOffsetX, 0 ) );
  }
  else
  {
    canvasDraw( _("##oracles_in_city##"), Point() );
    canvasDraw( utils::i2str( d.temples.small_n ), Point( 220, 0 ) );
  }
}

void ReligionInfoLabel::draw(Engine& painter)
{
  __D_REF(d,ReligionInfoLabel)
  Label::draw( painter );

  if( d.divinity.isValid() )
  {
    for( int k=0; k < d.divinity->wrathPoints() / 15; k++ )
    {
      painter.draw( d.wrath.image, absoluteRect().lefttop() + Point( d.wrath.xOffset + k * 15, 0), &absoluteClippingRectRef() );
    }
  }
}

}//end namespace advisorwnd

}//end namespace gui
