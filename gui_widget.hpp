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


#ifndef GUI_WIDGET_HPP
#define GUI_WIDGET_HPP

#include <picture.hpp>
#include <string>
#include <list>
#include <enums.hpp>


struct WidgetEvent
{
   WidgetEventType _eventType;

   BuildMenuType _buildMenuType;
   BuildingType _buildingType;

   WidgetEvent();

   static WidgetEvent NewGameEvent();
   static WidgetEvent LoadGameEvent();
   static WidgetEvent SaveGameEvent();
   static WidgetEvent QuitGameEvent();
   static WidgetEvent OptionsMenuEvent();
   static WidgetEvent BuildMenuEvent(const BuildMenuType menuType);
   static WidgetEvent BuildingEvent(const BuildingType buildingType);
   static WidgetEvent InGameMenuEvent();
   static WidgetEvent ClearLandEvent();
};


class Widget;
class WidgetListener
{
public:
   virtual void handleWidgetEvent(const WidgetEvent &event, Widget* widget) = 0;
};


// a graphical object
class Widget
{
public:
   Widget();

   // draw on screen
   virtual void draw(const int dx, const int dy) = 0;
   void drawPicture(Picture &picture, const int dx, const int dy);

   // listener=NULL => no listener
   virtual void setListener(WidgetListener *listener);
   void callListener(const WidgetEvent &event);

   void setParent(Widget *parent);
   Widget* getParent() const;

   void setPosition(const int x, const int y);
   void setSize(const int width, const int height);
   int getX() const;
   int getY() const;
   int getWidth() const;
   int getHeight() const;
   int getRelativeX(Widget* parent) const;  // relative to the given parent
   int getRelativeY(Widget* parent) const;  // relative to the given parent

   // event callbacks
   virtual void onMouseEnter();
   virtual void onMouseExit();
   virtual void onMouseClick();
   virtual void handleEvent(SDL_Event &event);

   // returns true if the given coordinate is inside the widget rectangle
   bool contains(const int x, const int y) const;

protected:
   int _x;
   int _y;
   int _width;
   int _height;
   Widget* _parent;  // parent widget

   WidgetListener *_listener;
   bool _underMouse;  // used to know if mouseEnter or mouseExit
};

// a graphic group, components are displayed in foreground
class WidgetGroup : public Widget
{
public:
   WidgetGroup();

   // draw on screen
   virtual void draw(const int dx, const int dy) = 0;
   void drawChildren(const int dx, const int dy);

   virtual void setListener(WidgetListener *listener);
   virtual void handleEvent(SDL_Event &event);

   std::list<Widget*>& get_widget_list();

   void add_widget(Widget &widget);

   // returns the widget under given coordinate, if any
   Widget *get_widget_at(const int x, const int y);

private:
   std::list<Widget*> _widget_list;
};


// an image with a position
class ImageIcon : public Widget
{
public:
   ImageIcon();

   // draw on screen
   virtual void draw(const int dx, const int dy);

   Picture &getPicture();
   void setPicture(Picture &pic);

private:
   Picture *_picture;
};


class TextIcon : public Widget
{
   // some text on an optional bgPicture
public:
   TextIcon();

   // draw on screen
   virtual void draw(const int dx, const int dy);

   Picture &getPicture();
   void setBgPicture(Picture *pic);
   void setFont(Font &font);
   void setText(const std::string &text);
   void fitSize();  // set size according to bgPicture
   void invalidatePicture();

   void setTextPosition(const int x, const int y);
   int getTextX() const;
   int getTextY() const;

protected:
   // repaint the picture with background + text
   void repaint();

private:
   Picture *_bgPicture;  // background
   std::string _text;
   Font *_font;

   // position of text, inside the widget
   int _text_x;
   int _text_y;

   Picture *_picture;   // cached final picture (background + text)
};


class MenuButton;
class ExclusiveButtonGroup
{
public:
   ExclusiveButtonGroup();

   void onStateChange(MenuButton &button);
   MenuButton *getSelectedButton();

private:
   MenuButton *_selected_button;
};


enum ButtonState { B_NORMAL, B_HOVER, B_SELECTED, B_UNAVAILABLE };

// a standard clickable button
class MenuButton : public Widget
{
public:
   MenuButton();

   // name is the action sent to the listener when clicked
   void setEvent(const WidgetEvent &event);
   WidgetEvent getEvent();

   void setState(const ButtonState state);
   ButtonState getState() const;
   void setExclusiveButtonGroup(ExclusiveButtonGroup *group);

   // event callbacks
   virtual void onMouseEnter();
   virtual void onMouseExit();
   virtual void onMouseClick();

protected:
   ExclusiveButtonGroup *_group;
   WidgetEvent _event;  // event sent when button clicked
   ButtonState _state;
};


class ImageButton : public MenuButton
{
public:
   ImageButton();

   // draw on screen
   virtual void draw(const int dx, const int dy);

   void setNormalPicture(Picture& pic);
   void setHoverPicture(Picture& pic);
   void setSelectedPicture(Picture& pic);
   void setUnavailablePicture(Picture& pic);

   // returns the right picture for the button state
   Picture& getPicture();

private:
   Picture *_normal_picture;
   Picture *_hover_picture;
   Picture *_selected_picture;
   Picture *_unavailable_picture;
};


// uses the standard button pictures
class TextButton : public MenuButton
{
public:
   TextButton();

   // draw on screen
   virtual void draw(const int dx, const int dy);

   void setText(const std::string &text);
   std::string getText() const;
   virtual void init_pictures();

   // returns the right picture for the button state
   Picture& getPicture();

protected:
   std::string _text;
   Picture *_normal_picture;
   Picture *_hover_picture;
   Picture *_selected_picture;
   Picture *_unavailable_picture;
};


// uses custom image backgrounds
class ImageTextButton : public TextButton
{
public:
   ImageTextButton();

   virtual void init_pictures();

   void setNormalPicture(Picture& pic);
   void setHoverPicture(Picture& pic);
   void setSelectedPicture(Picture& pic);
   void setUnavailablePicture(Picture& pic);
};


// used to display the building name and its cost
class BuildButton : public TextButton
{
public:
   BuildButton();

   // cost=-1 => no cost display
   void setCost(const int cost);
   int getCost() const;
   virtual void init_pictures();

   void setCostX(const int x);

private:
   int _cost;   // cost of the building
   int _costX;  // position of the "cost" text
};


#endif
