// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include <cstdio>

#include "oc3_gui_info_box.hpp"

#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "oc3_gettext.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_building_data.hpp"
#include "oc3_house_level.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_event.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_city.hpp"
#include "oc3_scenario.hpp"
#include "oc3_building_market.hpp"
#include "oc3_granary.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_goodhelper.hpp"
#include "oc3_farm.hpp"
#include "oc3_building_entertainment.hpp"
#include "oc3_building_house.hpp"
#include "oc3_building_religion.hpp"
#include "oc3_divinity.hpp"
#include "oc3_building_warehouse.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_special_orders_window.hpp"
#include "oc3_goodstore.hpp"
#include "oc3_groupbox.hpp"
#include "oc3_walker.hpp"
#include "oc3_building_senate.hpp"

class InfoBoxSimple::Impl
{
public:
  PictureRef bgPicture;
  Label* lbTitle;
  Label* lbText;
  PushButton* btnExit;
  PushButton* btnHelp;
  bool isAutoPosition;
};

InfoBoxSimple::InfoBoxSimple( Widget* parent, const Rect& rect, const Rect& blackArea, int id )
: Widget( parent, id, rect ), _d( new Impl )
{
  // create the title
  _d->lbTitle = new Label( this, Rect( 50, 10, getWidth()-50, 10 + 30 ), "", true );
  _d->lbTitle->setFont( Font::create( FONT_3 ) );
  _d->lbTitle->setTextAlignment( alignCenter, alignCenter );
  _d->isAutoPosition = true;

  _d->btnExit = new TexturedButton( this, Point( getWidth() - 39, getHeight() - 39 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  _d->btnExit->setTooltipText( _("##infobox_tooltip_exit##") );

  _d->btnHelp = new TexturedButton( this, Point( 14, getHeight() - 39 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnHelp->setTooltipText( _("##infobox_tooltip_help##") );

  CONNECT( _d->btnExit, onClicked(), this, InfoBoxLand::deleteLater );

  _d->bgPicture.init( getSize() );
  _d->lbText = new Label( this, Rect( 32, 64, 510 - 32, 300 - 48 ) );
  _d->lbText->setWordWrap( true );

  // draws the box and the inner black box
  PictureDecorator::draw( *_d->bgPicture, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  if( blackArea.getSize().getArea() > 0 )
  {
    PictureDecorator::draw( *_d->bgPicture, blackArea, PictureDecorator::blackFrame );
  }

  _afterCreate();
}

void InfoBoxSimple::setText( const std::string& text )
{
  _d->lbText->setText( text );
}

InfoBoxSimple::~InfoBoxSimple()
{
}

void InfoBoxSimple::draw( GfxEngine& engine )
{
  engine.drawPicture( getBgPicture(), getScreenLeft(), getScreenTop() );
  Widget::draw( engine );
}

Picture& InfoBoxSimple::getBgPicture()
{
  return *_d->bgPicture;
}

bool InfoBoxSimple::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return getParent()->getAbsoluteRect().isPointInside( point );
}

bool InfoBoxSimple::onEvent( const NEvent& event)
{
  switch( event.EventType )
  {
  case OC3_MOUSE_EVENT:
    if( event.MouseEvent.Event == OC3_RMOUSE_LEFT_UP )
    {
      deleteLater();
      return true;
    }
    else if( event.MouseEvent.Event == OC3_LMOUSE_LEFT_UP )
    {
      return true;
    }
  break;

  default:
  break;
  }

  return Widget::onEvent( event );
}

void InfoBoxSimple::setTitle( const std::string& title )
{
  _d->lbTitle->setText( title );
}

bool InfoBoxSimple::isAutoPosition() const
{
  return _d->isAutoPosition;
}

void InfoBoxSimple::setAutoPosition( bool value )
{
  _d->isAutoPosition = value;
}

void InfoBoxSimple::_drawWorkers(const Point &pos, int picId, int need, int have )
{
  if( 0 == need )
    return;

  _d->bgPicture->draw( Picture::load( ResourceGroup::panelBackground, picId ), pos );

  // number of workers
  std::string text = StringHelper::format( 0xff, "%d %s (%d %s)", have, _("employers"), need, _("requierd") );
  Font::create( FONT_2 ).draw( *_d->bgPicture, text, pos + Point( 20, 5), false );
}

InfoBoxWorkingBuilding::InfoBoxWorkingBuilding( Widget* parent, WorkingBuildingPtr building)
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 136, 510 - 16, 136 + 62 ) )
{
  setTitle( BuildingDataHolder::instance().getData( building->getType() ).getPrettyName() );

  _drawWorkers( Point( 32, 150 ), 542, building->getMaxWorkers(), building->getWorkers() );

  std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire",
                                           (int)building->getDamageLevel(), (int)building->getFireLevel());

  new Label( this, Rect( 50, getHeight() - 50, getWidth() - 50, getHeight() - 16 ), text );
  new Label( this, Rect( 16, 50, getWidth() - 16, 130 ), "", false, Label::bgNone, lbHelpId );
}

void InfoBoxWorkingBuilding::setText(const std::string& text)
{
  if( Widget* lb = findChild( lbHelpId ) )
      lb->setText( text );
}

InfoBoxSenate::InfoBoxSenate( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 290 ), Rect( 16, 136, 510 - 16, 136 + 62 ) )
{
  SenatePtr senate = tile.getTerrain().getOverlay().as<Senate>();
  setTitle( BuildingDataHolder::instance().getData( B_SENATE ).getPrettyName() );

  // number of workers
  _drawWorkers( Point( 32, 136), 542, senate->getMaxWorkers(), senate->getWorkers() );

  getBgPicture().draw( GoodHelper::getPicture( Good::denaries ), 16, 35);

  std::string denariesStr = StringHelper::format( 0xff, "%s %d", _("##senate_save##"), senate->getFunds() );

  new Label( this, Rect( 60, 35, getWidth() - 16, 35 + 30 ), denariesStr );
  new Label( this, Rect( 60, 215, 60 + 300, 215 + 24 ), _("##open_rating_adviser##") );
  new TexturedButton( this, Point( 350, 215 ), Size(28), -1, 289 );
  new Label( this, Rect( 16, 70, getWidth() - 16, 70 + 60 ), _("##senate_help_text##") );
}

