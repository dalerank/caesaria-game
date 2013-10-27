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


#include "screen_wait.hpp"

#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "gfx/picture.hpp"

class ScreenWait::Impl
{
public:
	Picture bgPicture;
	GfxEngine* engine;
};

ScreenWait::ScreenWait() : _d( new Impl )
{
}

ScreenWait::~ScreenWait() {}

void ScreenWait::initialize()
{
  GfxEngine& engine = GfxEngine::instance();

  _d->bgPicture = Picture::load("c3title", 1);

  // center the bgPicture on the screen
  Size s = (engine.getScreenSize() - _d->bgPicture.getSize()) / 2;
  _d->bgPicture.setOffset( s.getWidth(), -s.getHeight() );
}

void ScreenWait::draw()
{
  GfxEngine& engine = GfxEngine::instance();

  engine.drawPicture( _d->bgPicture, 0, 0);
}

int ScreenWait::getResult() const
{
	return 0;
}
