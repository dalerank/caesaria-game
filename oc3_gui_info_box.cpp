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
        PicLoader& ldr = PicLoader::instance();
        mapPictureGood[int(G_WHEAT)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 317);
        mapPictureGood[int(G_VEGETABLE)] = &ldr.get_picture( ResourceGroup::panelBackground, 318);
        mapPictureGood[int(G_FRUIT)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 319);
        mapPictureGood[int(G_OLIVE)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 320);
        mapPictureGood[int(G_GRAPE)    ] = &ldr.get_picture( ResourceGroup::panelBackground, 321);
        mapPictureGood[int(G_MEAT)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 322);
        mapPictureGood[int(G_WINE)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 323);
        mapPictureGood[int(G_OIL)      ] = &ldr.get_picture( ResourceGroup::panelBackground, 324);
        mapPictureGood[int(G_IRON)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 325);
        mapPictureGood[int(G_TIMBER)   ] = &ldr.get_picture( ResourceGroup::panelBackground, 326);
        mapPictureGood[int(G_CLAY)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 327);
        mapPictureGood[int(G_MARBLE)   ] = &ldr.get_picture( ResourceGroup::panelBackground, 328);
        mapPictureGood[int(G_WEAPON)   ] = &ldr.get_picture( ResourceGroup::panelBackground, 329);
        mapPictureGood[int(G_FURNITURE)] = &ldr.get_picture( ResourceGroup::panelBackground, 330);
        mapPictureGood[int(G_POTTERY)  ] = &ldr.get_picture( ResourceGroup::panelBackground, 331);
        mapPictureGood[int(G_FISH)     ] = &ldr.get_picture( ResourceGroup::panelBackground, 333);
    }
};

class GuiInfoBox::Impl
{
public:
    Picture *bgPicture;
    Label* lbTitle;
    PushButton* btnExit;
    PushButton* btnHelp;
};

GuiInfoBox::GuiInfoBox( Widget* parent, const Rect& rect, int id )
: Widget( parent, id, rect ), _d( new Impl )
{
    // create the title
    _d->lbTitle = new Label( this, Rect( 16, 10, getWidth()-16, 10 + 30 ), "", true );
    _d->lbTitle->setFont( FontCollection::instance().getFont(FONT_3) );
    _d->lbTitle->setTextAlignment( alignCenter, alignCenter );

    _d->btnExit = new PushButton( this, Rect( 472, getHeight() - 39, 496, getHeight() - 15 ) );
    GuiPaneling::configureTexturedButton( _d->btnExit, ResourceGroup::panelBackground, ResourceMenu::exitInfBtnPicId, false);
    _d->btnHelp = new PushButton( this, Rect( 14, getHeight() - 39, 38, getHeight() - 15 ) );
    GuiPaneling::configureTexturedButton( _d->btnHelp, ResourceGroup::panelBackground, ResourceMenu::helpInfBtnPicId, false);

    CONNECT( _d->btnExit, onClicked(), this, InfoBoxLand::deleteLater );

    SdlFacade &sdlFacade = SdlFacade::instance();
    _d->bgPicture = &sdlFacade.createPicture( getWidth(), getHeight() );

    // draws the box and the inner black box
    GuiPaneling::instance().draw_white_frame(*_d->bgPicture, 0, 0, getWidth(), getHeight() );
    GfxEngine::instance().load_picture(*_d->bgPicture);
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

GuiInfoService::GuiInfoService( Widget* parent, ServiceBuilding &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 300 ), -1 )
{
   _building = &building;
   setTitle( BuildingDataHolder::instance().getData(building.getType()).getPrettyName() );
   paint(); 
}


void GuiInfoService::paint()
{
   int paintY = _d->lbTitle->getBottom() + 10 ;
   GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, paintY, getWidth() - 32, getHeight() - paintY - 16);
   
   paintY+=10;

   drawWorkers( paintY );

   _dmgLabel = new Label( this, Rect( 16 + 42, paintY, getWidth() - 16 - 42, getHeight() - 16) ); 
   char buffer[100];
   sprintf(buffer, "%d%% damage - %d%% fire", (int)_building->getDamageLevel(), (int)_building->getFireLevel());
   _dmgLabel->setText( buffer );
}


void GuiInfoService::drawWorkers( int& paintY )
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_d->bgPicture, 16+15, paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 16+42, paintY+5, font);
   paintY+=20;
}


class InfoBoxHouse::Impl
{
public:
    House* house;
    Label* lbHabitants;
    Label* lbCrime;
};

