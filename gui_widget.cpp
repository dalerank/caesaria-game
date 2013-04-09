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


#include "gui_widget.hpp"

#include <iostream>
#include <SDL.h>

#include "exception.hpp"
#include "pic_loader.hpp"
#include "sdl_facade.hpp"
#include "gui_paneling.hpp"
#include "oc3_positioni.h"
#include "picture.hpp"
#include "custom_event.hpp"


WidgetEvent::WidgetEvent()
{
   _eventType = WE_NONE;
}

WidgetEvent WidgetEvent::NewGameEvent()
{
   WidgetEvent res;
   res._eventType = WE_NewGame;
   return res;
}

WidgetEvent WidgetEvent::LoadGameEvent()
{
   WidgetEvent res;
   res._eventType = WE_LoadGame;
   return res;
}

WidgetEvent WidgetEvent::SaveGameEvent()
{
   WidgetEvent res;
   res._eventType = WE_SaveGame;
   return res;
}

WidgetEvent WidgetEvent::QuitGameEvent()
{
   WidgetEvent res;
   res._eventType = WE_QuitGame;
   return res;
}

WidgetEvent WidgetEvent::OptionsMenuEvent()
{
   WidgetEvent res;
   res._eventType = WE_OptionsMenu;
   return res;
}

WidgetEvent WidgetEvent::BuildMenuEvent(const BuildMenuType menuType)
{
   WidgetEvent res;
   res._eventType = WE_BuildMenu;
   res._buildMenuType = menuType;
   return res;
}

WidgetEvent WidgetEvent::BuildingEvent(const BuildingType buildingType)
{
   WidgetEvent res;
   res._eventType = WE_Building;
   res._buildingType = buildingType;
   return res;
}

WidgetEvent WidgetEvent::InGameMenuEvent()
{
   WidgetEvent res;
   res._eventType = WE_InGameMenu;
   return res;
}

WidgetEvent WidgetEvent::ClearLandEvent()
{
   WidgetEvent res;
   res._eventType = WE_ClearLand;
   return res;
}


Widget::Widget()
{
   _x = 0;
   _y = 0;
   _width = 0;
   _height = 0;
   _listener = NULL;
   _parent = NULL;
   _underMouse = false;
}

Widget::~Widget()
{
  
}

void Widget::drawPicture( const Picture &picture, const int dx, const int dy)
{
   GfxEngine &engine = GfxEngine::instance();
   engine.drawPicture(picture, dx+_x, dy+_y);
}

void Widget::setPosition(const int x, const int y)
{
   _x = x;
   _y = y;
}

void Widget::setPosition( const Point& pos )
{
    _x = pos.getX();
    _y = pos.getY();
}

int Widget::getX() const
{
   return _x;
}

int Widget::getY() const
{
   return _y;
}

int Widget::getRelativeX(Widget* parent) const
{
   int res = getX();
   Widget *my_parent = getParent();
   while (my_parent != parent)
   {
      res += my_parent->getX();
      my_parent = my_parent->getParent();
   }
   return res;
}

int Widget::getRelativeY(Widget* parent) const
{
   int res = getY();
   Widget *my_parent = getParent();
   while (my_parent != parent)
   {
      res += my_parent->getY();
      my_parent = my_parent->getParent();
   }
   return res;
}

void Widget::setSize(const int width, const int height)
{
   _width = width;
   _height = height;
}

int Widget::getWidth() const
{
   return _width;
}

int Widget::getHeight() const
{
   return _height;
}

void Widget::setListener(WidgetListener *listener)
{
   _listener = listener;
}

void Widget::callListener(const WidgetEvent &event)
{
   if (_listener != NULL)
   {
      _listener->handleWidgetEvent(event, this);
   }
}

Widget* Widget::getParent() const
{
   return _parent;
}

void Widget::setParent(Widget* parent)
{
   _parent = parent;
}

bool Widget::contains(const int x, const int y) const
{
   return (x >= _x && y >= _y && x < (_x+_width) && y < (_y+_height));
}

