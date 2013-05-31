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

#include "oc3_gui_info_box.hpp"

#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_gettext.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_building_data.hpp"
#include "oc3_house_level.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_event.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_label.hpp"
#include "oc3_city.hpp"
#include "oc3_scenario.hpp"
#include "oc3_market.hpp"
#include "oc3_granary.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_goodhelper.hpp"
#include "oc3_farm.hpp"
#include "oc3_religion_building.hpp"
#include "oc3_divinity.hpp"

class InfoBoxHelper
{
public:
  std::vector<Picture*> mapPictureGood;  // index=GoodType, value=Picture
    
  static InfoBoxHelper& getInstance()
  {
    static InfoBoxHelper inst;
    return inst;
  }

  InfoBoxHelper()
  {
    mapPictureGood.resize(G_MAX);
    mapPictureGood[int(G_WHEAT)    ] = &Picture::load( ResourceGroup::panelBackground, 317);
    mapPictureGood[int(G_VEGETABLE)] = &Picture::load( ResourceGroup::panelBackground, 318);
    mapPictureGood[int(G_FRUIT)    ] = &Picture::load( ResourceGroup::panelBackground, 319);
    mapPictureGood[int(G_OLIVE)    ] = &Picture::load( ResourceGroup::panelBackground, 320);
    mapPictureGood[int(G_GRAPE)    ] = &Picture::load( ResourceGroup::panelBackground, 321);
    mapPictureGood[int(G_MEAT)     ] = &Picture::load( ResourceGroup::panelBackground, 322);
    mapPictureGood[int(G_WINE)     ] = &Picture::load( ResourceGroup::panelBackground, 323);
    mapPictureGood[int(G_OIL)      ] = &Picture::load( ResourceGroup::panelBackground, 324);
    mapPictureGood[int(G_IRON)     ] = &Picture::load( ResourceGroup::panelBackground, 325);
    mapPictureGood[int(G_TIMBER)   ] = &Picture::load( ResourceGroup::panelBackground, 326);
    mapPictureGood[int(G_CLAY)     ] = &Picture::load( ResourceGroup::panelBackground, 327);
    mapPictureGood[int(G_MARBLE)   ] = &Picture::load( ResourceGroup::panelBackground, 328);
    mapPictureGood[int(G_WEAPON)   ] = &Picture::load( ResourceGroup::panelBackground, 329);
    mapPictureGood[int(G_FURNITURE)] = &Picture::load( ResourceGroup::panelBackground, 330);
    mapPictureGood[int(G_POTTERY)  ] = &Picture::load( ResourceGroup::panelBackground, 331);
    mapPictureGood[int(G_FISH)     ] = &Picture::load( ResourceGroup::panelBackground, 333);
  }
};

class GuiInfoBox::Impl
{
public:
  Picture *bgPicture;
  Label* lbTitle;
  PushButton* btnExit;
  PushButton* btnHelp;
  bool isAutoPosition;
};

GuiInfoBox::GuiInfoBox( Widget* parent, const Rect& rect, int id )
: Widget( parent, id, rect ), _d( new Impl )
{
  // create the title
  _d->lbTitle = new Label( this, Rect( 16, 10, getWidth()-16, 10 + 30 ), "", true );
  _d->lbTitle->setFont( Font( FONT_3 ) );
  _d->lbTitle->setTextAlignment( alignCenter, alignCenter );
  _d->isAutoPosition = true;

  _d->btnExit = new PushButton( this, Rect( 472, getHeight() - 39, 496, getHeight() - 15 ) );
  GuiPaneling::configureTexturedButton( _d->btnExit, ResourceGroup::panelBackground, ResourceMenu::exitInfBtnPicId, false);
  _d->btnHelp = new PushButton( this, Rect( 14, getHeight() - 39, 38, getHeight() - 15 ) );
  GuiPaneling::configureTexturedButton( _d->btnHelp, ResourceGroup::panelBackground, ResourceMenu::helpInfBtnPicId, false);

  CONNECT( _d->btnExit, onClicked(), this, InfoBoxLand::deleteLater );

  _d->bgPicture = &GfxEngine::instance().createPicture( getWidth(), getHeight() );

  // draws the box and the inner black box
  GuiPaneling::instance().draw_white_frame(*_d->bgPicture, 0, 0, getWidth(), getHeight() );
  GfxEngine::instance().loadPicture(*_d->bgPicture);
}

