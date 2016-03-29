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

#ifndef __CAESARIA_WIDGET_HELPER_H_INCLUDED__
#define __CAESARIA_WIDGET_HELPER_H_INCLUDED__

#include "widget.hpp"

namespace gui
{

class SoundMuter : public Widget
{
public:
  SoundMuter(Widget* parent,int value=-1);
  ~SoundMuter();

  void setVolume(int value);
private:
  std::map<int,int> _states;
};

class SoundEmitter : public Widget
{
public:
  SoundEmitter(Widget* parent, const std::string& sound="", int volume=0, int type=0);
  ~SoundEmitter();
  void assign(const std::string& sampleName, int volume, int type);

private:
  std::string _sample;
};

}//end namesapce gui
#endif //__CAESARIA_WIDGET_HELPER_H_INCLUDED__