void Widget::onMouseEnter()
{
}

void Widget::onMouseExit()
{
}

void Widget::onMouseClick()
{
}

void Widget::handleEvent(SDL_Event &event)
{
   if (event.type == SDL_MOUSEMOTION)
   {
      // get button under the cursor
      bool underMouse = contains(event.motion.x, event.motion.y);
      if (underMouse && !_underMouse)
      {
         onMouseEnter();
      }
      else if (!underMouse && _underMouse)
      {
         onMouseExit();
      }
      _underMouse = underMouse;  // save the new state
   }
   else if (event.type == SDL_USEREVENT && event.user.code == SDL_USER_MOUSECLICK)
   {
      // mouse click
      SDL_USER_MouseClickEvent &uevent = *(SDL_USER_MouseClickEvent*)event.user.data1;

      // check if the mouse is on the widget (maybe the widget did not receive former mouseMove events)
      bool underMouse = contains(uevent.x, uevent.y);
      if (underMouse && ! _underMouse)
      {
         onMouseEnter();
      }
      else if (!underMouse && _underMouse)
      {
         onMouseExit();
      }
      _underMouse = underMouse;  // save the new state

      if (uevent.button == SDL_BUTTON_LEFT)
      {
         if (_underMouse)
         {
            onMouseClick();
         }
      }
   }
}

WidgetGroup::WidgetGroup()
{
}

void WidgetGroup::drawChildren(const int dx, const int dy)
{
   for (std::list<Widget*>::iterator itWidget = _widget_list.begin(); itWidget != _widget_list.end(); ++itWidget)
   {
      Widget &widget = **itWidget;
      widget.draw(_x+dx, _y+dy);
   }
}

std::list<Widget*> &WidgetGroup::get_widget_list()
{
   return _widget_list;
}

void WidgetGroup::add_widget(Widget &widget)
{
   _widget_list.push_back(&widget);
   widget.setListener(_listener);
   widget.setParent(this);
}

void WidgetGroup::setListener(WidgetListener *listener)
{
   Widget::setListener(listener);
   for (std::list<Widget*>::iterator itWidget = _widget_list.begin(); itWidget != _widget_list.end(); ++itWidget)
   {
      Widget &widget = **itWidget;
      widget.setListener(_listener);
   }
}

Widget* WidgetGroup::get_widget_at(const int x, const int y)
{
   Widget *res = NULL;

   for (std::list<Widget*>::iterator itWidget = _widget_list.begin(); itWidget != _widget_list.end(); ++itWidget)
   {
      // test all widgets
      Widget &widget = **itWidget;
      if (widget.contains(x, y))
      {
         res = &widget;
         break;
      }
   }

   return res;
}

void WidgetGroup::handleEvent(SDL_Event &event)
{
   SDL_Event event2 = event;
   SDL_USER_MouseClickEvent uevent_click;
   SDL_USER_MouseDragEvent uevent_drag;

   // translate events relative to (x, y)
   if (event.type == SDL_MOUSEMOTION)
   {
      event2.motion.x -= _x;
      event2.motion.y -= _y;
   }
   else if (event.type == SDL_USEREVENT && event.user.code == SDL_USER_MOUSECLICK)
   {
      // mouse click
      uevent_click = *(SDL_USER_MouseClickEvent*)event.user.data1;
      uevent_click.x -= _x;
      uevent_click.y -= _y;
      event2.user.data1 = &uevent_click;
   }
   else if (event.type == SDL_USEREVENT && event.user.code == SDL_USER_MOUSEDRAG)
   {
      // mouse drag
      uevent_drag = *(SDL_USER_MouseDragEvent*)event.user.data1;
      uevent_drag.x1 -= _x;
      uevent_drag.y1 -= _y;
      uevent_drag.x2 -= _x;
      uevent_drag.y2 -= _y;
      event2.user.data1 = &uevent_drag;
   }

   for (std::list<Widget*>::iterator itWidget = _widget_list.begin(); itWidget != _widget_list.end(); ++itWidget)
   {
      // propagate the event to every widget
      Widget &widget = **itWidget;
      widget.handleEvent(event2);
   }
}