GuiInfoBox::~GuiInfoBox()
{
  
}

void GuiInfoBox::draw( GfxEngine& engine )
{
  engine.drawPicture( getBgPicture(), getScreenLeft(), getScreenTop() );
  Widget::draw( engine );
}

Picture& GuiInfoBox::getBgPicture()
{
  return *_d->bgPicture;
}

bool GuiInfoBox::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return getParent()->getAbsoluteRect().isPointInside( point );
}

bool GuiInfoBox::onEvent( const NEvent& event)
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
  }

  return Widget::onEvent( event );
}

Picture& GuiInfoBox::getPictureGood(const GoodType& goodType)
{
  Picture* res = InfoBoxHelper::getInstance().mapPictureGood[int(goodType)];
  if (res == NULL)
  {
    THROW("No picture for good type:" << goodType);
  }
  return *res;
}

void GuiInfoBox::setTitle( const std::string& title )
{
  _d->lbTitle->setText( title );
}

bool GuiInfoBox::isAutoPosition() const
{
  return _d->isAutoPosition;
}

void GuiInfoBox::setAutoPosition( bool value )
{
  _d->isAutoPosition = value;
}

class GuiInfoService::Impl
{
public:
  Label* dmgLabel;
  Label* lbHelp;
  ServiceBuildingPtr building;
};

GuiInfoService::GuiInfoService( Widget* parent, ServiceBuildingPtr building)
    : GuiInfoBox( parent, Rect( 0, 0, 510, 256 ), -1 ), _sd( new Impl )
{
  _sd->building = building;
  setTitle( BuildingDataHolder::instance().getData( building->getType() ).getPrettyName() );
  paint(); 
}

void GuiInfoService::paint()
{
  GuiPaneling::instance().draw_black_frame( *_d->bgPicture, 16, 136, getWidth() - 32, 62 );
  
  if( _sd->building->getMaxWorkers() > 0 )
  {
    drawWorkers( 150 );
  }

  _sd->dmgLabel = new Label( this, Rect( 50, getHeight() - 50, getWidth() - 50, getHeight() - 16 ) ); 
  std::string text = StringHelper::format( 0xff, "%d%% damage - %d%% fire", 
                                           (int)_sd->building->getDamageLevel(), (int)_sd->building->getFireLevel());
  _sd->dmgLabel->setText( text );

  _sd->lbHelp = new Label( this, Rect( 16, 50, getWidth() - 16, 130 ) );
}

void GuiInfoService::setText(const std::string& text)
{
  _sd->lbHelp->setText( text );
}

void GuiInfoService::drawWorkers( int paintY )
{
  // picture of citizen
  Picture& pic = Picture::load( ResourceGroup::panelBackground, 542);
  _d->bgPicture->draw( pic, 16+15, paintY);

  // number of workers
  std::string text = StringHelper::format( 0xff, _("%d employers (%d requred)"), 
                                            _sd->building->getWorkers(), _sd->building->getMaxWorkers() );

  Font *font = new Font( FONT_2 );
  font->draw( *_d->bgPicture, text, 16+42, paintY+5 );
}


class InfoBoxHouse::Impl
{
public:
  HousePtr house;
  Label* lbHabitants;
  Label* lbCrime;
};

InfoBoxHouse::InfoBoxHouse( Widget* parent, const Tile& tile )
    : GuiInfoBox( parent, Rect( 0, 0, 510, 360 ), -1 ),
      _ed( new Impl )
{
  _ed->house = tile.get_terrain().getOverlay().as<House>();
  setTitle( _ed->house->getName() );
  _paint();
}


