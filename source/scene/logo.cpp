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

using namespace gfx;

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
  Engine& engine = Engine::instance();

  _d->bgPicture = Picture::load("logo", 1);

  // center the bgPicture on the screen
  Size s = (engine.screenSize() - _d->bgPicture.size()) / 2;
  _d->bgPicture.setOffset( Point( s.width(), -s.height() ) );
}

void SplashScreen::draw()
{
  Engine& engine = Engine::instance();

  engine.drawPicture( _d->bgPicture, 0, 0);
}

void SplashScreen::fadeOut()
{
  Engine& engine = Engine::instance();
  engine.loadPicture( _d->bgPicture );

  PictureRef pf;
  pf.init( engine.screenSize() );
  int offset = 3;

#ifdef CAESARIA_PLATFORM_ANDROID
  offset = 12;
#endif

  for( int k=0; k < 0xff; k+=offset )
  {
    engine.startRenderFrame();
    pf->fill( NColor(k, 0, 0, 0), Rect() );
    engine.drawPicture( _d->bgPicture, 0, 0);
    engine.drawPicture( *pf, 0, 0);
    //engine.delay( 1 );
    engine.endRenderFrame();
  }
}

int SplashScreen::result() const { return 0; }

}//end namespace scene
