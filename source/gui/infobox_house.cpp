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

#include <cstdio>

#include "infobox_house.hpp"

#include "gfx/tile.hpp"
#include "core/exception.hpp"
#include "core/gettext.hpp"
#include "gfx/decorator.hpp"
#include "objects/metadata.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "texturedbutton.hpp"
#include "gui/label.hpp"
#include "city/city.hpp"
#include "objects/market.hpp"
#include "objects/granary.hpp"
#include "core/utils.hpp"
#include "good/helper.hpp"
#include "objects/farm.hpp"
#include "objects/entertainment.hpp"
#include "objects/house.hpp"
#include "objects/religion.hpp"
#include "religion/romedivinity.hpp"
#include "objects/warehouse.hpp"
#include "gfx/engine.hpp"
#include "gui/special_orders_window.hpp"
#include "good/store.hpp"
#include "groupbox.hpp"
#include "walker/walker.hpp"
#include "objects/watersupply.hpp"
#include "objects/senate.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "events/event.hpp"
#include "game/settings.hpp"
#include "widget_helper.hpp"
#include "image.hpp"
#include "game/gamedate.hpp"
#include "dictionary.hpp"
#include "pushbutton.hpp"
#include "environment.hpp"
#include "core/gettext.hpp"
#include "dialogbox.hpp"
#include "game/infoboxmanager.hpp"
#include "events/playsound.hpp"
#include "infobox_land.hpp"

using namespace gfx;
using namespace gui::dialog;
using namespace events;