void InfoBoxHouse::_paint()
{
  int lbHeight = 20;
  GuiPaneling::instance().draw_black_frame( *_d->bgPicture, 16, 150, 
                                               _d->btnExit->getRight() - _d->btnHelp->getLeft(), 
                                               _d->btnExit->getTop() - 150 - 5 );

  drawHabitants();
    
  int taxes = -1; // _house->getMonthlyTaxes();
  Label* taxesLb = new Label( this, Rect( 16 + 15, _ed->lbHabitants->getBottom(), getWidth() - 16, 
                                            _ed->lbHabitants->getBottom() + lbHeight ), "", false, true );
  char buffer[200];
  if (taxes == -1)
  {
    sprintf(buffer, _("Aucun percepteur ne passe ici. Ne paye pas de taxes"));
    taxesLb->setFont( Font( FONT_2_RED ) );
  }
  else
  {
    sprintf(buffer, _("Paye %d Denarii de taxes par mois"), taxes);
  }

  taxesLb->setText( buffer );
  //_paintY+=22;
  
  _ed->lbCrime = new Label( this, taxesLb->getRelativeRect() + Point( 0, 22 ), "", false, true );
  sprintf(buffer, _("Inhabitants didn't report about crimes"));
  _ed->lbCrime->setText( buffer );

  int startY = _ed->lbCrime->getBottom() + 10;
  if( _ed->house->getLevelSpec().getHouseLevel() > 2 )
  {
    drawGood(G_WHEAT, 0, 0, startY );
  }
  else
  {
    Label* lb = new Label( this, _ed->lbCrime->getRelativeRect() + Point( 0, 30 ), "", false, true ); 
    lb->setHeight( 40 );
    lb->setLineIntervalOffset( -6 );
    lb->setText( _("Inabitants of tents provide food themselves, conducting a subsistence economy") );
    lb->setWordWrap( true );
    startY = lb->getTop();
  }

  drawGood(G_POTTERY, 0, 1, startY);
  drawGood(G_FURNITURE, 1, 1, startY);
  drawGood(G_OIL, 2, 1, startY);
  drawGood(G_WINE, 3, 1, startY);
}


void InfoBoxHouse::drawHabitants()
{
   // citizen or patrician picture
   Uint32 picId = _ed->house->getLevelSpec().isPatrician() ? 541 : 542; 
   
   Picture& citPic = Picture::load( ResourceGroup::panelBackground, picId );
   _d->bgPicture->draw( citPic, 16+15, 157 );

   // number of habitants
   _ed->lbHabitants = new Label( this, Rect( 60, 157, getWidth() - 16, 157 + citPic.getHeight() ), "", false, true );
   char buffer[200];
   int freeRoom = _ed->house->getMaxHabitants() - _ed->house->getNbHabitants();
   if( freeRoom > 0 )
   {
      // there is some room for new habitants!
      sprintf(buffer, _("%d citizens, additional rooms for %d"), _ed->house->getNbHabitants(), freeRoom);
   }
   else if (freeRoom == 0)
   {
      // full house!
      sprintf(buffer, _("%d citizens"), _ed->house->getNbHabitants());
   }
   else if (freeRoom < 0)
   {
      // too many habitants!
      sprintf(buffer, _("%d citizens, %d habitants en trop"), _ed->house->getNbHabitants(), -freeRoom);
      _ed->lbHabitants->setFont( Font( FONT_2_RED ) );
   }

   _ed->lbHabitants->setText( buffer );
}

void InfoBoxHouse::drawGood(const GoodType &goodType, const int col, const int row, const int startY )
{
  Font font( FONT_2 );
  int qty = _ed->house->getGoodStore().getCurrentQty(goodType);

  // pictures of goods
  Picture &pic = getPictureGood(goodType);
  _d->bgPicture->draw(pic, 31 + 100 * col, startY + 2 + 30 * row);

  std::string text = StringHelper::format( 0xff, "%d", qty);
  font.draw( *_d->bgPicture, text, 61 + 100 * col, startY + 30 * row );
}