ImageIcon::ImageIcon()
{
   _picture = NULL;
}

void ImageIcon::draw(const int dx, const int dy)
{
   drawPicture(getPicture(), dx, dy);
}

Picture &ImageIcon::getPicture()
{
   return *_picture;
}

void ImageIcon::setPicture(Picture &pic)
{
   _picture = &pic;
   setSize(pic.get_surface()->w, pic.get_surface()->h);
}


TextIcon::TextIcon()
{
   _bgPicture = NULL;
   _picture = NULL;
   _text = "";
   _font = NULL;
   _text_x = 0;
   _text_y = 0;
}

void TextIcon::draw(const int dx, const int dy)
{
   drawPicture(getPicture(), dx, dy);
}

Picture &TextIcon::getPicture()
{
   if (_picture == NULL)
   {
      repaint();
   }
   return *_picture;
}

void TextIcon::invalidatePicture()
{
   if (_picture != NULL)
   {
      GfxEngine::instance().unload_picture(*_picture);
   }
   _picture = NULL;
}

void TextIcon::setBgPicture(Picture *pic)
{
   _bgPicture = pic;
   invalidatePicture();
}

void TextIcon::fitSize()
{
   if (_bgPicture == NULL) THROW("Missing BgPicture");
   setSize(_bgPicture->get_surface()->w, _bgPicture->get_surface()->h);
}

void TextIcon::setFont( const Font& font )
{
   _font = const_cast< Font* >( &font );
}

void TextIcon::setText(const std::string &text)
{
   _text = text;
   invalidatePicture();
}

void TextIcon::setTextPosition(const int x, const int y)
{
   _text_x = x;
   _text_y = y;
   invalidatePicture();
}

int TextIcon::getTextX() const
{
   return _text_x;
}

int TextIcon::getTextY() const
{
   return _text_y;
}

void TextIcon::repaint()
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   _picture = &sdlFacade.createPicture(getWidth(), getHeight());
   SDL_SetAlpha(_picture->get_surface(), 0, 0);  // remove surface alpha
   if (_bgPicture != NULL)
   {
      sdlFacade.drawPicture(*_bgPicture, *_picture, 0, 0);   // draw the background
   }
   sdlFacade.drawText(*_picture, _text, _text_x, _text_y, *_font);
   GfxEngine::instance().load_picture(*_picture);
}


MenuButton::MenuButton()
{
   _state = B_NORMAL;
   _group = NULL;
}

void MenuButton::setEvent(const WidgetEvent &event)
{
   _event = event;
}

WidgetEvent MenuButton::getEvent()
{
   return _event;
}


void MenuButton::setState(const ButtonState state)
{
   _state = state;
   if (_group != NULL)
   {
      _group->onStateChange(*this);
   }
}

ButtonState MenuButton::getState() const
{
   return _state;
}

void MenuButton::setExclusiveButtonGroup(ExclusiveButtonGroup *group)
{
   _group = group;
}

void MenuButton::onMouseEnter()
{
   if (_state == B_NORMAL)
   {
      setState(B_HOVER);
   }
}

void MenuButton::onMouseExit()
{
   if (_state == B_HOVER)
   {
      setState(B_NORMAL);
   }
}

void MenuButton::onMouseClick()
{
   if (_state == B_SELECTED)
   {
      setState(B_NORMAL);
   }
   else if (_state != B_UNAVAILABLE)
   {
      setState(B_SELECTED);
      callListener(_event);
   }
}



ExclusiveButtonGroup::ExclusiveButtonGroup()
{
   _selected_button = NULL;
}

void ExclusiveButtonGroup::onStateChange(MenuButton &button)
{
   if (button.getState() == B_NORMAL && &button==_selected_button)
   {
      _selected_button = NULL;
   }
   else if (button.getState() == B_SELECTED)
   {
      // a button gets selected

      if (_selected_button != NULL)
      {
         // unselect the old button
         _selected_button->setState(B_NORMAL);
      }

      _selected_button = &button;
   }
}

