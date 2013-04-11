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



#include "gui_info_box.hpp"

#include <SDL.h>
#include <iostream>

#include "exception.hpp"
#include "pic_loader.hpp"
#include "sdl_facade.hpp"
#include "gettext.hpp"
#include "gui_paneling.hpp"
#include "building_data.hpp"
#include "house_level.hpp"
#include "oc3_resourcegroup.h"
#include "oc3_event.h"
#include "oc3_pushbutton.h"
#include "oc3_label.h"

std::vector<Picture*> GuiInfoBox::_mapPictureGood; 

GuiInfoBox::GuiInfoBox( Widget* parent, const Rect& rect, int id )
: Widget( parent, id, rect )
{
   _title = "";
   _bgPicture = 0;

   if (_mapPictureGood.empty())
   {
      initStatic();
   }
}

GuiInfoBox::~GuiInfoBox()
{
  
}


void GuiInfoBox::initStatic()
{
   _mapPictureGood.resize(G_MAX);
   PicLoader& ldr = PicLoader::instance();
   _mapPictureGood[int(G_WHEAT)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 317);
   _mapPictureGood[int(G_VEGETABLE)] = &ldr.get_picture( ResourceGroup::panelBackground, 318);
   _mapPictureGood[int(G_FRUIT)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 319);
   _mapPictureGood[int(G_OLIVE)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 320);
   _mapPictureGood[int(G_GRAPE)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 321);
   _mapPictureGood[int(G_MEAT)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 322);
   _mapPictureGood[int(G_WINE)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 323);
   _mapPictureGood[int(G_OIL)      ] = &ldr.get_picture( ResourceGroup::panelBackground, 324);
   _mapPictureGood[int(G_IRON)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 325);
   _mapPictureGood[int(G_TIMBER)   ] = &ldr.get_picture( ResourceGroup::panelBackground, 326);
   _mapPictureGood[int(G_CLAY)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 327);
   _mapPictureGood[int(G_MARBLE)   ] = &ldr.get_picture( ResourceGroup::panelBackground, 328);
   _mapPictureGood[int(G_WEAPON)   ] = &ldr.get_picture( ResourceGroup::panelBackground, 329);
   _mapPictureGood[int(G_FURNITURE)] = &ldr.get_picture( ResourceGroup::panelBackground, 330);
   _mapPictureGood[int(G_POTTERY)  ] = &ldr.get_picture( ResourceGroup::panelBackground, 331);
   _mapPictureGood[int(G_FISH)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 333);
}

void GuiInfoBox::_init()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   _bgPicture = &sdlFacade.createPicture( getWidth(), getHeight() );

   // draws the box and the inner black box
   GuiPaneling::instance().draw_white_frame(*_bgPicture, 0, 0, getWidth(), getHeight() );

   // draws the title
   _paintY = 4;
   Font &font = FontCollection::instance().getFont(FONT_3);
   int text_width = 0;
   int text_height = 0;
   sdlFacade.getTextSize(font, _title, text_width, text_height);
   sdlFacade.drawText(*_bgPicture, _title, (getWidth()-text_width)/2, _paintY, font);
   _paintY += 34;

   // draws the help button
   _hoverButton = NULL;
   //_helpButton.setEvent(WidgetEvent());
   /*_helpButton.setPicture(PicLoader::instance().get_picture("paneling", 529));
   _helpButton.setHoverPicture(PicLoader::instance().get_picture("paneling", 529+1));
   _helpButton.setSelectedPicture(PicLoader::instance().get_picture("paneling", 529+2));
   _helpButton.setPosition(width - 40, 13);
   add_widget(_helpButton);*/

   // custom paint
   paint();

   GfxEngine::instance().load_picture(*_bgPicture);
}


void GuiInfoBox::draw( GfxEngine& engine )
{
   engine.drawPicture( getBgPicture(), getScreenLeft(), getScreenTop() );
   Widget::draw( engine );
}

Picture& GuiInfoBox::getBgPicture()
{
   return *_bgPicture;
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
    break;
    }

    return true;
}

Picture& GuiInfoBox::getPictureGood(const GoodType& goodType)
{
   Picture* res = _mapPictureGood[int(goodType)];
   if (res == NULL)
   {
      THROW("No picture for good type:" << goodType);
   }
   return *res;
}