InfoBoxSenate::~InfoBoxSenate()
{
}

InfoBoxHouse::InfoBoxHouse( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 360 ), Rect( 16, 150, 510 - 16, 360 - 150 ) )
{
  HousePtr house = tile.getTerrain().getOverlay().as<House>();
  setTitle( house->getName() );

  drawHabitants( house );

  int taxes = -1;
  std::string taxesStr = taxes > 0
                           ? StringHelper::format( 0xff, _("Aucun percepteur ne passe ici. Ne paye pas de taxes") )
                           : StringHelper::format( 0xff, _("Paye %d Denarii de taxes par mois"), taxes );

  Label* taxesLb = new Label( this, Rect( 16 + 15, 177, getWidth() - 16, 177 + 20 ), taxesStr );

  std::string aboutCrimes = _("Inhabitants didn't report about crimes");
  Label* lbCrime = new Label( this, taxesLb->getRelativeRect() + Point( 0, 22 ), aboutCrimes );

  int startY = lbCrime->getBottom() + 10;
  if( house->getLevelSpec().getHouseLevel() > 2 )
  {
    drawGood( house, Good::wheat, 0, 0, startY );
  }
  else
  {
    Label* lb = new Label( this, lbCrime->getRelativeRect() + Point( 0, 30 ) );
    lb->setHeight( 40 );
    lb->setLineIntervalOffset( -6 );
    lb->setText( _("Inabitants of tents provide food themselves, conducting a subsistence economy") );
    lb->setWordWrap( true );
    startY = lb->getTop();
  }

  drawGood( house, Good::pottery, 0, 1, startY );
  drawGood( house, Good::furniture, 1, 1, startY );
  drawGood( house, Good::oil, 2, 1, startY );
  drawGood( house, Good::wine, 3, 1, startY );
}

InfoBoxHouse::~InfoBoxHouse()
{
}