MenuButton *ExclusiveButtonGroup::getSelectedButton()
{
   return _selected_button;
}

ImageButton::ImageButton()
{
   _normal_picture = NULL;
   _hover_picture = NULL;
   _selected_picture = NULL;
   _unavailable_picture = NULL;
}

void ImageButton::draw(const int dx, const int dy)
{
   drawPicture(getPicture(), dx, dy);
}

void ImageButton::setNormalPicture(Picture& pic)
{
   _normal_picture = &pic;
   setSize(pic.get_surface()->w, pic.get_surface()->h);
}

void ImageButton::setHoverPicture(Picture& pic)
{
   _hover_picture = &pic;
}

void ImageButton::setSelectedPicture(Picture& pic)
{
   _selected_picture = &pic;
}

void ImageButton::setUnavailablePicture(Picture& pic)
{
   _unavailable_picture = &pic;
}

Picture& ImageButton::getPicture()
{
   Picture* res = NULL;

   switch (_state)
   {
   case B_NORMAL:
      res = _normal_picture;
      break;
   case B_HOVER:
      res = _hover_picture;
      break;
   case B_SELECTED:
      res = _selected_picture;
      break;
   case B_UNAVAILABLE:
      res = _unavailable_picture;
      break;
   }

   if (res == NULL)
   {
      THROW("Undefined picture for button state!");
   }

   return *res;
}


TextButton::TextButton()
{
   _normal_picture = NULL;
   _hover_picture = NULL;
   _selected_picture = NULL;
   _unavailable_picture = NULL;
   _text = "";
}

void TextButton::draw(const int dx, const int dy)
{
   drawPicture(getPicture(), dx, dy);
}

void TextButton::setText(const std::string &text)
{
   _text = text;
}

std::string TextButton::getText() const
{
   return _text;
}

void TextButton::init_pictures()
{
   SdlFacade &sdlFacade = SdlFacade::instance();
   _normal_picture = &sdlFacade.createPicture(_width, _height);
   _hover_picture = &sdlFacade.createPicture(_width, _height);
   _selected_picture = &sdlFacade.createPicture(_width, _height);
   _unavailable_picture = &sdlFacade.createPicture(_width, _height);

   // draw button background
   GuiPaneling::instance().draw_basic_text_button(*_normal_picture, 0, 0, _width, 22);
   GuiPaneling::instance().draw_basic_text_button(*_hover_picture, 0, 0, _width, 25);
   GuiPaneling::instance().draw_basic_text_button(*_selected_picture, 0, 0, _width, 25);
   GuiPaneling::instance().draw_basic_text_button(*_unavailable_picture, 0, 0, _width, 22);

   // draw button text
   Font &font = FontCollection::instance().getFont(FONT_2);
   Font &fontRed = FontCollection::instance().getFont(FONT_2_RED);
   int text_width = 0;
   int text_height = 0;
   sdlFacade.getTextSize(font, _text, text_width, text_height);
   sdlFacade.drawText(*_normal_picture, _text, (_width-text_width)/2, 0, font);
   sdlFacade.drawText(*_hover_picture, _text, (_width-text_width)/2, 0, font);
   sdlFacade.drawText(*_selected_picture, _text, (_width-text_width)/2, 0, fontRed);
   sdlFacade.drawText(*_unavailable_picture, _text, (_width-text_width)/2, 0, font);
}

Picture& TextButton::getPicture()
{
   Picture* res = NULL;

   switch (_state)
   {
   case B_NORMAL:
      res = _normal_picture;
      break;
   case B_HOVER:
      res = _hover_picture;
      break;
   case B_SELECTED:
      res = _selected_picture;
      break;
   case B_UNAVAILABLE:
      res = _unavailable_picture;
      break;
   }

   if (res == NULL)
   {
      THROW("Undefined picture for button state!");
   }

   return *res;
}


ImageTextButton::ImageTextButton() {}

