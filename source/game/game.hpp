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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_APPLICATION_H_INCLUDED__
#define __CAESARIA_APPLICATION_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "core/signals.hpp"
#include "scene/base.hpp"
#include "gfx/engine.hpp"
#include "scene/constants.hpp"
#include "enums.hpp"

class Scene;

class Game
{
public:
  Game();
  ~Game();

  void save(std::string filename) const;
  bool load(std::string filename);

  void initialize();

  bool exec();

  void reset();
  void clear();

  PlayerPtr player() const;
  PlayerCityPtr city() const;
  world::EmpirePtr empire() const;
  gui::Ui* gui() const;
  gfx::Engine* engine() const;
  scene::Base* scene() const;
  DateTime date() const;

  void setPaused( bool value );
  bool isPaused() const;

  void step( unsigned int count = 1);

  void play();
  void pause();

  void changeTimeMultiplier(int percent);
  void setTimeMultiplier(int percent);
  int timeMultiplier() const;
  void setNextScreen( scene::ScreenType screen);

public signals:
  Signal1<std::string>& onSaveAccepted();

private:

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //__CAESARIA_APPLICATION_H_INCLUDED__