void InfoBoxHouse::drawHabitants( HousePtr house )
{
  // citizen or patrician picture
  int picId = house->getLevelSpec().isPatrician() ? 541 : 542;
   
  Picture& citPic = Picture::load( ResourceGroup::panelBackground, picId );
  _d->bgPicture->draw( citPic, 16+15, 157 );

  // number of habitants
  Label* lbHabitants = new Label( this, Rect( 60, 157, getWidth() - 16, 157 + citPic.getHeight() ) );

  std::string freeRoomText;
  int freeRoom = house->getMaxHabitants() - house->getNbHabitants();
  if( freeRoom > 0 )
  {
    // there is some room for new habitants!
    freeRoomText = StringHelper::format( 0xff, _("%d citizens, additional rooms for %d"), house->getNbHabitants(), freeRoom);
  }
  else if (freeRoom == 0)
  {
    // full house!
    freeRoomText = StringHelper::format( 0xff, _("%d citizens"), house->getNbHabitants());
  }
  else if (freeRoom < 0)
  {
    // too many habitants!
    freeRoomText = StringHelper::format( 0xff, _("%d citizens, %d habitants en trop"), house->getNbHabitants(), -freeRoom);
    lbHabitants->setFont( Font::create( FONT_2_RED ) );
  }

  lbHabitants->setText( freeRoomText );
}

void InfoBoxHouse::drawGood( HousePtr house, const Good::Type &goodType, const int col, const int row, const int startY )
{
  Font font = Font::create( FONT_2 );
  int qty = house->getGoodStore().getCurrentQty( goodType );

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  _d->bgPicture->draw(pic, 31 + 100 * col, startY + 2 + 30 * row);

  std::string text = StringHelper::format( 0xff, "%d", qty);
  font.draw( *_d->bgPicture, text, 61 + 100 * col, startY + 30 * row, false );
}

GuiInfoFactory::GuiInfoFactory( Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 147, 510 - 16, 147 + 62) )
{
  FactoryPtr building = tile.getTerrain().getOverlay().as<Factory>();
  setTitle( BuildingDataHolder::getPrettyName( building->getType() ) );

  // paint progress
  std::string text = StringHelper::format( 0xff, _("Le travail est a %d%% termine."), building->getProgress() );
  Label* lbProgress = new Label( this, Rect( _d->lbTitle->getLeftdownCorner(), Size( getWidth() - 32, 30 ) ), text );

  if( building->getOutGoodType() != Good::none )
  {
    getBgPicture().draw( GoodHelper::getPicture( building->getOutGoodType() ), 10, 10);
  }

  // paint picture of in good
  if( building->getInGood().type() != Good::none )
  {
    getBgPicture().draw( GoodHelper::getPicture( building->getInGood().type() ), Point( 32, 20 ) );
    std::string text = StringHelper::format( 0xff, _("%s stock: %d units"),
                                             GoodHelper::getName( building->getInGood().type() ).c_str(),
                                             building->getInGood()._currentQty / 100 );

    new Label( this, Rect( _d->lbTitle->getLeftdownCorner(), Size( getWidth() - 32, 30 ) ), text );
  }

  _drawWorkers( Point( 16, 157 ), 542, building->getMaxWorkers(), building->getWorkers() );
}

std::string GuiInfoFactory::getInfoText()
{
  /*std::string textKey = GoodHelper::getName( _building->getOutGood().type() );
  if (_building->isActive() == false)
  {
    textKey+= "- Production arretee sous ordre du gouverneur";
  }
  else if (_building->getInGood().type() != Good::G_NONE && _building->getInGood()._currentQty == 0)
  {
    textKey+= "- Production arretee par manque de matiere premiere";
  }
  else
  {
    int nbWorkers = _building->getWorkers();
    if (nbWorkers == 0)
    {
       textKey+= "- Production arretee par manque de personnel";
    }
    else if (nbWorkers < 5)
    {
       textKey+= "- Production quasimment a l'arret par manque de main-d'oeuvre";
    }
    else if (nbWorkers < 7)
    {
       textKey+= "- Production tres ralentie par manque d'employes";
    }
    else if (nbWorkers < 9)
    {
       textKey+= "- Production limitee par manque de personnel";
    }
    else if (nbWorkers < 10)
    {
       textKey+= "- Production legerement ralentie par manque d'employes";
    }
    else
    {
       textKey+= "- Production a son maximum";
    }
  }

  return _(textKey.c_str());*/
}

