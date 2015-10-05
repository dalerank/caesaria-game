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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "gameautopause.hpp"
#include "events/changespeed.hpp"
#include "gui/widget.hpp"

using namespace events;

class GameAutoPauseWidget : public gui::Widget
{
public:
  GameAutoPause locker;
  GameAutoPauseWidget( gui::Widget* parent )
    : gui::Widget( parent, -1, Rect() )
  {
    locker.activate();
  }
};

GameAutoPause::GameAutoPause()
{
  _activated = false;
}

void GameAutoPause::activate()
{
  GameEventPtr e = Pause::create( Pause::hidepause );
  e->dispatch();
  _activated = true;
}

void GameAutoPause::insertTo( gui::Widget *parent)
{
  new GameAutoPauseWidget( parent );
}

GameAutoPause::~GameAutoPause()
{
  if( !_activated )
    return;

  GameEventPtr e = Pause::create( Pause::hideplay );
  e->dispatch();
}