GuiInfoService::GuiInfoService( Widget* parent, ServiceBuilding &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 300 ), -1 )
{
   _building = &building;
   _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();
   _init();
}


void GuiInfoService::paint()
{
   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, getWidth() - 32, getHeight() -_paintY-16);
   _paintY+=10;

   drawWorkers();

   SdlFacade &sdlFacade = SdlFacade::instance();
   char buffer[1000];
   sprintf(buffer, "%d%% damage - %d%% fire", (int)_building->getDamageLevel(), (int)_building->getFireLevel());
   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+42, _paintY+5, font);

   _paintY+=10;
}


void GuiInfoService::drawWorkers()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_bgPicture, 16+15, _paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+42, _paintY+5, font);
   _paintY+=20;
}


GuiInfoHouse::GuiInfoHouse( Widget* parent, House &house )
    : GuiInfoBox( parent, Rect( 0, 0, 450, 300 ), -1 )
{
   _house = &house;
   _title = house.getName();
   _init();
}


void GuiInfoHouse::paint()
{
   drawTaxes();
   drawCrime();

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, getWidth()-32, getHeight()-_paintY-16);
   _paintY+=10;

   // paint basic info about the house
   drawHabitants();

   // paint goods
   _paintY+=15;
   drawGood(G_WHEAT, 0, 0);
   drawGood(G_POTTERY, 0, 1);
   drawGood(G_FURNITURE, 1, 1);
   drawGood(G_OIL, 2, 1);
   drawGood(G_WINE, 3, 1);
}


void GuiInfoHouse::drawHabitants()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // citizen or patrician picture
   Uint32 picId = _house->getLevelSpec().isPatrician() ? 541 : 542; 
   Picture* pic = &PicLoader::instance().get_picture( ResourceGroup::panelBackground, picId);
   
   sdlFacade.drawPicture( *pic, *_bgPicture, 16+15, _paintY);

   // number of habitants
   char buffer[1000];
   Font *font = &FontCollection::instance().getFont(FONT_2);
   int freeRoom = _house->getMaxHabitants() - _house->getNbHabitants();
   if (freeRoom > 0)
   {
      // there is some room for new habitants!
      sprintf(buffer, _("%d occupants, %d places disponibles"), _house->getNbHabitants(), freeRoom);
   }
   else if (freeRoom == 0)
   {
      // full house!
      sprintf(buffer, _("%d occupants"), _house->getNbHabitants());
   }
   else if (freeRoom < 0)
   {
      // too many habitants!
      sprintf(buffer, _("%d occupants, %d habitants en trop"), _house->getNbHabitants(), -freeRoom);
      font = &FontCollection::instance().getFont(FONT_2_RED);
   }

   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+42, _paintY+5, *font);
   _paintY+=33;
}


void GuiInfoHouse::drawTaxes()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   int taxes = -1; // _house->getMonthlyTaxes();

   char buffer[1000];
   Font *font = &FontCollection::instance().getFont(FONT_2);
   if (taxes == -1)
   {
      sprintf(buffer, _("Aucun percepteur ne passe ici. Ne paye pas de taxes"));
      font = &FontCollection::instance().getFont(FONT_2_RED);
   }
   else
   {
      sprintf(buffer, _("Paye %d Denarii de taxes par mois"), taxes);
   }

   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+15, _paintY, *font);
   _paintY+=22;
}


void GuiInfoHouse::drawCrime()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   char buffer[1000];
   Font *font = &FontCollection::instance().getFont(FONT_2);
   sprintf(buffer, _("Ce quartier est tranquille"));

   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+15, _paintY, *font);
   _paintY+=22;
}


void GuiInfoHouse::drawGood(const GoodType &goodType, const int col, const int row)
{
   SdlFacade &sdlFacade = SdlFacade::instance();
   Font &font = FontCollection::instance().getFont(FONT_2);
   int qty = _house->getGoodStore().getCurrentQty(goodType);

   // pictures of goods
   Picture &pic = getPictureGood(goodType);
   sdlFacade.drawPicture(pic, *_bgPicture, 31+100*col, _paintY+2+32*row);

   char buffer[1000];
   sprintf(buffer, "%d", qty);
   sdlFacade.drawText(*_bgPicture, std::string(buffer), 61+100*col, _paintY+32*row, font);
}


