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


#ifndef GUI_INFO_BOX_HPP
#define GUI_INFO_BOX_HPP

#include <string>
#include <list>

#include "oc3_widget.hpp"
#include "oc3_picture.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_building_service.hpp"
#include "oc3_predefinitions.hpp"

class PushButton;
class Label;

// base class for info boxes
class GuiInfoBox : public Widget
{
public:
    GuiInfoBox( Widget* parent, const Rect& rect, int id );
    ~GuiInfoBox();
    
    void draw( GfxEngine& engine );  // draw on screen

    Picture& getBgPicture();

    bool onEvent( const NEvent& event);

    bool isPointInside(const Point& point) const;

    void setTitle( const std::string& title );

    bool isAutoPosition() const;
    void setAutoPosition( bool value );
protected:
    void _resizeEvent();

    class Impl;
    ScopedPtr< Impl > _d;
};

// Simple info box with static text on plain background
class InfoBoxBasic : public GuiInfoBox
{
public:
  InfoBoxBasic( Widget* parent, const Tile& tile );

  void setText( const std::string& text );

private:
  class Impl;
  ScopedPtr< Impl > _bd;
};


class InfoBoxRawMaterial : public GuiInfoBox
{
public:
  InfoBoxRawMaterial( Widget* parent, const Tile& tile );

  void paint();

private:
  class Impl;
  ScopedPtr< Impl > _fd;
};

// info box about a service building
class InfoBoxWorkingBuilding : public GuiInfoBox
{
public:
  InfoBoxWorkingBuilding( Widget* parent, WorkingBuildingPtr building);
  
  void paint();
  void drawWorkers( int );
  void setText(const std::string& text);
private:

  class Impl;
  ScopedPtr< Impl > _sd;
};

class InfoBoxSenate : public GuiInfoBox
{
public:
  InfoBoxSenate( Widget* parent, const Tile& tile );

  void paint();
private:

  class Impl;
  ScopedPtr< Impl > _sd;
};

class InfoBoxLand : public GuiInfoBox
{
public:
  InfoBoxLand( Widget* parent, const Tile& tile );   
  void setText( const std::string& text );
  //bool onEvent(const NEvent& event);
private:
  void _paint();

  Label* _text;
};

class InfoBoxFreeHouse : public InfoBoxLand
{
public:
    InfoBoxFreeHouse( Widget* parent, const Tile& tile );   
};
// info box about a factory building
class GuiInfoFactory : public GuiInfoBox
{
public:
   GuiInfoFactory( Widget* parent, const Tile& tile );
   virtual void paint();

   void drawWorkers( int );
   std::string getInfoText();

private:
   FactoryPtr _building;
};


// info box about a granary
class GuiInfoGranary : public GuiInfoBox
{
public:
  GuiInfoGranary( Widget* parent, const Tile& tile );
  
  void paint();
  void drawWorkers( int );
  void drawGood(const GoodType &goodType, int, int&);
  void showSpecialOrdersWindow();

private:
  class Impl;
  ScopedPtr< Impl > _gd;
};


// info box about a market
class GuiInfoMarket : public GuiInfoBox
{
public:
   GuiInfoMarket( Widget* parent, const Tile& tile );
   
   void paint();
   void drawWorkers();
   void drawGood(const GoodType &goodType, int, int );

private:
   class Impl;
   ScopedPtr< Impl > _md;
};

class InfoBoxWarehouse : public GuiInfoBox
{
public:
  InfoBoxWarehouse( Widget* parent, const Tile& tile );

  void paint();
  void drawWorkers();
  void drawGood(const GoodType &goodType, int, int& );
  void showSpecialOrdersWindow();

private:
  class Impl;
  ScopedPtr< Impl > _wd;
};


class InfoBoxTemple : public GuiInfoBox
{
public:
  InfoBoxTemple( Widget* parent, const Tile& tile );

  void drawWorkers();
  void drawPicture();
private:
  class Impl;
  ScopedPtr< Impl > _td;
};

// info box about a house
class InfoBoxHouse : public GuiInfoBox
{
public:
   InfoBoxHouse( Widget* paarent, const Tile& tile);

   void drawHabitants();
   void drawGood(const GoodType &goodType, const int col, const int row, const int startY );

private:
   void _paint();
   
   class Impl;
   ScopedPtr< Impl > _ed;
};

class GuiBuilding : public GuiInfoBox
{
public:
   GuiBuilding( Widget* parent, const Tile& tile );
   virtual void paint();

private:

   class Impl;
   ScopedPtr< Impl > _bd;
};

#endif
