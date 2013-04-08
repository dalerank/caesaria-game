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


#include "gui_menu.hpp"

#include <iostream>
#include <SDL.h>
#include <cmath>
#include <iterator>

#include "exception.hpp"
#include "pic_loader.hpp"
#include "sdl_facade.hpp"
#include "gui_paneling.hpp"
#include "gettext.hpp"
#include "building.hpp"
#include "building_data.hpp"
#include "city.hpp"
#include "scenario.hpp"
#include "oc3_positioni.h"

namespace {
static const char* panelBgName = "paneling";
static const Uint32 panelBgStatus = 15;
static const Uint32 dateLabelOffset = 155;
static const Uint32 populationLabelOffset = 345;
static const Uint32 fundLabelOffset = 465;
static const Uint32 maximizeBtnPicId = 101;
}

MenuBar::MenuBar()
{
   _population = -1;
   _funds = -1;
}

void MenuBar::draw(const int dx, const int dy)
{
   updateLabels();
   drawPicture(getBgPicture(), dx, dy);
   drawChildren(dx, dy);
}

void MenuBar::updateLabels(const bool forceUpdate)
{
   char buffer[100];

   if (forceUpdate || _city->getPopulation() != _population)
   {
      _population = _city->getPopulation();
      sprintf(buffer, "Pop %d", _population);  // "'" is the thousands separator
      _populationLabel.setText(std::string(buffer));
   }

   if (forceUpdate || _city->getFunds() != _funds)
   {
      _funds = _city->getFunds();
      sprintf(buffer, "Dn %d", _funds);  // "'" is the thousands separator
      _fundsLabel.setText(std::string(buffer));
   }

   if (forceUpdate || _city->getMonth() != _month)
   {
      _month = _city->getMonth();
      
      const char *args[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
      const char *age[] = {"BC", "AD"};
      
      sprintf(buffer, "%.3s %d %.2s", args[_month % 12], (int)std::abs(((int)_month/12-39)), age[((int)_month/12-39)>0]);
      
      //_dateLabel.setText("Feb 39 BC");
      _dateLabel.setText(std::string(buffer));
   }   
}

void MenuBar::init( const unsigned int width, const unsigned int heigth  )
{
    setSize( width, heigth );

   PicLoader& loader = PicLoader::instance();
   FontCollection& fonts = FontCollection::instance();

   std::vector<Picture> p_marble;
   
   for (int i = 1; i<=12; ++i)
   {
      p_marble.push_back(loader.get_picture( panelBgName, i));
   }

   SdlFacade &sdlFacade = SdlFacade::instance();
   _bgPicture = &sdlFacade.createPicture(_width, _height);
   SDL_SetAlpha(_bgPicture->get_surface(), 0, 0);  // remove surface alpha

   int i = 0;
   int x = 0;
   while (x < _width)
   {
      const Picture& pic = p_marble[i%10];
      sdlFacade.drawPicture(pic, *_bgPicture, x, 0);
      x += pic.get_width();
      i++;
   }

   _populationLabel.setSize(120, 23);
   _populationLabel.setBgPicture( &loader.get_picture( panelBgName, panelBgStatus ) );
   _populationLabel.setFont( fonts.getFont(FONT_2_WHITE));
   _populationLabel.setText("Pop 34,124");
   _populationLabel.setPosition( _width - populationLabelOffset, 1);
   _populationLabel.setTextPosition(20, 0);
   add_widget(_populationLabel);

   _fundsLabel.setSize(120, 23);
   _fundsLabel.setFont( fonts.getFont(FONT_2_WHITE));
   _fundsLabel.setBgPicture( &loader.get_picture( panelBgName, panelBgStatus ) );
   _fundsLabel.setText("Dn 10,000");
   _fundsLabel.setPosition( _width - fundLabelOffset, 1);
   _fundsLabel.setTextPosition(20, 0);
   add_widget(_fundsLabel);

   _dateLabel.setSize(120, 23);
   _dateLabel.setFont( fonts.getFont(FONT_2_YELLOW));
   _dateLabel.setBgPicture( &loader.get_picture( panelBgName, panelBgStatus ) );
   _dateLabel.setText("Feb 39 BC");
   _dateLabel.setPosition( _width - dateLabelOffset, 1);
   _dateLabel.setTextPosition(20, 0);
   add_widget(_dateLabel);

   GfxEngine::instance().load_picture(*_bgPicture);

   _city = &Scenario::instance().getCity();
   updateLabels(true);  // force update
}


Picture& MenuBar::getBgPicture()
{
   return *_bgPicture;
}


Menu::Menu()
{
  menuType = false;
  
  if (menuType) 
  {
    // SMALL MENU
    // _bgPicture = &PicLoader::instance().get_picture( panelBgName, 16 );
    // _btPicture = &PicLoader::instance().get_picture( panelBgName, 21 );
  }
  else
  {
    // BIG MENU
    _bgPicture = &PicLoader::instance().get_picture( panelBgName, 17 );
    _btPicture = &PicLoader::instance().get_picture( panelBgName, 20 );
  }
   
  const Picture& rigthImageTile = PicLoader::instance().get_picture( panelBgName, 14 );

  setSize(_bgPicture->get_width() + rigthImageTile.get_width(), _bgPicture->get_height() + _btPicture->get_height() );
  SdlFacade &sdlFacade = SdlFacade::instance();
  _rigthPicture = &sdlFacade.createPicture( rigthImageTile.get_width(), getHeight());
  SDL_SetAlpha(_bgPicture->get_surface(), 0, 0);  // remove surface alpha

  int y = 0;
  while( y < getHeight() )
  {
    sdlFacade.drawPicture(rigthImageTile, *_rigthPicture, 0, y);
    y += rigthImageTile.get_height();
  }

  // // top of menu
  //_menuButton.setText("Menu");
  //_menuButton.setEvent(WidgetEvent::InGameMenuEvent());
  //_menuButton.setNormalPicture(PicLoader::instance().get_picture("paneling", 234));
  //_menuButton.setHoverPicture(PicLoader::instance().get_picture("paneling", 234+1));
  //_menuButton.setSelectedPicture(PicLoader::instance().get_picture("paneling", 234+2));
  //_menuButton.setUnavailablePicture(PicLoader::instance().get_picture("paneling", 234+3));
  //_menuButton.init_pictures();
  //add_widget(_menuButton);
  
  Point offset( 1, 32 );
  int dy = 35;

  set4Button(_menuButton , WidgetEvent::ChangeSideMenuType(), 15 );
  _menuButton.setPosition( Point( 4, 4 ));
  
  if (menuType)
  {
    set4Button( _minimizeButton, WidgetEvent::ChangeSideMenuType(), maximizeBtnPicId );
    _minimizeButton.setPosition( Point( 6, 4 ));
  }
  else
  {
    set4Button( _minimizeButton, WidgetEvent::ChangeSideMenuType(), 97 );
    _minimizeButton.setPosition( Point( 127, 5 ));
  }
   // //
   // _midIcon.setPicture(PicLoader::instance().get_picture("panelwindows", 1));
   // _midIcon.setPosition(8, 217);
   // add_widget(_midIcon);

   //_bottomIcon.setPicture(PicLoader::instance().get_picture("paneling", 20));
   //_bottomIcon.setPosition(0, _bgPicture->get_surface()->h);
   //add_widget(_bottomIcon);

   // // header
   // set3Button(_senateButton, WidgetEvent(), 79);
   // set3Button(_empireButton, WidgetEvent(), 82);
   // set3Button(_missionButton, WidgetEvent(), 85);
   // set3Button(_northButton, WidgetEvent(), 88);
   // set3Button(_rotateLeftButton, WidgetEvent(), 91);
   // set3Button(_rotateRightButton, WidgetEvent(), 94);

  if (menuType)
  {
    set4Button(_houseButton, WidgetEvent::BuildingEvent(B_HOUSE), 123);
    _houseButton.setPosition( offset + Point( 0, dy * 0 ) );
    set4Button(_clearButton, WidgetEvent::ClearLandEvent(), 131);
    _clearButton.setPosition( offset + Point( 0, dy * 1 ) );    
    set4Button(_roadButton, WidgetEvent::BuildingEvent(B_ROAD), 135);
    _roadButton.setPosition( offset + Point( 0, dy * 2 ) );
    // second row
    set4Button(_waterButton, WidgetEvent::BuildMenuEvent(BM_WATER), 127);
    _waterButton.setPosition( offset + Point( 0, dy * 3 ));
    set4Button(_healthButton, WidgetEvent::BuildMenuEvent(BM_HEALTH), 163);
    _healthButton.setPosition( offset + Point( 0, dy * 4 ) );
    set4Button(_templeButton, WidgetEvent::BuildMenuEvent(BM_RELIGION), 151);
    _templeButton.setPosition( offset + Point( 0, dy * 5 ) );
    // third row
    set4Button(_educationButton, WidgetEvent::BuildMenuEvent(BM_EDUCATION), 147);
    _educationButton.setPosition( offset + Point( 0, dy * 6 ) );
    set4Button(_entertainmentButton, WidgetEvent::BuildMenuEvent(BM_ENTERTAINMENT), 143);
    _entertainmentButton.setPosition( offset + Point( 0, dy * 7 ) );
    set4Button(_administrationButton, WidgetEvent::BuildMenuEvent(BM_ADMINISTRATION), 139);
    _administrationButton.setPosition( offset + Point( 0, dy * 8 ) );
    // 4th row
    set4Button(_engineerButton, WidgetEvent::BuildMenuEvent(BM_ENGINEERING), 167);
    _engineerButton.setPosition( offset + Point( 0, dy * 9 ) );
    set4Button(_securityButton, WidgetEvent::BuildMenuEvent(BM_SECURITY), 159);
    _securityButton.setPosition( offset + Point( 0, dy * 10 ) );
    set4Button(_commerceButton, WidgetEvent::BuildMenuEvent(BM_COMMERCE), 155);
    _commerceButton.setPosition( offset + Point( 0, dy * 11 ) );
  }
  else
  {
    set4Button(_houseButton, WidgetEvent::BuildingEvent(B_HOUSE), 123);
    _houseButton.setPosition( Point( 13, 277 ) );
    set4Button(_clearButton, WidgetEvent::ClearLandEvent(), 131);
    _clearButton.setPosition( Point( 64, 277 ) );    
    set4Button(_roadButton, WidgetEvent::BuildingEvent(B_ROAD), 135);
    _roadButton.setPosition( Point( 113, 277) );   
    // second row
    set4Button(_waterButton, WidgetEvent::BuildMenuEvent(BM_WATER), 127);
    _waterButton.setPosition( Point( 13, 313));
    set4Button(_healthButton, WidgetEvent::BuildMenuEvent(BM_HEALTH), 163);
    _healthButton.setPosition( Point( 64, 313 ) );
    set4Button(_templeButton, WidgetEvent::BuildMenuEvent(BM_RELIGION), 151);
    _templeButton.setPosition( Point( 113, 313 ) );
    // third row
    set4Button(_educationButton, WidgetEvent::BuildMenuEvent(BM_EDUCATION), 147);
    _educationButton.setPosition( Point( 13, 349 ) );
    set4Button(_entertainmentButton, WidgetEvent::BuildMenuEvent(BM_ENTERTAINMENT), 143);
    _entertainmentButton.setPosition( Point( 64, 349 ) );
    set4Button(_administrationButton, WidgetEvent::BuildMenuEvent(BM_ADMINISTRATION), 139);
    _administrationButton.setPosition( Point( 113, 349 ) );
    // 4th row
    set4Button(_engineerButton, WidgetEvent::BuildMenuEvent(BM_ENGINEERING), 167);
    _engineerButton.setPosition( Point( 13, 385 ) );
    set4Button(_securityButton, WidgetEvent::BuildMenuEvent(BM_SECURITY), 159);
    _securityButton.setPosition( Point( 64, 385 ) );
    set4Button(_commerceButton, WidgetEvent::BuildMenuEvent(BM_COMMERCE), 155);
    _commerceButton.setPosition( Point( 113, 385 ) );
    // 5th row
    set4Button(_cancelButton, WidgetEvent(), 171);
    _cancelButton.setPosition( Point( 13, 421 ) );
    set4Button(_messageButton, WidgetEvent(), 115);
    _messageButton.setPosition( Point( 64, 421 ) );
    set4Button(_disasterButton, WidgetEvent(), 119);
    _disasterButton.setPosition( Point( 113, 421 ) );
  }

   // set button position
   // _menuButton.setPosition(4, 3);
   // _minimizeButton.setPosition(127, 5);
   // header
   // _senateButton.setPosition(7, 155);
   // _empireButton.setPosition(84, 155);
   // _missionButton.setPosition(7, 184);
   // _northButton.setPosition(46, 184);
   // _rotateLeftButton.setPosition(84, 184);
   // _rotateRightButton.setPosition(123, 184);
   // first row

   
   
   // second row
   
   
   
   // third row
   
   
   
   // 4th row
   
   
   
//   // 5th row
//   _cancelButton.setPosition(x0, y0+dy*12);
//   _messageButton.setPosition(x0, y0+dy*13);
//   _disasterButton.setPosition(x0, y0+dy*14);

//   _bgPicture = &PicLoader::instance().get_picture("paneling", 17);
//   setSize(_bgPicture->get_surface()->w, _bgPicture->get_surface()->h);
//
//   // top of menu
//   _menuButton.setText("Menu");
//   _menuButton.setEvent(WidgetEvent::InGameMenuEvent());
//   _menuButton.setNormalPicture(PicLoader::instance().get_picture("paneling", 234));
//   _menuButton.setHoverPicture(PicLoader::instance().get_picture("paneling", 234+1));
//   _menuButton.setSelectedPicture(PicLoader::instance().get_picture("paneling", 234+2));
//   _menuButton.setUnavailablePicture(PicLoader::instance().get_picture("paneling", 234+3));
//   _menuButton.init_pictures();
//   add_widget(_menuButton);
//
//   set4Button(_minimizeButton, WidgetEvent(), 97);
//
//   //
//   _midIcon.setPicture(PicLoader::instance().get_picture("panelwindows", 1));
//   _midIcon.setPosition(8, 217);
//   add_widget(_midIcon);
//
//   //_bottomIcon.setPicture(PicLoader::instance().get_picture("paneling", 20));
//   //_bottomIcon.setPosition(0, _bgPicture->get_surface()->h);
//   //add_widget(_bottomIcon);
//
//   // header
//   set3Button(_senateButton, WidgetEvent(), 79);
//   set3Button(_empireButton, WidgetEvent(), 82);
//   set3Button(_missionButton, WidgetEvent(), 85);
//   set3Button(_northButton, WidgetEvent(), 88);
//   set3Button(_rotateLeftButton, WidgetEvent(), 91);
//   set3Button(_rotateRightButton, WidgetEvent(), 94);
//
//   set4Button(_houseButton, WidgetEvent::BuildingEvent(B_HOUSE), 123);
//   set4Button(_clearButton, WidgetEvent::ClearLandEvent(), 131);
//   set4Button(_roadButton, WidgetEvent::BuildingEvent(B_ROAD), 135);
//   // second row
//   set4Button(_waterButton, WidgetEvent::BuildMenuEvent(BM_WATER), 127);
//   set4Button(_healthButton, WidgetEvent::BuildMenuEvent(BM_HEALTH), 163);
//   set4Button(_templeButton, WidgetEvent::BuildMenuEvent(BM_TEMPLE), 151);
//   // third row
//   set4Button(_educationButton, WidgetEvent::BuildMenuEvent(BM_EDUCATION), 147);
//   set4Button(_entertainmentButton, WidgetEvent::BuildMenuEvent(BM_ENTERTAINMENT), 143);
//   set4Button(_administrationButton, WidgetEvent::BuildMenuEvent(BM_ADMINISTRATION), 139);
//   // 4th row
//   set4Button(_engineerButton, WidgetEvent::BuildMenuEvent(BM_ENGINEERING), 167);
//   set4Button(_securityButton, WidgetEvent::BuildMenuEvent(BM_SECURITY), 159);
//   set4Button(_commerceButton, WidgetEvent::BuildMenuEvent(BM_COMMERCE), 155);
//   // 5th row
//   set4Button(_cancelButton, WidgetEvent(), 171);
//   set4Button(_messageButton, WidgetEvent(), 115);
//   set4Button(_disasterButton, WidgetEvent(), 119);
//
//   // set button position
//   _menuButton.setPosition(4, 3);
//   _minimizeButton.setPosition(127, 5);
//   // header
//   _senateButton.setPosition(7, 155);
//   _empireButton.setPosition(84, 155);
//   _missionButton.setPosition(7, 184);
//   _northButton.setPosition(46, 184);
//   _rotateLeftButton.setPosition(84, 184);
//   _rotateRightButton.setPosition(123, 184);
//   // first row
//   _houseButton.setPosition(13, 277);
//   _clearButton.setPosition(63, 277);
//   _roadButton.setPosition(113, 277);
//   // second row
//   _waterButton.setPosition(13, 313);
//   _healthButton.setPosition(63, 313);
//   _templeButton.setPosition(113, 313);
//   // third row
//   _educationButton.setPosition(13, 349);
//   _entertainmentButton.setPosition(63, 349);
//   _administrationButton.setPosition(113, 349);
//   // 4th row
//   _engineerButton.setPosition(13, 385);
//   _securityButton.setPosition(63, 385);
//   _commerceButton.setPosition(113, 385);
//   // 5th row
//   _cancelButton.setPosition(13, 421);
//   _messageButton.setPosition(63, 421);
//   _disasterButton.setPosition(113, 421);
}

void Menu::draw(const int dx, const int dy)
{
   drawPicture( getBgPicture(), dx, dy );
   drawPicture( getBottomPicture(), dx, dy + _bgPicture->get_height() );
   drawPicture( getRigthPicture(), dx + getBgPicture().get_width(), dy );
   drawChildren( dx, dy );
}


const Picture& Menu::getBgPicture() const
{
   return *_bgPicture;
}

void Menu::set3Button(ImageButton &oButton, const WidgetEvent &event, const int pic_index)
{
   PicLoader& loader = PicLoader::instance();
   oButton.setEvent(event);
   oButton.setNormalPicture( loader.get_picture( panelBgName, pic_index));
   oButton.setHoverPicture( loader.get_picture( panelBgName, pic_index+1));
   oButton.setSelectedPicture( loader.get_picture( panelBgName, pic_index+2));
   oButton.setExclusiveButtonGroup(&_exclusiveButtonGroup);
   add_widget(oButton);
}

void Menu::set4Button(ImageButton &oButton, const WidgetEvent &event, const int pic_index)
{
    PicLoader& loader = PicLoader::instance();
   oButton.setEvent(event);
   oButton.setNormalPicture( loader.get_picture(panelBgName, pic_index));
   oButton.setHoverPicture( loader.get_picture(panelBgName, pic_index+1));
   oButton.setSelectedPicture( loader.get_picture(panelBgName, pic_index+2));
   oButton.setUnavailablePicture( loader.get_picture(panelBgName, pic_index+3));
   oButton.setExclusiveButtonGroup(&_exclusiveButtonGroup);
   add_widget(oButton);
}

void Menu::unselect()
{
   MenuButton *selectedButton = _exclusiveButtonGroup.getSelectedButton();
   if (selectedButton != NULL)
   {
      selectedButton->setState(B_NORMAL);
   }
}

const Picture& Menu::getBottomPicture() const
{
    return *_btPicture;
}

const Picture& Menu::getRigthPicture() const
{
    return *_rigthPicture;
}

BuildMenu::BuildMenu()
{
   _isDeleted = false;
   _hoverButton = NULL;
}

void BuildMenu::handleEvent(SDL_Event &event)
{
   if (event.type == SDL_MOUSEBUTTONDOWN)
   {
      // mouse click

      if (event.button.button == SDL_BUTTON_RIGHT)
      {
         // right click
         _isDeleted = true;
         return;
      }
   }
   else if (event.type == SDL_KEYDOWN)
   {
      if (event.key.keysym.sym == SDLK_ESCAPE)
      {
         _isDeleted = true;
         return;
      }
   }

   WidgetGroup::handleEvent(event);
}


void BuildMenu::init()
{
   addButtons();

   // compute the necessary width
   SdlFacade &sdlFacade = SdlFacade::instance();
   int max_text_width = 0;
   int max_cost_width = 0;
   int text_width;
   int text_height;
   std::stringstream cost_ss;
   Font &font = FontCollection::instance().getFont(FONT_2);
   for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
   {
      Widget *widget = *itWidget;
      BuildButton *button = (BuildButton*) widget;
      sdlFacade.getTextSize(font, button->getText(), text_width, text_height);
      max_text_width = std::max(max_text_width, text_width);

      cost_ss.str("");
      cost_ss << button->getCost() << "Dn";
      sdlFacade.getTextSize(font, cost_ss.str(), text_width, text_height);
      max_cost_width = std::max(max_cost_width, text_width);
   }

   _width = std::max(150, max_text_width + max_cost_width + 20);

   // set the same size for all buttons
   for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
   {
      Widget *widget = *itWidget;
      BuildButton *button = (BuildButton*) widget;
      button->setSize(_width, 25);
      button->setCostX(_width-max_cost_width-10);
      button->init_pictures();
   }

}


BuildMenu::~BuildMenu()
{
   for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
   {
      Widget *widget = *itWidget;
      delete widget;
   }
}


void BuildMenu::draw(const int dx, const int dy)
{
   drawChildren(dx, dy);
}


bool BuildMenu::isDeleted() const
{
   return _isDeleted;
}

void BuildMenu::setDeleted()
{
   _isDeleted = true;
}


void BuildMenu::addSubmenuButton(const BuildMenuType menuType, const std::string &text)
{
   BuildButton* button = new BuildButton();
   button->setEvent(WidgetEvent::BuildMenuEvent(menuType));
   button->setCost(-1);  // no display
   button->setPosition(0, _height);
   button->setText(text);
   add_widget(*button);

   _height += 30;
}


void BuildMenu::addBuildButton(const BuildingType buildingType)
{
   BuildingData &buildingData = BuildingDataHolder::instance().getData(buildingType);

   int cost = buildingData.getCost();
   if (cost != -1)
   {
      // building can be built
      BuildButton* button = new BuildButton();
      button->setEvent(WidgetEvent::BuildingEvent(buildingType));
      button->setCost(cost);
      button->setPosition(0, _height);
      button->setText(buildingData.getPrettyName());
      add_widget(*button);

      _height += 30;
   }
}


BuildMenu *BuildMenu::getMenuInstance(const BuildMenuType menuType)
{
   BuildMenu *res = NULL;

   switch (menuType)
   {
   case BM_WATER:          res = new BuildMenu_water(); break;
   case BM_HEALTH:         res = new BuildMenu_health(); break;
   case BM_SECURITY:       res = new BuildMenu_security(); break;
   case BM_EDUCATION:      res = new BuildMenu_education(); break;
   case BM_ENGINEERING:    res = new BuildMenu_engineering(); break;
   case BM_ADMINISTRATION: res = new BuildMenu_administration(); break;
   case BM_ENTERTAINMENT:  res = new BuildMenu_entertainment(); break;
   case BM_COMMERCE:       res = new BuildMenu_commerce(); break;
   case BM_FARM:           res = new BuildMenu_farm(); break;
   case BM_RAW_MATERIAL:   res = new BuildMenu_raw_factory(); break;
   case BM_FACTORY:        res = new BuildMenu_factory(); break;
   case BM_RELIGION:       res = new BuildMenu_religion(); break;
   case BM_TEMPLE:         res = new BuildMenu_temple(); break;
   case BM_BIGTEMPLE:      res = new BuildMenu_bigtemple(); break;
   default:       break; // DO NOTHING 
   };

   return res;
}

void BuildMenu_water::addButtons()
{
  addBuildButton(B_FOUNTAIN);
  addBuildButton(B_WELL);
  addBuildButton(B_AQUEDUCT);
  addBuildButton(B_RESERVOIR);
}


void BuildMenu_security::addButtons()
{
  addBuildButton(B_PREFECT);
  addBuildButton(B_FORT_LEGIONNAIRE);
  addBuildButton(B_FORT_JAVELIN);
  addBuildButton(B_FORT_MOUNTED);
  addBuildButton(B_MILITARY_ACADEMY);
  addBuildButton(B_BARRACKS);
}


void BuildMenu_education::addButtons()
{
  addBuildButton(B_SCHOOL);
  addBuildButton(B_LIBRARY);
  addBuildButton(B_COLLEGE);
  addBuildButton(B_MISSION_POST);
}


void BuildMenu_health::addButtons()
{
  addBuildButton(B_DOCTOR);
  addBuildButton(B_BARBER);
  addBuildButton(B_BATHS);
  addBuildButton(B_HOSPITAL);
}


void BuildMenu_engineering::addButtons()
{
  addBuildButton(B_ENGINEER);
  addBuildButton(B_LOW_BRIDGE);
  addBuildButton(B_HIGH_BRIDGE);
  addBuildButton(B_DOCK);
  addBuildButton(B_SHIPYARD);
  addBuildButton(B_WHARF);
  addBuildButton(B_TRIUMPHAL_ARCH);
  addBuildButton(B_GARDEN);
  addBuildButton(B_PLAZA);   
}


void BuildMenu_administration::addButtons()
{
  addBuildButton(B_FORUM);
  addBuildButton(B_SENATE);
  
  addBuildButton(B_STATUE1);
  addBuildButton(B_STATUE2);
  addBuildButton(B_STATUE3);
  addBuildButton(B_GOVERNOR_HOUSE);
  addBuildButton(B_GOVERNOR_VILLA);
  addBuildButton(B_GOVERNOR_PALACE);
}


void BuildMenu_entertainment::addButtons()
{
   addBuildButton(B_THEATER);
   addBuildButton(B_AMPHITHEATER);
   addBuildButton(B_COLLOSSEUM);
   addBuildButton(B_HIPPODROME);
   addBuildButton(B_ACTOR);
   addBuildButton(B_GLADIATOR);
   addBuildButton(B_LION);
   addBuildButton(B_CHARIOT);
}


void BuildMenu_commerce::addButtons()
{
   addSubmenuButton(BM_FARM, _("Ferme"));
   addSubmenuButton(BM_RAW_MATERIAL, _("Matieres premieres"));
   addSubmenuButton(BM_FACTORY, _("Atelier"));
   addBuildButton(B_MARKET);
   addBuildButton(B_GRANARY);
   addBuildButton(B_WAREHOUSE);
}


void BuildMenu_farm::addButtons()
{
   addBuildButton(B_WHEAT);
   addBuildButton(B_FRUIT);
   addBuildButton(B_OLIVE);
   addBuildButton(B_GRAPE);
   addBuildButton(B_MEAT);
   addBuildButton(B_VEGETABLE);
}


void BuildMenu_raw_factory::addButtons()
{
   addBuildButton(B_MARBLE);
   addBuildButton(B_IRON);
   addBuildButton(B_TIMBER);
   addBuildButton(B_CLAY);
}


void BuildMenu_factory::addButtons()
{
   addBuildButton(B_WINE);
   addBuildButton(B_OIL);
   addBuildButton(B_WEAPON);
   addBuildButton(B_FURNITURE);
   addBuildButton(B_POTTERY);
}

void BuildMenu_religion::addButtons()
{
   addSubmenuButton(BM_TEMPLE ,_("Small temples"));   
   addSubmenuButton(BM_BIGTEMPLE ,_("Large temples"));
   addBuildButton(B_TEMPLE_ORACLE);   
}

void BuildMenu_temple::addButtons()
{

   addBuildButton(B_TEMPLE_CERES);
   addBuildButton(B_TEMPLE_NEPTUNE);
   addBuildButton(B_TEMPLE_MARS);
   addBuildButton(B_TEMPLE_VENUS);
   addBuildButton(B_TEMPLE_MERCURE);
}

void BuildMenu_bigtemple::addButtons()
{
   addBuildButton(B_BIG_TEMPLE_CERES);
   addBuildButton(B_BIG_TEMPLE_NEPTUNE);
   addBuildButton(B_BIG_TEMPLE_MARS);
   addBuildButton(B_BIG_TEMPLE_VENUS);
   addBuildButton(B_BIG_TEMPLE_MERCURE);
}

StartMenu::StartMenu()
{
   _isDeleted = false;
   _hoverButton = NULL;
}


void StartMenu::init()
{
   _newGameButton.setPosition(0, _height);
   _newGameButton.setText(_("New Game"));
   _newGameButton.setEvent(WidgetEvent::NewGameEvent());
   add_widget(_newGameButton);
   _height += 40;

   _loadGameButton.setPosition(0, _height);
   _loadGameButton.setText(_("Load Game"));
   _loadGameButton.setEvent(WidgetEvent::LoadGameEvent());
   add_widget(_loadGameButton);
   _height += 40;

   _quitButton.setPosition(0, _height);
   _quitButton.setText(_("Quit"));
   _quitButton.setEvent(WidgetEvent::QuitGameEvent());
   add_widget(_quitButton);
   _height += 40;

   // compute the necessary width
   SdlFacade &sdlFacade = SdlFacade::instance();
   int max_text_width = 0;
   int text_width;
   int text_height;
   std::stringstream cost_ss;
   Font &font = FontCollection::instance().getFont(FONT_2);
   for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
   {
      Widget *widget = *itWidget;
      BuildButton *button = (BuildButton*) widget;
      sdlFacade.getTextSize(font, button->getText(), text_width, text_height);
      max_text_width = std::max(max_text_width, text_width);
   }

   _width = std::max(200, max_text_width + 20);

   // set the same size for all buttons
   for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
   {
      Widget *widget = *itWidget;
      TextButton *button = (TextButton*) widget;
      button->setSize(_width, 25);
      button->init_pictures();
   }

}


StartMenu::~StartMenu()
{
}


void StartMenu::draw(const int dx, const int dy)
{
   drawChildren(dx, dy);
}


bool StartMenu::isDeleted() const
{
   return _isDeleted;
}

void StartMenu::setDeleted()
{
   _isDeleted = true;
}



InGameMenu::InGameMenu()
{
   _isDeleted = false;
   _hoverButton = NULL;
}

void InGameMenu::init()
{
   _optionsButton.setPosition(0, _height);
   _optionsButton.setText(_("Options"));
   _optionsButton.setEvent(WidgetEvent::OptionsMenuEvent());
   add_widget(_optionsButton);
   _height += 30;

   _saveGameButton.setPosition(0, _height);
   _saveGameButton.setText(_("Save Game"));
   _saveGameButton.setEvent(WidgetEvent::SaveGameEvent());
   add_widget(_saveGameButton);
   _height += 30;

   _quitGameButton.setPosition(0, _height);
   _quitGameButton.setText(_("Quit Game"));
   _quitGameButton.setEvent(WidgetEvent::QuitGameEvent());
   add_widget(_quitGameButton);
   _height += 30;

   // compute the necessary width
   SdlFacade &sdlFacade = SdlFacade::instance();
   int max_text_width = 0;
   int text_width;
   int text_height;
   std::stringstream cost_ss;
   Font &font = FontCollection::instance().getFont(FONT_2);
   for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
   {
      Widget *widget = *itWidget;
      BuildButton *button = (BuildButton*) widget;
      sdlFacade.getTextSize(font, button->getText(), text_width, text_height);
      max_text_width = std::max(max_text_width, text_width);
   }

   _width = std::max(200, max_text_width + 20);

   // set the same size for all buttons
   for (std::list<Widget*>::iterator itWidget = get_widget_list().begin(); itWidget != get_widget_list().end(); ++itWidget)
   {
      Widget *widget = *itWidget;
      TextButton *button = (TextButton*) widget;
      button->setSize(_width, 25);
      button->init_pictures();
   }

}


InGameMenu::~InGameMenu()
{
}


void InGameMenu::draw(const int dx, const int dy)
{
   drawChildren(dx, dy);
}

bool InGameMenu::isDeleted() const
{
   return _isDeleted;
}

void InGameMenu::setDeleted()
{
   _isDeleted = true;
}

void InGameMenu::handleEvent(SDL_Event &event)
{
   if (event.type == SDL_MOUSEBUTTONDOWN)
   {
      // mouse click

      if (event.button.button == SDL_BUTTON_RIGHT)
      {
         // right click
         _isDeleted = true;
         return;
      }
   }
   else if (event.type == SDL_KEYDOWN)
   {
      if (event.key.keysym.sym == SDLK_ESCAPE)
      {
         _isDeleted = true;
         return;
      }
   }

   WidgetGroup::handleEvent(event);
}