namespace gui
{

namespace infobox
{

class InfoboxHouseCreator : public InfoboxCreator
{
public:
  Infobox* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    HousePtr house = city->getOverlay( pos ).as<House>();
    if( house.isValid() && house->habitants().count() > 0 )
    {
      return &parent->add<AboutHouse>( city, city->tilemap().at( pos ) );
    }
    else
    {
      return &parent->add<AboutFreeHouse>( city, city->tilemap().at( pos ) );
    }
  }
};

REGISTER_OBJECT_INFOBOX( house, new InfoboxHouseCreator() )

AboutHouse::AboutHouse(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : Infobox( parent, Rect( 0, 0, 510, 360 ), Rect( 16, 150, 510 - 16, 360 - 50 ) )
{
  setupUI( ":/gui/infoboxhouse.gui" );

  _house = tile.overlay<House>();

  if( _house.isNull() )
  {
    Logger::warning( "!!! WARNING: Cant find house at [{0},{1}]", tile.pos().i(), tile.pos().j() );
    deleteLater();
    return;
  }

  events::dispatch<PlaySound>( "bmsel_house", 1, 100, audio::infobox, true );

  setTitle( _(_house->levelName()) );

  _btnExit()->setTooltipText( _("##advanced_houseinfo##") );

  Label& houseInfo = add<Label>( Rect( 30, 40, width() - 30, 40 + 100 ), "" );
  houseInfo.setWordwrap( true );

  std::string text = _house->evolveInfo();
  if( _house->level() == HouseLevel::greatPalace && text.empty() )
  {
    text =  "##greatPalace_info##";
  }
  else
  {
    if( text == "##nearby_building_negative_effect##" )
    {
      object::Type needBuilding;
      TilePos rPos;
      HouseSpecification spec = _house->spec().next();

      int unwish = spec.findUnwishedBuildingNearby( _house, needBuilding, rPos );

      if( !unwish )
        spec.findLowLevelHouseNearby( _house, rPos );

      text = _(text);
      OverlayPtr overlay = city->getOverlay( rPos );
      if( overlay.isValid() )
      {
        std::string housePrettyType;
        if( overlay->type() == object::house )
        {
          HousePtr house = overlay.as<House>();
          housePrettyType = house.isValid() ? house->levelName() : "##unknown_house_type##";
        }
        else
        {
          housePrettyType = overlay.isValid() ? overlay->info().prettyName() : "";
        }

        housePrettyType = utils::format( 0xff, "(%s)", _(housePrettyType) );
        text = utils::replace( text, "{0}", housePrettyType );
      }
    }
  }

  houseInfo.setText( _(text) );

  INIT_WIDGET_FROM_UI( TexturedButton*, btnHelp )
  if( btnHelp )
  {
    Rect rect = btnHelp->relativeRect();
    rect += Point( btnHelp->width() + 5, 0 );
    rect.rright() += 60;
    auto& btnHabitants = add<PushButton>( rect, "Habitants", -1, false, PushButton::whiteBorderUp );
    CONNECT( &btnHabitants, onClicked(), this, AboutHouse::_showHbtInfo )

    rect += Point( btnHabitants.width() + 5, 0 );
    auto& btnServices = add<PushButton>( rect, "Services", -1, false, PushButton::whiteBorderUp );
    CONNECT( &btnServices, onClicked(), this, AboutHouse::_showSrvcInfo )
  }

  drawHabitants( _house );

  int taxes = _house->spec().taxRate();
  std::string taxesStr = "##house_not_taxation##";
  if( taxes > 0 )
  {
    if( _house->getServiceValue( Service::forum ) > 0 )
    {
      taxesStr = fmt::format( "{} {}", taxes * _house->size().area(), _("##house_pay_tax##") );
    }
    else
    {
      DateTime lastTax = _house->lastTaxationDate();
      if( game::Date::current().year() > lastTax.year() )
      {
        taxesStr = "##no_tax_in_this_year##";
      }
      else
      {        
        taxesStr = "##no_visited_by_taxman##";
      }
    }
  }

  auto& taxesLb = add<Label>( Rect( 16 + 35, 177, width() - 16, 177 + 20 ), _( taxesStr ) );

  std::string aboutCrimes = _("##house_not_report_about_crimes##");
  Label& lbCrime = add<Label>( taxesLb.relativeRect() + Point( 0, 22 ), aboutCrimes );

  int startY = lbCrime.bottom() + 10;
  if( _house->level() > HouseLevel::tent )
  {
    drawGood( _house, good::wheat, 0, 0, startY );
    drawGood( _house, good::fish, 1, 0, startY );
    drawGood( _house, good::meat, 2, 0, startY );
    drawGood( _house, good::fruit, 3, 0, startY );
    drawGood( _house, good::vegetable, 4, 0, startY );
  }
  else
  {
    Label& lb = add<Label>( lbCrime.relativeRect() + Point( 0, 30 ) );
    lb.setHeight( 40 );
    lb.setLineIntervalOffset( -6 );
    lb.setText( _("##house_provide_food_themselves##") );
    lb.setWordwrap( true );
    startY = lb.top();
  }

  drawGood( _house, good::pottery, 0, 1, startY );
  drawGood( _house, good::furniture, 1, 1, startY );
  drawGood( _house, good::oil, 2, 1, startY );
  drawGood( _house, good::wine, 3, 1, startY );
}

AboutHouse::~AboutHouse() {}

void AboutHouse::drawHabitants( HousePtr house )
{
  // citizen or patrician picture
  int picId = house->spec().isPatrician() ? 541 : 542;
   
  Picture citPic( gui::rc.panel, picId );
  _lbBlackFrame()->setIcon( citPic, Point( 15, 5 ) );

  // number of habitants
  Label& lbHabitants = add<Label>( Rect( 60, 157, width() - 16, 157 + citPic.height() ) );

  std::string freeRoomText;
  int current = house->habitants().count();
  int freeRoom = house->capacity() - current;
  if( freeRoom > 0 )
  {
    // there is some room for new habitants!
    freeRoomText = fmt::format( "{} {} {}", current, _("##citizens_additional_rooms_for##"), freeRoom);
  }
  else if (freeRoom == 0)
  {
    // full house!
    freeRoomText = fmt::format( "{} {}", current, _("##occupants##"));
  }
  else if (freeRoom < 0)
  {
    // too many habitants!
    freeRoomText = fmt::format( "{} {} {}", current, _("##no_room_for_citizens##"),-freeRoom);
    lbHabitants.setFont( Font::create( FONT_2_RED ) );
  }

  lbHabitants.setText( freeRoomText );
}

void AboutHouse::drawGood(HousePtr house, const good::Product& goodType, const int col, const int row, const int startY )
{
  int qty = house->store().qty( goodType );
  std::string text = utils::i2str(qty);

  // pictures of goods
  const Picture& pic = good::Helper::picture( goodType );
  Label& lb = add<Label>( Rect( Point( 30 + 100 * col, startY + 2 + 30 * row), Size( 80, 50) ) );
  lb.setFont( Font::create( FONT_2 ) );
  lb.setIcon( pic );
  lb.setText( text );
  lb.setTextOffset( Point( 30, 0 ));
  //font.draw( *_d->bgPicture, text, 61 + 100 * col, startY + 30 * row, false );
}

bool AboutHouse::onEvent(const NEvent& event)
{
  if( event.EventType == sEventKeyboard )
  {
    if( event.keyboard.control && event.keyboard.shift && !event.keyboard.pressed )
    {
      switch( event.keyboard.key )
      {
      case KEY_COMMA:
      case KEY_PERIOD:
        _house->__debugChangeLevel( event.keyboard.key == KEY_COMMA ? +1 : -1);
        return true;
      default:
         break;
      }
    }
  }

  return Infobox::onEvent( event );
}

void AboutHouse::_showHelp() { ui()->add<DictionaryWindow>( "house" ); }

void AboutHouse::_showHbtInfo()
{
  std::string workerState = utils::format( 0xff, "Live=%d\nUnemployed=%d\nHired=%d\nNewborn=%d\nChild=%d\nIn school=%d\nStudents=%d\nMature=%d\nAged(not work)=%d",
                                                  _house->habitants().count(),
                                                  (int)_house->unemployed(),
                                                  _house->hired(),
                                                  _house->habitants().count( CitizenGroup::newborn ),
                                                  _house->habitants().child_n(),
                                                  _house->habitants().scholar_n(),
                                                  _house->habitants().student_n(),
                                                  _house->habitants().mature_n(),
                                                  _house->habitants().aged_n() );

  Dialog& dialog = ui()->add<Dialog>( Rect( 0, 0, 400, 400 ), "Habitants", workerState, Dialog::btnOk );
  CONNECT( &dialog, onOk(), &dialog, Dialog::deleteLater )
}

void AboutHouse::_showSrvcInfo()
{
  std::string srvcState = fmt::format( "Health={}",
                                        (int)_house->state( pr::health ));

  Dialog& dialog = ui()->add<Dialog>( Rect( 0, 0, 400, 400 ), "Services", srvcState, Dialog::btnOk );
  dialog.moveTo( Widget::parentCenter );
  CONNECT( &dialog, onOk(), &dialog, Dialog::deleteLater )
}

}

}//end namespace gui