InfoBoxGranary::InfoBoxGranary( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 280 ), Rect( 16, 80, 510 - 16, 80 + 62) )
{
  GranaryPtr _granary = tile.getTerrain().getOverlay().as<Granary>();
  Size btnOrdersSize( 350, 20 );
  PushButton* btnOrders = new PushButton( this, Rect( Point( (getWidth() - btnOrdersSize.getWidth())/ 2, getHeight() - 34 ), btnOrdersSize),
                                         _("##special_orders##"), -1, false, PushButton::whiteBorderUp );
  CONNECT( btnOrders, onClicked(), this, InfoBoxGranary::showSpecialOrdersWindow );

  setTitle( BuildingDataHolder::getPrettyName( _granary->getType() ) );

  // summary: total stock, free capacity
  std::string desc = StringHelper::format( 0xff, _("%d unites en stock. Espace pour %d unites."),
                                                  _granary->getGoodStore().getCurrentQty(),
                                                  _granary->getGoodStore().getMaxQty() );

  Label* lbUnits = new Label( this, Rect( _d->lbTitle->getLeftdownCorner(), Size( getWidth() - 16, 40 )), desc );

  drawGood(Good::wheat, 0, lbUnits->getBottom() );
  drawGood(Good::meat, 0, lbUnits->getBottom() + 25);
  drawGood(Good::fruit, 1, lbUnits->getBottom() );
  drawGood(Good::vegetable, 1, lbUnits->getBottom() + 25);

  _drawWorkers( Point( 32, lbUnits->getBottom() + 50 ), 542, _granary->getMaxWorkers(), _granary->getWorkers() );
}

InfoBoxGranary::~InfoBoxGranary()
{
}

void InfoBoxGranary::showSpecialOrdersWindow()
{
  Point pos;
  if( getTop() > (int)getParent()->getHeight() / 2 )
  {
    pos = Point( getScreenLeft(), getScreenBottom() - 450 );   
  }
  else
  {
    pos = getAbsoluteRect().UpperLeftCorner;
  }

  new GranarySpecialOrdersWindow( getParent(), pos, _granary );
}

void InfoBoxGranary::drawGood(const Good::Type &goodType, int col, int paintY)
{
  std::string goodName = GoodHelper::getName( goodType );

  Font font = Font::create( FONT_2 );
  int qty = _granary->getGoodStore().getCurrentQty(goodType);

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  _d->bgPicture->draw(pic, (col == 0 ? 31 : 250), paintY);

  std::string outText = StringHelper::format( 0xff, "%d %s", qty, goodName.c_str() );
  font.draw( *_d->bgPicture, outText, (col == 0 ? 61 : 280), paintY, false );
}

InfoBoxWarehouse::InfoBoxWarehouse( Widget* parent, const Tile& tile )
: InfoBoxSimple( parent, Rect( 0, 0, 510, 360 ) )
{
  _warehouse = tile.getTerrain().getOverlay().as<Warehouse>();
  Size btnOrdersSize( 350, 20 );
  PushButton* btnOrders = new PushButton( this, Rect( Point( (getWidth() - btnOrdersSize.getWidth()) / 2, getHeight() - 34 ), btnOrdersSize ),
                                   _("##special_orders##"), -1, false, PushButton::whiteBorderUp );

  CONNECT( btnOrders, onClicked(), this, InfoBoxWarehouse::showSpecialOrdersWindow );

  setTitle( BuildingDataHolder::getPrettyName( _warehouse->getType() ) );

  // summary: total stock, free capacity
  int _paintY = _d->lbTitle->getBottom();

  drawGood(Good::wheat, 0, _paintY+25);
  drawGood(Good::vegetable, 0, _paintY+50);
  drawGood(Good::fruit, 0, _paintY+75);
  drawGood(Good::olive, 0, _paintY+100);
  drawGood(Good::grape, 0, _paintY+125);

  drawGood(Good::meat, 1, _paintY+25);
  drawGood(Good::wine, 1, _paintY+50);
  drawGood(Good::oil, 1, _paintY+75);
  drawGood(Good::iron, 1, _paintY+100);
  drawGood(Good::timber, 1, _paintY+125);

  drawGood(Good::clay, 2, _paintY+25);
  drawGood(Good::marble, 2, _paintY+50);
  drawGood(Good::weapon, 2, _paintY+75);
  drawGood(Good::furniture, 2, _paintY+100);
  drawGood(Good::pottery, 2, _paintY+125);

  Point workerFramePos( 16, 170 );
  PictureDecorator::draw( *_d->bgPicture,
                          Rect( workerFramePos, Size( getWidth()-32, 62 ) ),
                          PictureDecorator::blackFrame );

  _drawWorkers( workerFramePos + Point( 20, 10 ), 542, _warehouse->getMaxWorkers(), _warehouse->getWorkers() );
}

