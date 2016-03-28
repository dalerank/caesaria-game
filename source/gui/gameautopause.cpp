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
// Copyright 2012-2016 Dalerank, dalerankn8@gmail.com

#include "gameautopause.hpp"
#include "events/changespeed.hpp"
#include "gui/widget.hpp"
#include "widget_factory.hpp"

using namespace events;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(GameAutoPauseWidget)

GameAutoPauseWidget::GameAutoPauseWidget(Widget* parent)
  : gui::Widget( parent, -1, Rect() )
{
  _activated = false;
}

void GameAutoPauseWidget::activate()
{
  if (!_activated)
  {
    events::dispatch<Pause>(Pause::hidepause);
    _activated = true;
  }
}

void GameAutoPauseWidget::insertTo(Widget *parent)
{
  auto& pause = parent->add<GameAutoPauseWidget>();
  pause.activate();
}

GameAutoPauseWidget::~GameAutoPauseWidget()
{
  if (!_activated)
    return;

  events::dispatch<Pause>(Pause::hideplay);
}

}// end namespace gui
