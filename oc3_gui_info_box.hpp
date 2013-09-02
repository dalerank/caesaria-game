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
#include "oc3_building_factory.hpp"
#include "oc3_building_service.hpp"
#include "oc3_predefinitions.hpp"

class PushButton;
class Label;

// base class for info boxes
class InfoBoxSimple : public Widget
{
public:
  InfoBoxSimple( Widget* parent, const Rect& rect, const Rect& blackArea=Rect(), int id=-1 );
  virtual ~InfoBoxSimple();

  virtual void draw( GfxEngine& engine );  // draw on screen

  Picture& getBgPicture();

  virtual bool onEvent( const NEvent& event);

  bool isPointInside(const Point& point) const;

  void setTitle( const std::string& title );
  virtual void setText( const std::string& text );

  bool isAutoPosition() const;
  void setAutoPosition( bool value );

protected:
  virtual void _afterCreate() {}

  virtual void _drawWorkers( const Point& pos, int picId, int need, int have );

  class Impl;
  ScopedPtr< Impl > _d;
};

class InfoBoxRawMaterial : public InfoBoxSimple
{
public:
  InfoBoxRawMaterial( Widget* parent, const Tile& tile );
  virtual ~InfoBoxRawMaterial();
};

// info box about a service building
class InfoBoxWorkingBuilding : public InfoBoxSimple
{
  static const int lbHelpId=2;
public:
  InfoBoxWorkingBuilding( Widget* parent, WorkingBuildingPtr building );
  
  void drawWorkers( int );
  void setText(const std::string& text);
};

class InfoBoxSenate : public InfoBoxSimple
{
public:
  InfoBoxSenate( Widget* parent, const Tile& tile );
  virtual ~InfoBoxSenate();

  void paint();
};

class InfoBoxLand : public InfoBoxSimple
{
  static const int lbTextId=2;
public:
  InfoBoxLand( Widget* parent, const Tile& tile );   

  void setText( const std::string& text );
};

class InfoBoxFreeHouse : public InfoBoxLand
{
public:
  InfoBoxFreeHouse( Widget* parent, const Tile& tile );
};
// info box about a factory building
class GuiInfoFactory : public InfoBoxSimple
{
public:
   GuiInfoFactory( Widget* parent, const Tile& tile );
   std::string getInfoText();
};

// info box about a granary
class InfoBoxGranary : public InfoBoxSimple
{
public:
  InfoBoxGranary( Widget* parent, const Tile& tile );
  virtual ~InfoBoxGranary();
  
  void paint();
  void drawWorkers( int );
  void drawGood(const Good::Type &goodType, int, int);
  void showSpecialOrdersWindow();

private:
  GranaryPtr _granary;
};


// info box about a market
class InfoBoxMarket : public InfoBoxSimple
{
public:
   InfoBoxMarket( Widget* parent, const Tile& tile );
   virtual ~InfoBoxMarket();
   
   void paint();
   void drawWorkers();
   void drawGood( MarketPtr market, const Good::Type &goodType, int, int );
};

class InfoBoxWarehouse : public InfoBoxSimple
{
public:
  InfoBoxWarehouse( Widget* parent, const Tile& tile );
  virtual ~InfoBoxWarehouse();

  void drawGood( const Good::Type &goodType, int col, int paintY);
  void showSpecialOrdersWindow();
private:
  WarehousePtr _warehouse;
};

class InfoBoxTemple : public InfoBoxSimple
{
public:
  InfoBoxTemple( Widget* parent, const Tile& tile );
  virtual ~InfoBoxTemple();
};

class InfoBoxColosseum : public InfoBoxSimple
{
public:
  InfoBoxColosseum( Widget* parent, const Tile& tile );
  virtual ~InfoBoxColosseum();

  void drawWorkers();
  void drawPicture();
};


// info box about a house
class InfoBoxHouse : public InfoBoxSimple
{
public:
   InfoBoxHouse( Widget* parent, const Tile& tile);
   virtual ~InfoBoxHouse();

   void drawHabitants(HousePtr house);
   void drawGood(HousePtr house, const Good::Type &goodType, const int col, const int row, const int startY );
};

class GuiBuilding : public InfoBoxSimple
{
public:
   GuiBuilding( Widget* parent, const Tile& tile );
};

// Simple info box with static text on plain background
class InfoBoxCitizen : public InfoBoxSimple
{
public:
  InfoBoxCitizen(Widget* parent, const Walkers &walkers );
  virtual ~InfoBoxCitizen();
};

#endif