InfoBoxHouse::InfoBoxHouse( Widget* parent, House &house )
    : GuiInfoBox( parent, Rect( 0, 0, 510, 360 ), -1 ),
      _ed( new Impl )
{
   _ed->house = &house;
   setTitle( house.getName() );
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
        taxesLb->setFont( FontCollection::instance().getFont(FONT_2_RED) );
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
        lb->setHeight( 30 );
        lb->setLineIntevalOffset( -6 );
        lb->setText( _("Inabitants of tents provide food themselves, conducting a subsistence economy") );
        lb->setWordWrap( true );
        startY = lb->getBottom() + 10;
    }

    drawGood(G_POTTERY, 0, 1, startY);
    drawGood(G_FURNITURE, 1, 1, startY);
    drawGood(G_OIL, 2, 1, startY);
    drawGood(G_WINE, 3, 1, startY);
}


void InfoBoxHouse::drawHabitants()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // citizen or patrician picture
   Uint32 picId = _ed->house->getLevelSpec().isPatrician() ? 541 : 542; 
   
   Picture& citPic = PicLoader::instance().get_picture( ResourceGroup::panelBackground, picId );
   sdlFacade.drawPicture( citPic, *_d->bgPicture, 16+15, 157 );

   // number of habitants
   _ed->lbHabitants = new Label( this, Rect( 60, 157, getWidth() - 16, 157 + citPic.get_height() ), "", false, true );
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
      _ed->lbHabitants->setFont( FontCollection::instance().getFont(FONT_2_RED) );
   }

   _ed->lbHabitants->setText( buffer );
}

void InfoBoxHouse::drawGood(const GoodType &goodType, const int col, const int row, const int startY )
{
   SdlFacade &sdlFacade = SdlFacade::instance();
   Font &font = FontCollection::instance().getFont(FONT_2);
   int qty = _ed->house->getGoodStore().getCurrentQty(goodType);

   // pictures of goods
   Picture &pic = getPictureGood(goodType);
   sdlFacade.drawPicture(pic, *_d->bgPicture, 31+100*col, startY + 2 + 30 * row);

   char buffer[1000];
   sprintf(buffer, "%d", qty);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 61+100*col, startY + 30 * row, font);
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
   SdlFacade &sdlFacade = SdlFacade::instance();
   //Font &font_red = FontCollection::instance().getFont(FONT_2_RED);
   Font &font = FontCollection::instance().getFont(FONT_2);

   // paint progress
   char buffer[1000];
   int progress = _building->getProgress();\
   int _paintY = _d->lbTitle->getBottom();

   sprintf(buffer, _("Le travail est a %d%% termine."), progress);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 15, _paintY, font);
   _paintY+=22;

   // paint picture of in good
   if (_building->getInGood()._goodType != G_NONE)
   {
      Picture &pic = getPictureGood(_building->getInGood()._goodType);
      sdlFacade.drawPicture(pic, *_d->bgPicture, 15, _paintY+2);
      int amount = _building->getInGood()._currentQty / 100;
      std::string goodName = Good::instance(_building->getInGood()._goodType).getName();
      sprintf(buffer, _("%s en stock: %d unites"), goodName.c_str(), amount);
      sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 42, _paintY, font);
      _paintY+=22;
   }

   std::string desc = getInfoText().c_str();
   std::list<std::string> text_lines = font.split_text(desc, getWidth()-32);
   for (std::list<std::string>::iterator itlines = text_lines.begin(); itlines != text_lines.end(); ++itlines)
   {
      std::string &line = *itlines;
      sdlFacade.drawText(*_d->bgPicture, line, 15, _paintY, font);
      _paintY+=19;
   }

   _paintY+=10;
   GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, _paintY, getWidth()-32, getHeight()-_paintY-16);
   _paintY+=10;

   drawWorkers( _paintY );
}


void GuiInfoFactory::drawWorkers( int& paintY )
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_d->bgPicture, 16+15, paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 16+42, paintY+5, font);
   paintY += 20 ;
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
   setTitle( BuildingDataHolder::instance().getData(building.getType()).getPrettyName() );

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
   paint();
}


void GuiInfoGranary::paint()
{
   SdlFacade &sdlFacade = SdlFacade::instance();
   //Font &font_red = FontCollection::instance().getFont(FONT_2_RED);
   Font &font = FontCollection::instance().getFont(FONT_2);

   // summary: total stock, free capacity
   int _paintY = _d->lbTitle->getBottom();
   int currentQty = _building->getGoodStore().getCurrentQty();
   int maxQty = _building->getGoodStore().getMaxQty();
   char buffer[1000];
   sprintf(buffer, _("%d unites en stock. Espace pour %d unites."), currentQty, maxQty-currentQty);

   sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 16, _paintY+5, font);
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
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_d->bgPicture, 16+15, paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 16+42, paintY+5, font);
   paintY+=20;
}