GuiInfoFactory::GuiInfoFactory( Widget* parent, Factory &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 220 ), -1 )
{
  _building = &building;
  setTitle( BuildingDataHolder::instance().getData(building.getType()).getPrettyName() );
  paint();
}


void GuiInfoFactory::paint()
{
   // paint picture of out good
   //Font &font_red = FontCollection::instance().getFont(FONT_2_RED);
   Font font( FONT_2 );

   // paint progress
   int progress = _building->getProgress();
   int _paintY = _d->lbTitle->getBottom();

   std::string text = StringHelper::format( 0xff, _("Le travail est a %d%% termine."), progress );
   font.draw(*_d->bgPicture, text, 15, _paintY);
   _paintY+=22;

   // paint picture of in good
   if (_building->getInGood()._goodType != G_NONE)
   {
      Picture &pic = getPictureGood(_building->getInGood()._goodType);
      _d->bgPicture->draw(pic, 15, _paintY+2);
      int amount = _building->getInGood()._currentQty / 100;
      std::string goodName = GoodHelper::getInstance().getName( _building->getInGood()._goodType );
    
      text = StringHelper::format( 0xff, _("%s en stock: %d unites"), goodName.c_str(), amount );
      
      font.draw( *_d->bgPicture, text, 42, _paintY);
      _paintY+=22;
   }

   /*std::string desc = getInfoText();
   std::list<std::string> text_lines = font.split_text(desc, getWidth()-32);
   for (std::list<std::string>::iterator itlines = text_lines.begin(); itlines != text_lines.end(); ++itlines)
   {
      std::string &line = *itlines;
      font.draw(*_d->bgPicture, line, 15, _paintY );
      _paintY+=19;
   }*/

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, _paintY, getWidth()-32, getHeight()-_paintY-16);
   _paintY+=10;

   drawWorkers( _paintY );
}


void GuiInfoFactory::drawWorkers( int& paintY )
{
   // picture of citizen
   Picture *pic = &Picture::load( ResourceGroup::panelBackground, 542);
   _d->bgPicture->draw( *pic, 16+15, paintY);

   // number of workers
   std::string text = StringHelper::format( 0xff, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font font( FONT_2 );
   font.draw(*_d->bgPicture, text, 16+42, paintY+5 );
   paintY += 20;
}


std::string GuiInfoFactory::getInfoText()
{
   std::string textKey = GoodHelper::getInstance().getName( _building->getOutGood()._goodType );
   if (_building->isActive() == false)
   {
      textKey+= "- Production arretee sous ordre du gouverneur";
   }
   else if (_building->getInGood()._goodType != G_NONE && _building->getInGood()._currentQty == 0)
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

   return _(textKey.c_str());
}

class GuiInfoGranary::Impl
{
public:
   GranaryPtr building;
};

GuiInfoGranary::GuiInfoGranary( Widget* parent, const Tile& tile )
    : GuiInfoBox( parent, Rect( 0, 0, 450, 300 ), -1 ), _gd( new Impl )
{
   _gd->building = tile.get_terrain().getOverlay().as<Granary>();

   setTitle( BuildingDataHolder::instance().getData( _gd->building->getType()).getPrettyName() );

   int height = 160;

   for (int n = 0; n < G_MAX; ++n)
   {
      int qty = _gd->building->getGoodStore().getCurrentQty((GoodType) n);
      if (qty != 0)
      {
         height += 22;
      }
   }

   setHeight( height );
   paint();
}


void GuiInfoGranary::paint()
{
   //Font &font_red = FontCollection::instance().getFont(FONT_2_RED);
   Font font( FONT_2 );

   // summary: total stock, free capacity
   int _paintY = _d->lbTitle->getBottom();
   int currentQty = _gd->building->getGoodStore().getCurrentQty();
   int maxQty = _gd->building->getGoodStore().getMaxQty();
   char buffer[1000];
   sprintf(buffer, _("%d unites en stock. Espace pour %d unites."), currentQty, maxQty-currentQty);

   font.draw( *_d->bgPicture, std::string(buffer), 16, _paintY+5 );
   _paintY+=40;

   drawGood(G_WHEAT, _paintY);
   drawGood(G_FISH, _paintY);
   drawGood(G_MEAT, _paintY);
   drawGood(G_FRUIT, _paintY);
   drawGood(G_VEGETABLE, _paintY);

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, _paintY, getWidth()-32, getHeight()-_paintY-16);
   _paintY+=10;

   drawWorkers( _paintY );
}


