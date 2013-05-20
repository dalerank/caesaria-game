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
#include "oc3_house.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_service_building.hpp"
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

    Picture& getPictureGood(const GoodType& goodType);

    void setTitle( const std::string& title );
protected:
    void _resizeEvent();

    class Impl;
    ScopedPtr< Impl > _d;
};


// info box about a service building
class GuiInfoService : public GuiInfoBox
{
public:
  GuiInfoService( Widget* parent, ServiceBuildingPtr building);
  virtual void paint();

  void drawWorkers( int );
  void setText(const std::string& text);
private:
  Label* _dmgLabel;
  Label* _lbHelp;
  ServiceBuildingPtr _building;
};

class InfoBoxLand : public GuiInfoBox
{
public:
    InfoBoxLand( Widget* parent, Tile* tile );   
    void setText( const std::string& text );
    //bool onEvent(const NEvent& event);
private:
    void _paint();

    Label* _text;
};

class InfoBoxFreeHouse : public InfoBoxLand
{
public:
    InfoBoxFreeHouse( Widget* parent, Tile* tile );   

    //bool onEvent(const NEvent& event);
private:
    //void _paint();

};
// info box about a factory building
class GuiInfoFactory : public GuiInfoBox
{
public:
   GuiInfoFactory( Widget* parent, Factory &building);
   virtual void paint();

   void drawWorkers( int& );
   std::string getInfoText();

private:
   Factory *_building;
};


// info box about a granary
class GuiInfoGranary : public GuiInfoBox
{
public:
   GuiInfoGranary( Widget* parent, GranaryPtr building);
   virtual void paint();

   void drawWorkers( int );
   void drawGood(const GoodType &goodType, int&);

private:
   GranaryPtr _building;
};


// info box about a market
class GuiInfoMarket : public GuiInfoBox
{
public:
   GuiInfoMarket( Widget* parent, Market &building);
   virtual void paint();

   void drawWorkers( int );
   void drawGood(const GoodType &goodType, int&);

private:
   Market *_building;
};


// info box about a house
class InfoBoxHouse : public GuiInfoBox
{
public:
   InfoBoxHouse( Widget* paarent, HousePtr house);


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
   GuiBuilding( Widget* parent, Building &building);
   virtual void paint();

private:

   Building *_building;
};

#endif