InfoBoxWarehouse::~InfoBoxWarehouse()
{
}

void InfoBoxWarehouse::showSpecialOrdersWindow()
{
  Point pos;
  if( getTop() > (int)getParent()->getHeight() / 2 )
  {
    pos = Point( getScreenLeft(), getScreenBottom() - 450 );   
  }
  else
  {
    pos = getAbsoluteRect().UpperLeftCorner;
  }

  new WarehouseSpecialOrdersWindow( getParent(), pos, _warehouse );
}

void InfoBoxWarehouse::drawGood( const Good::Type &goodType, int col, int paintY )
{
  std::string goodName = GoodHelper::getName( goodType );

  Font font = Font::create( FONT_2 );
  int qty = _warehouse->getGoodStore().getCurrentQty(goodType);

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  getBgPicture().draw( pic, col * 150 + 15, paintY );

  std::string outText = StringHelper::format( 0xff, "%d %s", qty, goodName.c_str() );
  font.draw( *_d->bgPicture, outText, col * 150 + 45, paintY, false );
}

InfoBoxTemple::InfoBoxTemple( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ) )
{
  TemplePtr temple = tile.getTerrain().getOverlay().as<Temple>();
  RomeDivinityPtr divn = temple->getDivinity();

  std::string text = StringHelper::format( 0xff, "##Temple of ##%s (%s)", 
                                                 divn->getName().c_str(), 
                                                 divn->getShortDescription().c_str() );
  setTitle( text );

  _drawWorkers( Point( 32, 56 + 12), 542, temple->getMaxWorkers(), temple->getWorkers() );
  _d->bgPicture->draw( temple->getDivinity()->getPicture(), 192, 140 );
}

InfoBoxTemple::~InfoBoxTemple()
{
}

InfoBoxMarket::InfoBoxMarket( Widget* parent, const Tile& tile )
    : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ) )
{
   MarketPtr market = tile.getTerrain().getOverlay().as<Market>();

   Label* lbAbout = new Label( this, _d->lbTitle->getRelativeRect() + Point( 0, 30 ) );

   setTitle( _("building_market") );

   if( market->getWorkers() > 0 )
   {
     GoodStore& goods = market->getGoodStore();
     int furageSum = 0;
     // for all furage types of good
     for (int goodType = 0; goodType<Good::olive; ++goodType)
     {
       furageSum += goods.getCurrentQty( (Good::Type)goodType );
     }

     int paintY = 78;
     if( 0 < furageSum )
     {
       drawGood( market, Good::wheat, 0, paintY );
       drawGood( market, Good::fish, 1, paintY);
       drawGood( market, Good::meat, 2, paintY);
       drawGood( market, Good::fruit, 3, paintY);
       drawGood( market, Good::vegetable, 4, paintY);
       lbAbout->setHeight( 25 );
     }
     else
     {
       lbAbout->setHeight( 50 );
     }

     paintY += 24;
     drawGood( market, Good::pottery, 0, paintY);
     drawGood( market, Good::furniture, 1, paintY);
     drawGood( market, Good::oil, 2, paintY);
     drawGood( market, Good::wine, 3, paintY);

     lbAbout->setText( 0 == furageSum ? _("##market_search_food_source##") : _("##market_about##"));
   }
   else
   {
     lbAbout->setHeight( 50 );
     lbAbout->setText( _("##market_not_work##") );
   }

   _drawWorkers( Point( 32, 138 ), 542, market->getMaxWorkers(), market->getWorkers() );
}

InfoBoxMarket::~InfoBoxMarket()
{
}

void InfoBoxMarket::drawGood( MarketPtr market, const Good::Type &goodType, int index, int paintY )
{
  int startOffset = 25;

  int offset = ( getWidth() - startOffset * 2 ) / 5;
  std::string goodName = GoodHelper::getName( goodType );

  // pictures of goods
  Picture pic = GoodHelper::getPicture( goodType );
  Point pos( index * offset + startOffset, paintY );
  _d->bgPicture->draw( pic, pos.getX(), pos.getY() );

  std::string outText = StringHelper::format( 0xff, "%d", market->getGoodStore().getCurrentQty(goodType) );
  Font font2 = Font::create( FONT_2 );
  font2.draw(*_d->bgPicture, outText, pos.getX() + 30, pos.getY(), false );
}