void GuiInfoGranary::drawWorkers( int paintY )
{
   // picture of citizen
   Picture& pic = Picture::load( ResourceGroup::panelBackground, 542);
   _d->bgPicture->draw(pic, 16+15, paintY);

   // number of workers
   std::string text = StringHelper::format( 0xff, _("%d employers (%d requires)"), _gd->building->getWorkers(), _gd->building->getMaxWorkers());

   Font font( FONT_2 );
   font.draw(*_d->bgPicture, text, 16+42, paintY+5 );
   paintY+=20;
}


void GuiInfoGranary::drawGood(const GoodType &goodType, int& paintY)
{
  std::string goodName = GoodHelper::getInstance().getName( goodType );

  Font font( FONT_2 );
  int qty = _gd->building->getGoodStore().getCurrentQty(goodType);

  // pictures of goods
  Picture &pic = getPictureGood(goodType);
  _d->bgPicture->draw(pic, 31, paintY);

  std::string outText = StringHelper::format( 0xff, "%d %s", qty, goodName.c_str() );
  font.draw( *_d->bgPicture, outText, 61, paintY );
  paintY += 22;
}

class InfoBoxTemple::Impl
{
public:
  TemplePtr temple;
  Font font;
};

InfoBoxTemple::InfoBoxTemple( Widget* parent, const Tile& tile )
  : GuiInfoBox( parent, Rect( 0, 0, 510, 256 ), -1 ), _td( new Impl )
{
  _td->font = Font( FONT_2 );
  _td->temple = tile.get_terrain().getOverlay().as<Temple>();
  RomeDivinityPtr divn = _td->temple->getDivinity();

  std::string text = StringHelper::format( 0xff, "##Temple of ##%s (%s)", 
                                                 divn->getName().c_str(), 
                                                 divn->getShortDescription().c_str() );
  setTitle( text );

  drawWorkers();
  drawPicture();
}

void InfoBoxTemple::drawWorkers()
{
  int y = 56;
  GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, y, getWidth() - 32, 62 );
  // picture of citizen
  Picture& pic = Picture::load( ResourceGroup::panelBackground, 542 );
  _d->bgPicture->draw( pic, 16+15, y + 12);

  // number of workers
  std::string text = StringHelper::format( 0xff, _("%d employers (%d requires)"), 
                                                 _td->temple->getWorkers(), 
                                                 _td->temple->getMaxWorkers());

  _td->font.draw( *_d->bgPicture, text, 16 + 15 + pic.getWidth() + 5, y+18 );
}

void InfoBoxTemple::drawPicture()
{
  const Picture& pic = _td->temple->getDivinity()->getPicture();
  _d->bgPicture->draw( pic, 192, 140 );
}

class GuiInfoMarket::Impl
{
public:
  MarketPtr market;
  Font goodFont;
  Label* lbAbout;

};

GuiInfoMarket::GuiInfoMarket( Widget* parent, const Tile& tile )
    : GuiInfoBox( parent, Rect( 0, 0, 510, 256 ), -1 ), _md( new Impl )
{
   _md->market = tile.get_terrain().getOverlay().as<Market>();
   _md->goodFont = Font( FONT_2 );
   _md->lbAbout = new Label( this, _d->lbTitle->getRelativeRect() + Point( 0, 30 ) );

   setTitle( "##market_title##" );
   paint();
}


