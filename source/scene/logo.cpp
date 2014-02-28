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
// Copyright 2012-2013 Dalerank, dalerank@gmail.com

#include "logo.hpp"

#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "gfx/picture.hpp"
#include "gfx/pictureconverter.hpp"
#include "core/color.hpp"

namespace scene
{

class SplashScreen::Impl
{
public:
	Picture bgPicture;	
};

SplashScreen::SplashScreen() : _d( new Impl ) {}

SplashScreen::~SplashScreen() {}

void SplashScreen::initialize()
{
  GfxEngine& engine = GfxEngine::instance();

  _d->bgPicture = Picture::load("c3title", 2);

  // center the bgPicture on the screen
  Size s = (engine.getScreenSize() - _d->bgPicture.getSize()) / 2;
  _d->bgPicture.setOffset( Point( s.width(), -s.height() ) );
}

void SplashScreen::draw()
{
  GfxEngine& engine = GfxEngine::instance();

  engine.drawPicture( _d->bgPicture, 0, 0);
}

void SplashScreen::fadeOut()
{
  GfxEngine& engine = GfxEngine::instance();
  engine.loadPicture( _d->bgPicture );

  PictureRef pf;
  pf.init( engine.getScreenSize() );
  for( int k=0; k < 0xff; k+=3 )
  {
    engine.startRenderFrame();
    pf->fill( NColor(k, 0, 0, 0), Rect() );
    engine.drawPicture( _d->bgPicture, 0, 0);
    engine.drawPicture( *pf, 0, 0);
    //engine.delay( 1 );
    engine.endRenderFrame();
  }
}

int SplashScreen::getResult() const { return 0; }

}//end namespace scene
