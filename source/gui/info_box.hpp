// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _CAESARIA_INFOBOXSIMPLE_H_INCLUDE_
#define _CAESARIA_INFOBOXSIMPLE_H_INCLUDE_

#include <string>
#include <list>

#include "widget.hpp"
#include "gfx/picture.hpp"
#include "building/factory.hpp"
#include "building/service.hpp"
#include "game/predefinitions.hpp"

namespace gui
{

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

  virtual void setupUI(const VariantMap &ui);

protected:
  virtual void _afterCreate() {}
  Label* _getTitle();
  Label* _getBlackFrame();
  virtual void _updateWorkersLabel( const Point& pos, int picId, int need, int have );

  class Impl;
  ScopedPtr< Impl > _d;
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
  
  void drawGood(const Good::Type &goodType, int, int);
  void showSpecialOrdersWindow();

private:
  GranaryPtr _granary;
};

// info box about a fontain
class InfoBoxFontain : public InfoBoxSimple
{
public:
   InfoBoxFontain( Widget* parent, const Tile& tile );
   virtual ~InfoBoxFontain();
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
};

class InfoBoxBuilding : public InfoBoxSimple
{
public:
   InfoBoxBuilding( Widget* parent, const Tile& tile );
};

// Simple info box with static text on plain background
class InfoBoxCitizen : public InfoBoxSimple
{
public:
  InfoBoxCitizen(Widget* parent, const WalkerList& walkers );
  virtual ~InfoBoxCitizen();
};

class InfoBoxText : public InfoBoxSimple
{
public:
  InfoBoxText(Widget* parent, const std::string& title, const std::string& message );
  virtual ~InfoBoxText();
};

}//end namespace gui
#endif //_CAESARIA_INFOBOXSIMPLE_H_INCLUDE_
