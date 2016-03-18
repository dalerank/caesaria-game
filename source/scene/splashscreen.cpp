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
// Copyright 2012-2014 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerank@gmail.com

#include "splashscreen.hpp"

#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "gfx/picture.hpp"
#include "gfx/pictureconverter.hpp"
#include "core/color_list.hpp"
#include "font/font.hpp"
#include "core/osystem.hpp"
#include "core/gettext.hpp"
#include "scripting/core.hpp"
#include "core/variant.hpp"
#include "core/utils.hpp"

using namespace gfx;

namespace scene
{

class SplashScreen::Impl
{
public:
  Picture background;
  Picture textPic;
  Picture fadetx;
  std::string lastText;
  std::string text, prefix;
  StringArray devText;

public:
  void fade(Engine &engine, Picture &pic, bool out, int offset);
};

SplashScreen::SplashScreen() : _d( new Impl ) {}

SplashScreen::~SplashScreen() {}

void SplashScreen::initialize()
{
  Engine& engine = Engine::instance();

  _d->textPic = Picture( Size( 800, 30 ), 0, true );
  _d->fadetx = Picture( engine.screenSize(), 0, true );
  _d->fadetx.fill( NColor(0xff, 0, 0, 0), Rect() );

  script::Core::execFunction("OnStartSplashScreen");  
}

void SplashScreen::draw(Engine& engine)
{
  if( _d->text != _d->lastText )
  {
    Font textFont = Font::create("FONT_2_WHITE");
    Rect textRect = textFont.getTextRect( _d->text, Rect( Point(), _d->textPic.size() ), align::center, align::center );

    _d->textPic.fill( ColorList::black );

    textFont.draw( _d->textPic, _d->text, textRect.left(), textRect.top(), false, true );

    _d->lastText = _d->text;
  }

  engine.draw( _d->background, 0, 0);
  engine.draw( _d->textPic, 0, 0 );
}

void SplashScreen::setBackground(const std::string &image, int index)
{
  Engine& engine = Engine::instance();
  _d->background.load(image)
                .withFallback(image, index)
                .withFallback("logo", 1);

  // center the background on the screen
  Size s = (engine.screenSize() - _d->background.size()) / 2;
  _d->background.setOffset( Point( s.width(), -s.height() ) );

  _d->textPic.setOffset( Point( (engine.screenSize().width() - _d->textPic.width()) / 2,
                                  _d->background.offset().y() - _d->background.height() - 5 ) );
}

void SplashScreen::Impl::fade( Engine& engine, Picture& pic, bool out, int offset )
{
  int start = out ? 0 : 0xff;
  int stop = out ? 0xff : 0;
  offset *= (out ? 1 : -1);

  for( int k=start; out ? k < stop : k > stop ; k+=offset )
  {
    engine.frame().start();
    fadetx.setAlpha( k );
    fadetx.update();
    engine.draw( pic, 0, 0);
    engine.draw( fadetx, 0, 0);
    engine.frame().finish();
  }
}

void SplashScreen::exitScene(bool devText)
{
#ifdef DEBUG
  devText = false;
#endif
  script::Core::execFunction("OnExitSplashScreen");

  Engine& engine = Engine::instance();

  int offset = 3;

  Font textFont = Font::create("FONT_3") ;

  if( OSystem::isAndroid() )
  {
    offset = 12;
    textFont = Font::create("FONT_4");
  }

  _d->fade( engine, _d->background, true, offset );
  _d->textPic = Picture( engine.screenSize(), 0, true );
  _d->textPic.fill( ColorList::black, Rect( Point( 0, 0 ), _d->textPic.size() ) );

  if (devText)
  {
    script::Core::execFunction("OnDevtextSplashScreen");

    int offset = 0;
    for( const auto& line : _d->devText )
    {
      Rect textRect = textFont.getTextRect( line, Rect( Point(), _d->textPic.size() ), align::center, align::center );
      bool defaultColor = ( (line)[0] != ' ');
      textFont.setColor( defaultColor ? ColorList::dodgerBlue : ColorList::indianRed );
      textFont.draw( _d->textPic, line, textRect.left(), textRect.top() + offset, false, true );
      offset += 20;
    }

    _d->fade( engine, _d->textPic, false, offset );
    engine.delay( 3000 );
  }
  _d->fade( engine, _d->textPic, true, offset );
}

void SplashScreen::setText(std::string text)
{
  _d->text = _d->prefix + " " + _( text );
  update( Engine::instance() );
}

void SplashScreen::setOption(const std::string & name, Variant value)
{
  if (name == "background")
  {
    setBackground(value.toString(), 1);
  }
  else if (name == "tooltip")
  {
    setText(value.toString());
  }
  else if (name == "dev-text")
  {
    _d->devText = utils::split( value.toString(), "\n");
  }
}

void SplashScreen::setPrefix(std::string prefix) { _d->prefix = _( prefix ); }
int SplashScreen::result() const { return 0; }

}//end namespace scene