void ImageTextButton::init_pictures()
{
   SdlFacade &sdlFacade = SdlFacade::instance();
   // copy the picture background
   // _normal_picture = &sdlFacade.copyPicture(*_normal_picture);
   // _hover_picture = &sdlFacade.copyPicture(*_hover_picture);
   // _selected_picture = &sdlFacade.copyPicture(*_selected_picture);
   // _unavailable_picture = &sdlFacade.copyPicture(*_unavailable_picture);

   // draw button text
   Font &font = FontCollection::instance().getFont(FONT_2);
   Font &fontRed = FontCollection::instance().getFont(FONT_2_RED);
   int text_width = 0;
   int text_height = 0;
   sdlFacade.getTextSize(font, _text, text_width, text_height);
   int text_x = (_width-text_width)/2;
   int text_y = (_height-text_height)/2+1;
   sdlFacade.drawText(*_normal_picture, _text, text_x, text_y, font);
   sdlFacade.drawText(*_hover_picture, _text, text_x, text_y, font);
   sdlFacade.drawText(*_selected_picture, _text, text_x, text_y, fontRed);
   sdlFacade.drawText(*_unavailable_picture, _text, text_x, text_y, font);
}

void ImageTextButton::setNormalPicture(Picture& pic)
{
   _normal_picture = &pic;
   setSize(pic.get_surface()->w, pic.get_surface()->h);
}

void ImageTextButton::setHoverPicture(Picture& pic)
{
   _hover_picture = &pic;
}

void ImageTextButton::setSelectedPicture(Picture& pic)
{
   _selected_picture = &pic;
}

void ImageTextButton::setUnavailablePicture(Picture& pic)
{
   _unavailable_picture = &pic;
}


BuildButton::BuildButton()
{
   _cost = 0;
}

void BuildButton::setCost(const int cost)
{
   _cost = cost;
}

int BuildButton::getCost() const
{
   return _cost;
}

void BuildButton::setCostX(const int x)
{
   _costX = x;
}

void BuildButton::init_pictures()
{
   SdlFacade &sdlFacade = SdlFacade::instance();
   _normal_picture = &sdlFacade.createPicture(_width, _height);
   _hover_picture = &sdlFacade.createPicture(_width, _height);
   _selected_picture = &sdlFacade.createPicture(_width, _height);
   _unavailable_picture = &sdlFacade.createPicture(_width, _height);

   // draw button background
   GuiPaneling::instance().draw_basic_text_button(*_normal_picture, 0, 0, _width, 22);
   GuiPaneling::instance().draw_basic_text_button(*_hover_picture, 0, 0, _width, 25);
   GuiPaneling::instance().draw_basic_text_button(*_selected_picture, 0, 0, _width, 25);
   GuiPaneling::instance().draw_basic_text_button(*_unavailable_picture, 0, 0, _width, 22);

   // draw button text
   Font &font = FontCollection::instance().getFont(FONT_2);
   Font &fontRed = FontCollection::instance().getFont(FONT_2_RED);
   int text_width = 0;
   int text_height = 0;
   sdlFacade.getTextSize(font, _text, text_width, text_height);
   int text_x = (_costX-text_width+5)/2;  // position of the text
   sdlFacade.drawText(*_normal_picture, _text, text_x, 0, font);
   sdlFacade.drawText(*_hover_picture, _text, text_x, 0, font);
   sdlFacade.drawText(*_selected_picture, _text, text_x, 0, fontRed);
   sdlFacade.drawText(*_unavailable_picture, _text, text_x, 0, font);

   // draw cost
   if (_cost >= 0)
   {
      std::stringstream cost_ss;
      cost_ss << _cost << "Dn";

      std::string cost_str = cost_ss.str();
      sdlFacade.drawText(*_normal_picture, cost_str, _costX, 0, font);
      sdlFacade.drawText(*_hover_picture, cost_str, _costX, 0, font);
      sdlFacade.drawText(*_selected_picture, cost_str, _costX, 0, fontRed);
      sdlFacade.drawText(*_unavailable_picture, cost_str, _costX, 0, font);
   }
}