GuiBuilding::GuiBuilding( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 450, 220 ), Rect( 16, 60, 450 - 16, 60 + 50) )
{
  BuildingPtr building = tile.getTerrain().getOverlay().as<Building>();
  setTitle( BuildingDataHolder::getPrettyName( building->getType() ) );
}

InfoBoxLand::InfoBoxLand( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 60, 510 - 16, 60 + 180) )
{ 
  Label* lbText = new Label( this, Rect( 38, 239, 470, 338 ), "", true, Label::bgNone, lbTextId );
  lbText->setFont( Font::create( FONT_2 ) );
  lbText->setWordWrap( true );

  if( tile.getTerrain().isTree() )
  {
    setTitle( _("##trees_and_forest_caption") );
    lbText->setText( _("##trees_and_forest_text"));
  } 
  else if( tile.getTerrain().isWater() )
  {
    setTitle( _("##water_caption") );
    lbText->setText( _("##water_text"));
  }
  else if( tile.getTerrain().isRock() )
  {
    setTitle( _("##rock_caption") );
    lbText->setText( _("##rock_text"));
  }
  else if( tile.getTerrain().isRoad() )
  {
    if( tile.getTerrain().getOverlay()->getType() == B_PLAZA )
    {
      setTitle( _("##plaza_caption") );
      lbText->setText( _("##plaza_text"));
    }
    else 
    {
     setTitle( _("##road_caption") );
      lbText->setText( _("##road_text"));
    }
  }
  else 
  {
    setTitle( _("##clear_land_caption") );
    lbText->setText( _("##clear_land_text"));
  }
 
  CityPtr oCity = Scenario::instance().getCity();
  //Tilemap& oTilemap = oCity->getTilemap();
  //int size = oTilemap.getSize();
  //int border_size = (162 - size) / 2;
  
  //int index = (size - tile.getJ() - 1 + border_size) * 162 + tile.getI() + border_size;
  
  const TerrainTile& terrain = tile.getTerrain();

  std::string text = StringHelper::format( 0xff, "Tile at: (%d,%d) %04X %02X %04X %02X %02X %02X",
                                           tile.getI(), tile.getJ(),  
                                          ((short int) terrain.getOriginalImgId() ),
                                          ((short int) terrain.getEdgeData()),
                                          ((short int) terrain.getTerrainData()),  
                                          ((short int) terrain.getTerrainRndmData()),  
                                          ((short int) terrain.getRandomData()),
                                          ((short int) terrain.getElevationData() ) );
  
  lbText->setText( text );
}

void InfoBoxLand::setText( const std::string& text )
{
  if( Widget* lb = findChild( lbTextId ) )
    lb->setText( text );
}

InfoBoxFreeHouse::InfoBoxFreeHouse( Widget* parent, const Tile& tile )
    : InfoBoxLand( parent, tile )
{
    setTitle( _("##freehouse_caption##") );

    if( tile.getTerrain().getOverlay().as<Construction>()->getAccessRoads().size() == 0 )
    {
      setText( _("##freehouse_text_noroad##") );
    }
    else
    {
      setText( _("##freehouse_text##") );
    }
}   