void GuiInfoMarket::paint()
{
  if( _md->market->getWorkers() > 0 )
  {
    int paintY = 78;
    drawGood(G_WHEAT, 0, paintY );
    drawGood(G_FISH, 1, paintY);
    drawGood(G_MEAT, 2, paintY);
    drawGood(G_FRUIT, 3, paintY);
    drawGood(G_VEGETABLE, 4, paintY);

    paintY += 24;
    drawGood(G_POTTERY, 0, paintY);
    drawGood(G_FURNITURE, 1, paintY);
    drawGood(G_OIL, 2, paintY);
    drawGood(G_WINE, 3, paintY); 

    SimpleGoodStore& goods = _md->market->getGoodStore();
    int furageSum = 0;
    // for all furage types of good
    for (int goodType = 0; goodType<G_OLIVE; ++goodType)
    {
      furageSum += goods.getCurrentQty( (GoodType)goodType );     
    }

    _md->lbAbout->setText( 0 == furageSum ? "##market_search_food_source##" : "##market_about##");
  }
  else
  {
    _md->lbAbout->setText( "##market_not_work##" );
  }

  drawWorkers();
}

void GuiInfoMarket::drawWorkers()
{
  int y = 136;
  GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, y, getWidth() - 32, 62 );
  // picture of citizen
  Picture& pic = Picture::load( ResourceGroup::panelBackground, 542);
  _d->bgPicture->draw( pic, 16+15, y + 12);

  // number of workers
  std::string text = StringHelper::format( 0xff, _("%d employers (%d requires)"), 
                                                   _md->market->getWorkers(), 
                                                   _md->market->getMaxWorkers());

  _md->goodFont.draw(*_d->bgPicture, text, 16+pic.getWidth() + 5, y+18 );
}


void GuiInfoMarket::drawGood(const GoodType &goodType, int index, int paintY )
{
  int startOffset = 25;

  int offset = ( getWidth() - startOffset * 2 ) / 5;
  std::string goodName = GoodHelper::getInstance().getName( goodType );

  // pictures of goods
  Picture &pic = getPictureGood(goodType);
  Point pos( index * offset + startOffset, paintY );
  _d->bgPicture->draw( pic, pos.getX(), pos.getY() );

  std::string outText = StringHelper::format( 0xff, "%d", _md->market->getGoodStore().getCurrentQty(goodType) );
  _md->goodFont.draw(*_d->bgPicture, outText, pos.getX() + pic.getWidth() + 5, pos.getY() );
}

class GuiBuilding::Impl
{
public:
  BuildingPtr building;
};

GuiBuilding::GuiBuilding( Widget* parent, const Tile& tile )
    : GuiInfoBox( parent, Rect( 0, 0, 450, 220 ), -1 ), _bd( new Impl )
{
  _bd->building = tile.get_terrain().getOverlay().as<Building>();
  setTitle( BuildingDataHolder::instance().getData( _bd->building->getType()).getPrettyName() );

  paint();
}

void GuiBuilding::paint()
{
   int paintY = _d->lbTitle->getBottom() + 10;
   GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, paintY, getWidth()-32, getHeight()-paintY-16);
   paintY+=10;  
}

