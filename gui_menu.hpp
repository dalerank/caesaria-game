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


#ifndef GUI_MENU_HPP
#define GUI_MENU_HPP

#include "gui_widget.hpp"

#include <string>

#include "picture.hpp"
#include "enums.hpp"


class City;

// the line at the top of the screen
class MenuBar : public WidgetGroup
{
public:
   MenuBar();
   void init( const unsigned int width, const unsigned int heigth );

   // draw on screen
   virtual void draw(const int dx, const int dy);
   void updateLabels(const bool forceUpdate = false); // update labels, if needed

   Picture& getBgPicture();

private:
   City *_city;
   int _population;  // cached value
   int _funds;       // cached value
   unsigned int _month;       // cached value

   TextIcon _populationLabel;
   TextIcon _fundsLabel;
   TextIcon _dateLabel;
   Picture *_bgPicture;
};


// this is the main menu interface with all construction tools and the minimap
class Menu : public WidgetGroup
{
public:
   Menu();

   // draw on screen
   virtual void draw(const int dx, const int dy);

   // unselect button, if any
   void unselect();

   const Picture& getBgPicture() const;
   const Picture& getBottomPicture() const; 

private:

   // init the given button with data. this button has 3 states
   void set3Button(ImageButton &oButton, const WidgetEvent &event, const int pic_index);

   // init the given button with data. this button has 4 states
   void set4Button(ImageButton &oButton, const WidgetEvent &event, const int pic_index);

   Picture* _bgPicture;
   Picture* _btPicture;

   ImageIcon _midIcon;
   //ImageIcon _bottomIcon;
   ImageTextButton _menuButton;
   ImageButton _minimizeButton;
   ImageButton _senateButton;
   ImageButton _empireButton;
   ImageButton _missionButton;
   ImageButton _northButton;
   ImageButton _rotateLeftButton;
   ImageButton _rotateRightButton;
   ImageButton _messageButton;
   ImageButton _disasterButton;
   ImageButton _houseButton;
   ImageButton _waterButton;
   ImageButton _clearButton;
   ImageButton _roadButton;
   ImageButton _administrationButton;
   ImageButton _entertainmentButton;
   ImageButton _educationButton;
   ImageButton _templeButton;
   ImageButton _commerceButton;
   ImageButton _securityButton;
   ImageButton _healthButton;
   ImageButton _engineerButton;
   ImageButton _cancelButton;

   ExclusiveButtonGroup _exclusiveButtonGroup;
};


// this is the sub menu with building choice. example: selected=water, submenu=(aqueduct, well, fountain, ...)
class BuildMenu : public WidgetGroup
{
public:
   BuildMenu();
   virtual void addButtons() = 0;
   virtual ~BuildMenu();
   void init();

   static BuildMenu* getMenuInstance(const BuildMenuType menuType);

   // draw on screen
   virtual void draw(const int dx, const int dy);

   void handleEvent(SDL_Event &event);

   // add the subMenu in the menu.
   void addSubmenuButton(const BuildMenuType menuType, const std::string &text);
   // add the button in the menu.
   void addBuildButton(const BuildingType buildingType);

   // returns true if widget needs to be deleted
   bool isDeleted() const;
   void setDeleted();

protected:
   bool _isDeleted;  // true if needs to be deleted

   BuildButton* _hoverButton;
};


class BuildMenu_water : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_security : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_education : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_health : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_engineering : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_administration : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_entertainment : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_commerce : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_farm : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_raw_factory : public BuildMenu
{
public:
   void addButtons();
};


class BuildMenu_factory : public BuildMenu
{
public:
   void addButtons();
};

class BuildMenu_religion: public BuildMenu
{
public:
   void addButtons();
};

class BuildMenu_temple : public BuildMenu
{
public:
   void addButtons();
};

class BuildMenu_bigtemple : public BuildMenu
{
public:
   void addButtons();
};


// this is the menu with newGame/loadGame/quit choice
class StartMenu : public WidgetGroup
{
public:
   StartMenu();
   virtual ~StartMenu();
   void init();

   // draw on screen
   virtual void draw(const int dx, const int dy);

   // returns true if widget needs to be deleted
   bool isDeleted() const;
   void setDeleted();

protected:
   bool _isDeleted;  // true if needs to be deleted

   TextButton* _hoverButton;
   TextButton _newGameButton;
   TextButton _loadGameButton;
   TextButton _quitButton;
};


// this is the inGame menu with options/saveGame/quitGame choice
class InGameMenu : public WidgetGroup
{
public:
   InGameMenu();
   virtual ~InGameMenu();
   void init();

   // draw on screen
   virtual void draw(const int dx, const int dy);

   void handleEvent(SDL_Event &event);

   // returns true if widget needs to be deleted
   bool isDeleted() const;
   void setDeleted();

protected:
   bool _isDeleted;  // true if needs to be deleted

   TextButton* _hoverButton;
   TextButton _optionsButton;
   TextButton _saveGameButton;
   TextButton _quitGameButton;
};


#endif