InfoBoxRawMaterial::InfoBoxRawMaterial( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 146, 510 - 16, 146 + 64 ) )
{
  FactoryPtr rawmb = tile.getTerrain().getOverlay().as<Factory>();

  if( rawmb->getOutGoodType() != Good::none )
  {
    Picture pic = GoodHelper::getPicture( rawmb->getOutGoodType() );
    _d->bgPicture->draw( pic, 10, 10 );
  }

  _drawWorkers( Point( 32, 160 ), 542, rawmb->getMaxWorkers(), rawmb->getWorkers() );

  std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire",
                                          (int)rawmb->getDamageLevel(), (int)rawmb->getFireLevel());
  new Label( this, Rect( 50, getHeight() - 50, getWidth() - 50, getHeight() - 16 ), text );

  text = StringHelper::format( 0xff, _("Production %d%% complete."), rawmb->getProgress() );
  Label* lbProgress = new Label( this, Rect( 32, 50, getWidth() - 16, 50 + 32 ), text );
  Label* lbAbout = new Label( this, Rect( 32, lbProgress->getBottom() + 6, getWidth() - 16, 130 ) );

  std::string desc, name;
  //GoodType goodType = G_NONE;
  switch( rawmb->getType() )
  {
    case B_WHEAT_FARM:
      desc.assign( _("##farm_description_wheat##") );
      name.assign( _("##farm_title_wheat##") );
      //goodType = G_WHEAT;
      break;
    case B_FRUIT_FARM:
      desc.assign( _("##farm_description_fruit##") );
      name.assign( _("##farm_title_fruit##") );
      //goodType = G_FRUIT;
      break;
    case B_OLIVE_FARM:
      desc.assign( _("##farm_description_olive##") );
      name.assign( _("##farm_title_olive##") );
      //goodType = G_OLIVE;
      break;
    case B_GRAPE_FARM:
      desc.assign( _("##farm_description_vine##") );
      name.assign( _("##farm_title_vine##") );
      //goodType = G_GRAPE;
      break;
    case B_PIG_FARM:
      desc.assign( _("##farm_description_meat##") );
      name.assign( _("##farm_title_meat##") );
      //goodType = G_MEAT;
      break;
    case B_VEGETABLE_FARM:
      desc.assign( _("##farm_description_vegetable##") );
      name.assign( _("##farm_title_vegetable##") );
      //goodType = G_VEGETABLE;
      break;
    default:
    break;
  }

  new Label( this, Rect( 32, 236, getWidth() - 50, getHeight() - 50 ), desc );

  setTitle( name );

  text = _("##farm_working_normally##");
  if( rawmb->getWorkers() == 0 )
  {
    text = _("##farm_have_no_workers##");
  }
  else if( rawmb->getWorkers() <= rawmb->getMaxWorkers() / 2 )
  {
    text = _("##farm_working_bad##");
  }

  lbAbout->setText( text );
}

InfoBoxRawMaterial::~InfoBoxRawMaterial()
{
}

InfoBoxCitizen::InfoBoxCitizen(Widget* parent, const WalkerList& walkers )
  : InfoBoxSimple( parent, Rect( 0, 0, 460, 350 ), Rect( 18, 40, 460 - 18, 350 - 120 ) )
{
  Picture& bg = getBgPicture();
  PictureDecorator::draw( bg, Rect( 25, 100, getWidth() - 25, getHeight() - 130), PictureDecorator::whiteBorderA );
  //mini screenshot from citizen pos need here
  PictureDecorator::draw( bg, Rect( 25, 45, 25 + 52, 45 + 52), PictureDecorator::blackArea );

  Label* lbName = new Label( this, Rect( 90, 108, getWidth() - 30, 108 + 35) );
  lbName->setFont( Font::create( FONT_3 ));
  Label* lbType = new Label( this, Rect( 90, 145, getWidth() - 30, 145 + 25) );
  Label* lbThinks = new Label( this, Rect( 90, 170, getWidth() - 30, getHeight() - 140), "Citizen's thoughts will be placed here" );
  Label* lbCitizenPic = new Label( this, Rect( 30, 112, 30 + 55, 112 + 80) );

  if( !walkers.empty() )
  {
    WalkerPtr walker = walkers.front();
    lbName->setText( walker->getName() );
    lbType->setText( WalkerHelper::getPrettyTypeName( (WalkerType)walker->getType() ) );
    lbThinks->setText( walker->getThinks() );
    lbCitizenPic->setBackgroundPicture( WalkerHelper::getBigPicture( (WalkerType)walker->getType() ) );
  }
}

InfoBoxCitizen::~InfoBoxCitizen() {
}


InfoBoxColosseum::InfoBoxColosseum(Widget *parent, const Tile &tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 470, 300), Rect( 16, 145, 470 - 16, 145 + 100 ) )
{
  CollosseumPtr colloseum = tile.getTerrain().getOverlay().as<Collosseum>();
  setTitle( BuildingDataHolder::getPrettyName( B_COLLOSSEUM ) );

  _drawWorkers( Point( 40, 150), 542, colloseum->getMaxWorkers(), colloseum->getWorkers() );

  std::string text = StringHelper::format( 0xff, "Animal contest runs for another %d days", 0 );
  new Label( this, Rect( 35, 190, getWidth() - 35, 190 + 20 ), text );

  text = StringHelper::format( 0xff, "Gladiator bouts runs for another %d days", 0 );
  new Label( this, Rect( 35, 210, getWidth() - 35, 210 + 20 ), text );
}

InfoBoxColosseum::~InfoBoxColosseum()
{

}
