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


#ifndef _CAESARIA_INFOBOXSIMPLE_H_INCLUDE_
#define _CAESARIA_INFOBOXSIMPLE_H_INCLUDE_

#include <string>
#include <list>

#include "window.hpp"
#include "gfx/picture.hpp"
#include "objects/service.hpp"
#include "game/predefinitions.hpp"
#include "city/city.hpp"

namespace gui
{

class PushButton;
class Label;

namespace infobox
{

// base class for info boxes
class Simple : public Window
{
public:
  Simple( Widget* parent, const Rect& rect, const Rect& blackArea=Rect(), int id=-1 );
  virtual ~Simple();

  virtual void draw( gfx::Engine& engine );  // draw on screen

  virtual bool onEvent( const NEvent& event);

  virtual bool isPointInside(const Point& point) const;

  void setTitle( const std::string& title );
  virtual void setText( const std::string& text );

  bool isAutoPosition() const;
  void setAutoPosition( bool value );

  virtual void setupUI(const VariantMap &ui);
  virtual void setupUI(const vfs::Path& filename);

protected:
  virtual void _showHelp() {}
  gfx::Picture& _background();
  virtual void _afterCreate() {}
  Label* _lbTitleRef();
  Label* _lbTextRef();
  Label* _lbBlackFrameRef();
  PushButton* _btnExitRef();

  virtual void _updateWorkersLabel( const Point& pos, int picId, int need, int have );

  class Impl;
  ScopedPtr< Impl > _d;
};

class InfoboxBuilding : public Simple
{
public:
  InfoboxBuilding( Widget* parent, const gfx::Tile& tile );
};

}

}//end namespace gui
#endif //_CAESARIA_INFOBOXSIMPLE_H_INCLUDE_
