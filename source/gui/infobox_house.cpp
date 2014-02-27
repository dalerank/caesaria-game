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

#include <cstdio>

#include "infobox_house.hpp"

#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "core/gettext.hpp"
#include "gfx/decorator.hpp"
#include "objects/metadata.hpp"
#include "objects/house_level.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "texturedbutton.hpp"
#include "gui/label.hpp"
#include "city/city.hpp"
#include "objects/market.hpp"
#include "objects/granary.hpp"
#include "core/stringhelper.hpp"
#include "good/goodhelper.hpp"
#include "objects/farm.hpp"
#include "objects/entertainment.hpp"
#include "objects/house.hpp"
#include "objects/religion.hpp"
#include "game/divinity.hpp"
#include "objects/warehouse.hpp"
#include "gfx/engine.hpp"
#include "gui/special_orders_window.hpp"
#include "good/goodstore.hpp"
#include "groupbox.hpp"
#include "walker/walker.hpp"
#include "objects/watersupply.hpp"
#include "objects/senate.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "events/event.hpp"
#include "game/settings.hpp"
#include "image.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace gui
{

InfoBoxHouse::InfoBoxHouse( Widget* parent, const Tile& tile )
  : InfoboxSimple( parent, Rect( 0, 0, 510, 360 ), Rect( 16, 150, 510 - 16, 360 - 50 ) )
{
  HousePtr house = ptr_cast<House>( tile.getOverlay() );
  setTitle( _(house->getSpec().getLevelName()) );

  _getBtnExit()->setTooltipText( _("##advanced_houseinfo##") );

  Label* houseInfo = new Label( this, Rect( 30, 40, getWidth() - 30, 40 + 100 ), _( house->getEvolveInfo() ) );
  houseInfo->setWordwrap( true );

  std::string workerState = StringHelper::format( 0xff, "hb=%d hr=%d nb=%d ch=%d sch=%d st=%d mt=%d old=%d",
                                                  house->getHabitants().count(),
                                                  (int)house->getServiceValue( Service::recruter ),
                                                  house->getHabitants().count( CitizenGroup::newborn ),
                                                  house->getHabitants().count( CitizenGroup::child ),
                                                  house->getHabitants().count( CitizenGroup::scholar ),
                                                  house->getHabitants().count( CitizenGroup::student ),
                                                  house->getHabitants().count( CitizenGroup::mature ),
                                                  house->getHabitants().count( CitizenGroup::aged ) );
  new Label( this, Rect( 16, 125, getWidth() - 16, 150 ), workerState );

  drawHabitants( house );

  int taxes = house->getSpec().getTaxRate();
  std::string taxesStr;
  if( taxes > 0 )
  {
    if( house->getServiceValue( Service::forum ) == 0 )
    {
      taxesStr = StringHelper::format( 0xff, "%d %s", taxes, _("##house_pay_tax##") );
    }
    else
    {
      DateTime lastTax = house->getLastTaxation();
      if( GameDate::current().year() == lastTax.year() )
      {
        taxesStr = "##no_tax_in_this_year##";
      }
      else
      {
        taxesStr = "##no_visited_by_taxman##";
      }
    }
  }
  else
  {
    taxesStr = "##house_not_taxation##";
  }

  Label* taxesLb = new Label( this, Rect( 16 + 35, 177, getWidth() - 16, 177 + 20 ), _( taxesStr ) );

  std::string aboutCrimes = _("##house_not_report_about_crimes##");
  Label* lbCrime = new Label( this, taxesLb->getRelativeRect() + Point( 0, 22 ), aboutCrimes );

  int startY = lbCrime->getBottom() + 10;
  if( house->getSpec().getLevel() > 2 )
  {
    drawGood( house, Good::wheat, 0, 0, startY );
    drawGood( house, Good::fish, 1, 0, startY );
    drawGood( house, Good::meat, 2, 0, startY );
    drawGood( house, Good::fruit, 3, 0, startY );
    drawGood( house, Good::vegetable, 4, 0, startY );
  }
  else
  {
    Label* lb = new Label( this, lbCrime->getRelativeRect() + Point( 0, 30 ) );
    lb->setHeight( 40 );
    lb->setLineIntervalOffset( -6 );
    lb->setText( _("##house_provide_food_themselves##") );
    lb->setWordwrap( true );
    startY = lb->getTop();
  }

  drawGood( house, Good::pottery, 0, 1, startY );
  drawGood( house, Good::furniture, 1, 1, startY );
  drawGood( house, Good::oil, 2, 1, startY );
  drawGood( house, Good::wine, 3, 1, startY );
}

InfoBoxHouse::~InfoBoxHouse() {}

void InfoBoxHouse::drawHabitants( HousePtr house )
{
  // citizen or patrician picture
  int picId = house->getSpec().isPatrician() ? 541 : 542;
   
  Picture& citPic = Picture::load( ResourceGroup::panelBackground, picId );
  _getBlackFrame()->setIcon( citPic, Point( 15, 5 ) );

  // number of habitants
  Label* lbHabitants = new Label( this, Rect( 60, 157, getWidth() - 16, 157 + citPic.getHeight() ) );

  std::string freeRoomText;
  int current = house->getHabitants().count();
  int freeRoom = house->getMaxHabitants() - current;
  if( freeRoom > 0 )
  {
    // there is some room for new habitants!
    freeRoomText = StringHelper::format( 0xff, "%d %s %d", current, _("##citizens_additional_rooms_for##"), freeRoom);
  }
  else if (freeRoom == 0)
  {
    // full house!
    freeRoomText = StringHelper::format( 0xff, "%d %s", current, _("##occupants##"));
  }
  else if (freeRoom < 0)
  {
    // too many habitants!
    freeRoomText = StringHelper::format( 0xff, "%d %s %d", current, _("##no_room_for_citizens##"),-freeRoom);
    lbHabitants->setFont( Font::create( FONT_2_RED ) );
  }

  lbHabitants->setText( freeRoomText );
}

void InfoBoxHouse::drawGood( HousePtr house, const Good::Type &goodType, const int col, const int row, const int startY )
{
  int qty = house->getGoodStore().getQty( goodType );
  std::string text = StringHelper::format( 0xff, "%d", qty);

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  Label* lb = new Label( this, Rect( Point( 30 + 100 * col, startY + 2 + 30 * row), Size( 80, 50) ) );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setIcon( pic );
  lb->setText( text );
  lb->setTextOffset( Point( 30, 0 ));
  //font.draw( *_d->bgPicture, text, 61 + 100 * col, startY + 30 * row, false );
}

}//end namespace gui
