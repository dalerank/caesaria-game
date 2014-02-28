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
// Copyright 2012-2013 dalerank, dalerankn8@gmail.com


#ifndef __CAESARIA_APPLICATION_H_INCLUDED__
#define __CAESARIA_APPLICATION_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "core/signals.hpp"
#include "scene/base.hpp"

#include <string>

class GfxEngine;
class Scene;

class Game
{
public:
  Game();
  ~Game();

  void save(std::string filename) const;
  void load(std::string filename);

  void initialize();

  void exec();

  void reset();

  void initSound();
  void mountArchives();

  void setScreenMenu();
  void setScreenGame();

  PlayerPtr getPlayer() const;
  PlayerCityPtr getCity() const;
  world::EmpirePtr getEmpire() const;
  gui::GuiEnv* getGui() const;
  GfxEngine* getEngine() const;
  scene::Base* getScene() const;

  void setPaused( bool value );
  bool isPaused() const;

  void play();
  void pause();

  void changeTimeMultiplier(int percent);
  void setTimeMultiplier( int percent );
  int getTimeMultiplier() const;

public oc3_signals:
  Signal1<std::string>& onSaveAccepted();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //__CAESARIA_APPLICATION_H_INCLUDED__
