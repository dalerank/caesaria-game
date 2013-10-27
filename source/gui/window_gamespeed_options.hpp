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

#ifndef _OPENCAESAR3_WINDOW_GAMESPEED_OPTIONS_H_INCLUDE_
#define _OPENCAESAR3_WINDOW_GAMESPEED_OPTIONS_H_INCLUDE_

#include "widget.hpp"
#include "core/signals.hpp"

namespace gui
{

class GameSpeedOptionsWindow : public Widget
{
public:
  GameSpeedOptionsWindow( Widget* parent, int gameSpeed, int scrollSpeed );

  //! Деструктор
  virtual ~GameSpeedOptionsWindow(void);

  virtual bool onEvent(const NEvent &event);

public oc3_signals:
  Signal1<int>& onGameSpeedChange();
  Signal1<int>& onScrollSpeedChange();

private:
  void _update();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //_OPENCAESAR3_WINDOW_VIDEO_OPTIONS_H_INCLUDE_