GuiInfoFactory::GuiInfoFactory( Widget* parent, Factory &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 220 ), -1 )
{
   _building = &building;
   _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();
   _init();
}


void GuiInfoFactory::paint()
{
   // paint picture of out good
   SdlFacade &sdlFacade = SdlFacade::instance();
   //Font &font_red = FontCollection::instance().getFont(FONT_2_RED);
   Font &font = FontCollection::instance().getFont(FONT_2);

   // paint progress
   char buffer[1000];
   int progress = _building->getProgress();
   sprintf(buffer, _("Le travail est a %d%% termine."), progress);
   sdlFacade.drawText(*_bgPicture, std::string(buffer), 15, _paintY, font);
   _paintY+=22;

   // paint picture of in good
   if (_building->getInGood()._goodType != G_NONE)
   {
      Picture &pic = getPictureGood(_building->getInGood()._goodType);
      sdlFacade.drawPicture(pic, *_bgPicture, 15, _paintY+2);
      int amount = _building->getInGood()._currentQty / 100;
      std::string goodName = Good::instance(_building->getInGood()._goodType).getName();
      sprintf(buffer, _("%s en stock: %d unites"), goodName.c_str(), amount);
      sdlFacade.drawText(*_bgPicture, std::string(buffer), 42, _paintY, font);
      _paintY+=22;
   }

   std::string desc = getInfoText().c_str();
   std::list<std::string> text_lines = font.split_text(desc, getWidth()-32);
   for (std::list<std::string>::iterator itlines = text_lines.begin(); itlines != text_lines.end(); ++itlines)
   {
      std::string &line = *itlines;
      sdlFacade.drawText(*_bgPicture, line, 15, _paintY, font);
      _paintY+=19;
   }

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, getWidth()-32, getHeight()-_paintY-16);
   _paintY+=10;

   drawWorkers();
}


void GuiInfoFactory::drawWorkers()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_bgPicture, 16+15, _paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+42, _paintY+5, font);
   _paintY+=20;
}


std::string GuiInfoFactory::getInfoText()
{

   std::string textKey = Good::instance(_building->getOutGood()._goodType).getName();
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


GuiInfoGranary::GuiInfoGranary( Widget* parent, Granary &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 300 ), -1 )
{
   _building = &building;
   _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();

   int height = 160;

   for (int n = 0; n < G_MAX; ++n)
   {
      int qty = _building->getGoodStore().getCurrentQty((GoodType) n);
      if (qty != 0)
      {
         height += 22;
      }
   }

   setHeight( height );
   _init();
}


void GuiInfoGranary::paint()
{
   SdlFacade &sdlFacade = SdlFacade::instance();
   //Font &font_red = FontCollection::instance().getFont(FONT_2_RED);
   Font &font = FontCollection::instance().getFont(FONT_2);

   // summary: total stock, free capacity
   int currentQty = _building->getGoodStore().getCurrentQty();
   int maxQty = _building->getGoodStore().getMaxQty();
   char buffer[1000];
   sprintf(buffer, _("%d unites en stock. Espace pour %d unites."), currentQty, maxQty-currentQty);

   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16, _paintY+5, font);
   _paintY+=40;

   drawGood(G_WHEAT);
   drawGood(G_FISH);
   drawGood(G_MEAT);
   drawGood(G_FRUIT);
   drawGood(G_VEGETABLE);

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, getWidth()-32, getHeight()-_paintY-16);
   _paintY+=10;

   drawWorkers();
}


void GuiInfoGranary::drawWorkers()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_bgPicture, 16+15, _paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+42, _paintY+5, font);
   _paintY+=20;
}


void GuiInfoGranary::drawGood(const GoodType &goodType)
{
   Good &good = Good::instance(goodType);
   SdlFacade &sdlFacade = SdlFacade::instance();
   Font &font = FontCollection::instance().getFont(FONT_2);
   int qty = _building->getGoodStore().getCurrentQty(goodType);

   if (qty == 0)
   {
      // no drawing
      return;
   }

   // pictures of goods
   Picture &pic = getPictureGood(goodType);
   sdlFacade.drawPicture(pic, *_bgPicture, 31, _paintY);

   char buffer[1000];
   sprintf(buffer, "%d ", qty);
   sdlFacade.drawText(*_bgPicture, std::string(buffer) + good.getName(), 61, _paintY, font);

   _paintY += 22;
}