InfoBoxLand::InfoBoxLand( Widget* parent, const Tile& tile )
    : GuiInfoBox( parent, Rect( 0, 0, 510, 350 ), -1 )
{
  _text = new Label( this, Rect( 38, 239, 470, 338 ), "", true );
  _text->setFont( Font( FONT_2 ) );

  if( tile.get_terrain().isTree() )
  {
    setTitle( _("##trees_and_forest_caption") );
    _text->setText( _("##trees_and_forest_text"));
  } 
  else if( tile.get_terrain().isWater() )
  {
    setTitle( _("##water_caption") );
    _text->setText( _("##water_text"));
  }
  else if( tile.get_terrain().isRock() )
  {
    setTitle( _("##rock_caption") );
    _text->setText( _("##rock_text"));
  }
  else if( tile.get_terrain().isRoad() )
  {
    setTitle( _("##road_caption") );
    _text->setText( _("##road_text"));
  }
  else 
  {
    setTitle( _("##clear_land_caption") );
    _text->setText( _("##clear_land_text"));
  }
 
  City& oCity = Scenario::instance().getCity();
  Tilemap& oTilemap = Scenario::instance().getCity().getTilemap();
  int size = oTilemap.getSize();
  int border_size = (162 - size) / 2;
  
  int index = (size - tile.getJ() - 1 + border_size) * 162 + tile.getI() + border_size;
  
  const TerrainTile& terrain = tile.get_terrain();

  std::string text = StringHelper::format( 0xff, "Tile at: (%d,%d) %04X %02X %04X %02X %02X %02X",
                                           tile.getI(), tile.getJ(),  
                                          ((short int) terrain.getOriginalImgId() ),
                                          ((short int) terrain.getEdgeData()),
                                          ((short int) terrain.getTerrainData()),  
                                          ((short int) terrain.getTerrainRndmData()),  
                                          ((short int) terrain.getRandomData()),
                                          ((short int) terrain.getElevationData() ) );
  
  _text->setText( text );
  
  GuiPaneling::instance().draw_black_frame( *_d->bgPicture, 16, _d->lbTitle->getBottom() + 10, getWidth()-32, 180 );
}

void InfoBoxLand::setText( const std::string& text )
{
    _text->setText( text );
}

InfoBoxFreeHouse::InfoBoxFreeHouse( Widget* parent, const Tile& tile )
    : InfoBoxLand( parent, tile )
{
    setTitle( _("##freehouse_caption") );
    setText( _("##freehouse_text") );
}   

class InfoBoxFarm::Impl
{
public:
  FarmPtr farm;
  Label* lbProgress;
  Label* lbAbout;
  Label* lbDesc;
  Label* dmgLabel;

  void updateAboutText();
};

void InfoBoxFarm::Impl::updateAboutText()
{
  std::string text = "##farm_working_normally##";
  if( farm->getWorkers() == 0 )
  {
    text = "##farm_have_no_workers##";
  }
  else if( farm->getWorkers() <= farm->getMaxWorkers() / 2 )
  {
    text = "##farm_working_bad##";
  }

  lbAbout->setText( text );
}

InfoBoxFarm::InfoBoxFarm( Widget* parent, const Tile& tile )
: GuiInfoBox( parent, Rect( 0, 0, 510, 350 ), -1 ), _fd( new Impl )
{
  _fd->farm = tile.get_terrain().getOverlay().as<Farm>();
  
  setTitle( BuildingDataHolder::instance().getData( _fd->farm->getType() ).getPrettyName() );
  GuiPaneling::instance().draw_black_frame( *_d->bgPicture, 16, 146, getWidth() - 32, 64 );

  // picture of citizen
  Picture& pic = Picture::load( ResourceGroup::panelBackground, 542);
  _d->bgPicture->draw( pic, 16+15, 158 );

  // number of workers
  std::string text = StringHelper::format( 0xff, _("%d employers (%d required)"), 
                                           _fd->farm->getWorkers(), _fd->farm->getMaxWorkers() );

  Font font( FONT_2 );
  font.draw( *_d->bgPicture, text, 16+42, 156+5 );

  _fd->dmgLabel = new Label( this, Rect( 50, getHeight() - 50, getWidth() - 50, getHeight() - 16 ) ); 
  text = StringHelper::format( 0xff, "%d%% damage - %d%% fire", 
  (int)_fd->farm->getDamageLevel(), (int)_fd->farm->getFireLevel());
  _fd->dmgLabel->setText( text );

  text = StringHelper::format( 0xff, "##farm_progress## %d%%", _fd->farm->getProgress() );
  _fd->lbProgress = new Label( this, Rect( 32, 50, getWidth() - 16, 50 + 32 ), text );
  _fd->lbAbout = new Label( this, Rect( 32, _fd->lbProgress->getBottom() + 6, getWidth() - 16, 130 ) );

  _fd->lbDesc = new Label( this, Rect( 32, 236, getWidth() - 50, getHeight() - 50 ), "##farm_description##" );

  _fd->updateAboutText();
}