void GuiInfoGranary::drawGood(const GoodType &goodType, int& paintY)
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
   sdlFacade.drawPicture(pic, *_d->bgPicture, 31, paintY);

   char buffer[1000];
   sprintf(buffer, "%d ", qty);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer) + good.getName(), 61, paintY, font);

   paintY += 22;
}


GuiInfoMarket::GuiInfoMarket( Widget* parent, Market &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 300 ), -1 )
{
   _building = &building;
   setTitle( BuildingDataHolder::instance().getData(building.getType()).getPrettyName() );
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
   paint();
}


void GuiInfoMarket::paint()
{
   int paintY = _d->lbTitle->getBottom() + 10;
   drawGood(G_WHEAT, paintY );
   drawGood(G_FISH, paintY);
   drawGood(G_MEAT, paintY);
   drawGood(G_FRUIT, paintY);
   drawGood(G_VEGETABLE, paintY);

   drawGood(G_POTTERY, paintY);
   drawGood(G_FURNITURE, paintY);
   drawGood(G_OIL, paintY);
   drawGood(G_WINE, paintY);

   paintY += 10;
   GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, paintY, getWidth() - 32, getHeight() - paintY-16);
   paintY += 10;

   drawWorkers( paintY );
}


void GuiInfoMarket::drawWorkers( int paintY )
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // picture of citizen
   Picture *pic = &PicLoader::instance().get_picture("paneling", 542);
   sdlFacade.drawPicture(*pic, *_d->bgPicture, 16+15, paintY);

   // number of workers
   char buffer[1000];
   sprintf(buffer, _("%d employes (%d requis)"), _building->getWorkers(), _building->getMaxWorkers());

   Font &font = FontCollection::instance().getFont(FONT_2);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer), 16+42, paintY+5, font);
   paintY+=20;
}


void GuiInfoMarket::drawGood(const GoodType &goodType, int& paintY )
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
   sdlFacade.drawPicture(pic, *_d->bgPicture, 31, paintY);

   char buffer[1000];
   sprintf(buffer, "%d ", qty);
   sdlFacade.drawText(*_d->bgPicture, std::string(buffer) + good.getName(), 61, paintY, font);

   paintY += 22;
}

GuiBuilding::GuiBuilding( Widget* parent, Building &building)
    : GuiInfoBox( parent, Rect( 0, 0, 450, 220 ), -1 )
{
  _building = &building;
  setTitle( BuildingDataHolder::instance().getData(building.getType()).getPrettyName() );

  paint();
}

void GuiBuilding::paint()
{
   int paintY = _d->lbTitle->getBottom() + 10;
   GuiPaneling::instance().draw_black_frame(*_d->bgPicture, 16, paintY, getWidth()-32, getHeight()-paintY-16);
   paintY+=10;  
}

InfoBoxLand::InfoBoxLand( Widget* parent, Tile* tile )
    : GuiInfoBox( parent, Rect( 0, 0, 510, 350 ), -1 )
{
    _text = new Label( this, Rect( 36, 239, 470, 338 ), "", true );
    _text->setFont( FontCollection::instance().getFont(FONT_2) );

    if( tile->get_terrain().isTree() )
    {
        setTitle( _("##trees_and_forest_caption") );
        _text->setText( _("##trees_and_forest_text"));
    } 
    else if( tile->get_terrain().isWater() )
    {
        setTitle( _("##water_caption") );
        _text->setText( _("##water_text"));
    }
    else if( tile->get_terrain().isRock() )
    {
        setTitle( _("##rock_caption") );
        _text->setText( _("##rock_text"));
    }
    else if( tile->get_terrain().isRoad() )
    {
        setTitle( _("##road_caption") );
        _text->setText( _("##road_text"));
    }
    else 
    {
        setTitle( _("##clear_land_caption") );
        _text->setText( _("##clear_land_text"));
    }

    GuiPaneling::instance().draw_black_frame( *_d->bgPicture, 16, _d->lbTitle->getBottom() + 10, getWidth()-32, 180 );
}

void InfoBoxLand::setText( const std::string& text )
{
    _text->setText( text );
}

InfoBoxFreeHouse::InfoBoxFreeHouse( Widget* parent, Tile* tile )
    : InfoBoxLand( parent, tile )
{
    setTitle( _("##freehouse_caption") );
    setText( _("##freehouse_text") );
}   