GuiInfoMarket::GuiInfoMarket( Widget* parent, Market &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 300 ), -1 )
{
   _building = &building;
   _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();
   int height = 120;
   GoodStore& goodStore = building.getGoodStore();
   for (int n = 0; n<G_MAX; ++n)
   {
      // for all types of good
      int qty = goodStore.getCurrentQty((GoodType) n);
      if (qty != 0)
      {
         height += 22;
      }
   }

   setHeight( height );
   _init();
}


void GuiInfoMarket::paint()
{
   drawGood(G_WHEAT);
   drawGood(G_FISH);
   drawGood(G_MEAT);
   drawGood(G_FRUIT);
   drawGood(G_VEGETABLE);

   drawGood(G_POTTERY);
   drawGood(G_FURNITURE);
   drawGood(G_OIL);
   drawGood(G_WINE);

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, getWidth() - 32, getHeight()-_paintY-16);
   _paintY+=10;

   drawWorkers();
}


void GuiInfoMarket::drawWorkers()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_bgPicture, 16+15, _paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_bgPicture, std::string(buffer), 16+42, _paintY+5, font);
   _paintY+=20;
}


void GuiInfoMarket::drawGood(const GoodType &goodType)
{
   Good &good = Good::instance(goodType);
   SdlFacade &sdlFacade = SdlFacade::instance();
   Font &font = FontCollection::instance().getFont(FONT_2);
   int qty = _building->getGoodStore().getCurrentQty(goodType);

   if (qty == 0)
   {
      // no drawing
      return;
   }

   // pictures of goods
   Picture &pic = getPictureGood(goodType);
   sdlFacade.drawPicture(pic, *_bgPicture, 31, _paintY);

   char buffer[1000];
   sprintf(buffer, "%d ", qty);
   sdlFacade.drawText(*_bgPicture, std::string(buffer) + good.getName(), 61, _paintY, font);

   _paintY += 22;
}

GuiBuilding::GuiBuilding( Widget* parent, Building &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 220 ), -1 )
{
  _building = &building;
  _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();

  _init();
}

void GuiBuilding::paint()
{
   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, getWidth()-32, getHeight()-_paintY-16);
   _paintY+=10;  
}

InfoBoxLand::InfoBoxLand( Widget* parent, Tile* tile )
    : GuiInfoBox( parent, Rect( 0, 0, 510, 350 ), -1 )
{
    Label* _text = new Label( this, Rect( 36, 239, 470, 338 ), "" );
    _text->setFont( FontCollection::instance().getFont(FONT_2) );
    _btnExit = new PushButton( this, Rect( 472, 311, 496, 335 ) );
    GuiPaneling::configureTexturedButton( _btnExit, ResourceGroup::panelBackground, ResourceMenu::exitInfBtnPicId, false);
    _btnHelp = new PushButton( this, Rect( 14, 311, 38, 335 ) );
    GuiPaneling::configureTexturedButton( _btnHelp, ResourceGroup::panelBackground, ResourceMenu::helpInfBtnPicId, false);

    CONNECT( _btnExit, onClicked(), this, InfoBoxLand::deleteLater );

    if( tile->get_terrain().isTree() )
    {
        _title = _("##trees_and_forest_caption");
        _text->setText( _("##trees_and_forest_text"));
    } 
    else if( tile->get_terrain().isWater() )
    {
        _title = _("##water_caption");
        _text->setText( _("##water_text"));
    }
    else if( tile->get_terrain().isRock() )
    {
        _title = _("##rock_caption");
        _text->setText( _("##rock_text"));
    }
    else if( tile->get_terrain().isRoad() )
    {
        _title = _("##road_caption");
        _text->setText( _("##road_text"));
    }
    else 
    {
        _title = _("##clear_land_caption");
        _text->setText( _("##clear_land_text"));
    }

    _init();
}

void InfoBoxLand::paint()
{
    GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, 40, getWidth()-32, 190 );
}