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


std::vector<Picture*> GuiInfoBox::_mapPictureGood;

GuiInfoBox::GuiInfoBox()
{
   _title = "";
   _bgPicture = NULL;
   _isDeleted = false;

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
   _mapPictureGood[int(G_WHEAT)]     = &PicLoader::instance().get_picture("paneling", 317);
   _mapPictureGood[int(G_VEGETABLE)] = &PicLoader::instance().get_picture("paneling", 318);
   _mapPictureGood[int(G_FRUIT)]     = &PicLoader::instance().get_picture("paneling", 319);
   _mapPictureGood[int(G_OLIVE)]     = &PicLoader::instance().get_picture("paneling", 320);
   _mapPictureGood[int(G_GRAPE)]     = &PicLoader::instance().get_picture("paneling", 321);
   _mapPictureGood[int(G_MEAT)]      = &PicLoader::instance().get_picture("paneling", 322);
   _mapPictureGood[int(G_WINE)]      = &PicLoader::instance().get_picture("paneling", 323);
   _mapPictureGood[int(G_OIL)]       = &PicLoader::instance().get_picture("paneling", 324);
   _mapPictureGood[int(G_IRON)]      = &PicLoader::instance().get_picture("paneling", 325);
   _mapPictureGood[int(G_TIMBER)]    = &PicLoader::instance().get_picture("paneling", 326);
   _mapPictureGood[int(G_CLAY)]      = &PicLoader::instance().get_picture("paneling", 327);
   _mapPictureGood[int(G_MARBLE)]    = &PicLoader::instance().get_picture("paneling", 328);
   _mapPictureGood[int(G_WEAPON)]    = &PicLoader::instance().get_picture("paneling", 329);
   _mapPictureGood[int(G_FURNITURE)] = &PicLoader::instance().get_picture("paneling", 330);
   _mapPictureGood[int(G_POTTERY)]   = &PicLoader::instance().get_picture("paneling", 331);
   _mapPictureGood[int(G_FISH)]      = &PicLoader::instance().get_picture("paneling", 333);
}

void GuiInfoBox::init(const int width, const int height)
{
   setSize(width, height);

   SdlFacade &sdlFacade = SdlFacade::instance();

   _bgPicture = &sdlFacade.createPicture(width, height);

   // draws the box and the inner black box
   GuiPaneling::instance().draw_white_frame(*_bgPicture, 0, 0, width, height);

   // draws the title
   _paintY = 4;
   Font &font = FontCollection::instance().getFont(FONT_3);
   int text_width = 0;
   int text_height = 0;
   sdlFacade.getTextSize(font, _title, text_width, text_height);
   sdlFacade.drawText(*_bgPicture, _title, (width-text_width)/2, _paintY, font);
   _paintY += 34;

   // draws the help button
   _hoverButton = NULL;
   _helpButton.setEvent(WidgetEvent());
   _helpButton.setNormalPicture(PicLoader::instance().get_picture("paneling", 529));
   _helpButton.setHoverPicture(PicLoader::instance().get_picture("paneling", 529+1));
   _helpButton.setSelectedPicture(PicLoader::instance().get_picture("paneling", 529+2));
   _helpButton.setPosition(width - 40, 13);
   add_widget(_helpButton);

   // custom paint
   paint();

   GfxEngine::instance().load_picture(*_bgPicture);
}


void GuiInfoBox::draw(const int dx, const int dy)
{
   drawPicture(getBgPicture(), dx, dy);
   drawChildren(dx, dy);
}

Picture& GuiInfoBox::getBgPicture()
{
   return *_bgPicture;
}


void GuiInfoBox::handleEvent(SDL_Event &event)
{
   if (event.type == SDL_MOUSEMOTION)
   {
      // mouse move: hover the right widget

      // get button under the cursor
      Widget *hoverWidget = get_widget_at(event.motion.x - _x, event.motion.y - _y);
      ImageButton *hoverButton = dynamic_cast<ImageButton*>(hoverWidget);

      if (_hoverButton != NULL && _hoverButton != hoverButton)
      {
         // mouse quits hover button => unhover
         _hoverButton->setState(B_NORMAL);
         _hoverButton = NULL;
      }
      if (hoverButton != NULL && hoverButton->getState() == B_NORMAL)
      {
         // mouse enters a normal button => hover
         hoverButton->setState(B_HOVER);
         _hoverButton = hoverButton;
      }
   }
   if (event.type == SDL_MOUSEBUTTONDOWN)
   {
      int button = event.button.button;
      // int x = event.button.x;
      // int y = event.button.y;
      if (button == SDL_BUTTON_RIGHT)
      {
         _isDeleted = true;
      }
   }
   if (event.type == SDL_KEYDOWN)
   {
      if (event.key.keysym.sym == SDLK_ESCAPE)
      {
         _isDeleted = true;
      }
   }
}


bool GuiInfoBox::isDeleted() const
{
   return _isDeleted;
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


GuiInfoService::GuiInfoService(ServiceBuilding &building)
{
   _building = &building;
   _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();
   init(450, 300);
}


void GuiInfoService::paint()
{
   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, _width-32, _height-_paintY-16);
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


GuiInfoHouse::GuiInfoHouse(House &house)
{
   _house = &house;
   _title = house.getName();
   init(450, 300);
}


void GuiInfoHouse::paint()
{
   drawTaxes();
   drawCrime();

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, _width-32, _height-_paintY-16);
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
   Picture *patrician = &PicLoader::instance().get_picture("paneling", 541);
   Picture *citizen = &PicLoader::instance().get_picture("paneling", 542);
   Picture *pic = citizen;
   if (_house->getLevelSpec().isPatrician())
   {
      // oops it was a patrician, sorry sir!
      pic = patrician;
   }
   sdlFacade.drawPicture(*pic, *_bgPicture, 16+15, _paintY);

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


GuiInfoFactory::GuiInfoFactory(Factory &building)
{
   _building = &building;
   _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();
   init(450, 220);
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
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, _width-32, _height-_paintY-16);
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


GuiInfoGranary::GuiInfoGranary(Granary &building)
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

   init(450, height);
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
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, _width-32, _height-_paintY-16);
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


GuiInfoMarket::GuiInfoMarket(Market &building)
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

   init(450, height);
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
   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, _width-32, _height-_paintY-16);
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

GuiBuilding::GuiBuilding(Building &building)
{
  _building = &building;
  _title = BuildingDataHolder::instance().getData(building.getType()).getPrettyName();
  init(450, 220);
}

void GuiBuilding::paint()
{
//   _paintY+=10;
//   GuiPaneling::instance().draw_black_frame(*_bgPicture, 16, _paintY, _width-32, _height-_paintY-16);
//   _paintY+=10;  
}
