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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_ADVISORSWINDOW_H_INCLUDED__
#define __CAESARIA_ADVISORSWINDOW_H_INCLUDED__

#include "window.hpp"
#include "core/signals.hpp"
#include "game/predefinitions.hpp"

namespace gui
{

class PushButton;

namespace advisorwnd
{

class ParlorModel;
class Parlor : public Window
{
public:
  struct Item
  {
    Advisor type;
    gfx::ImgID pic;
    std::string tooltip;
  };
  typedef std::vector<Item> Items;

  static Parlor* create(Widget* parent, int id, const Advisor type, ParlorModel* model);

  // draw on screen
  virtual void draw( gfx::Engine& engine );
  virtual bool onEvent(const NEvent& event);

  void showAdvisor(Advisor type);
  void setModel( ParlorModel* model );

public signals:
  Signal1<Advisor> onSwitchAdvisor;

protected:
  ScopedPtr<ParlorModel> _model;

  Parlor( Widget* parent, int id );
  PushButton* _addButton(Advisor advisorName, int picId, std::string="" );
  void _initButtons();
};

class ParlorModel
{
public:
  ParlorModel( PlayerCityPtr city );
  void setParent( Widget* parlor );
  Parlor::Items items();

public slots:
  void switchAdvisor( Advisor advisor );
  void sendMoney2City( int money );
  void showEmpireMapWindow();

protected:
  __DECLARE_IMPL(ParlorModel)
};


}//end namespace advisorwnd

}//end namespace gui
#endif //__CAESARIA_ADVISORSWINDOW_H_INCLUDED__
