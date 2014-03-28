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

#include "base.hpp"

#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "core/event.hpp"
#include "core/eventconverter.hpp"
#include "core/time.hpp"

namespace scene
{

Base::Base()
{
  _isStopped = false;
}

Base::~Base() {}

void Base::drawFrame( GfxEngine &engine )
{
   engine.startRenderFrame();
   
   draw();
   
   engine.endRenderFrame();
}

void Base::handleEvent( NEvent& event) {}
void Base::afterFrame() {}
void Base::stop(){ _isStopped = true;}

void Base::update( GfxEngine &engine )
{
  static unsigned int lastclock = DateTime::getElapsedTime();
  static unsigned int currentclock = 0;  
  static unsigned int ref_delay = 1000 / 20;  // 20fps

#ifdef CAESARIA_PLATFORM_ANDROID
  ref_delay = 0;
#endif

  drawFrame( engine );
  afterFrame();

  NEvent nEvent;
  while( engine.haveEvent( nEvent )  )
  {
    handleEvent( nEvent );
  }

  // sets a fix frameRate
  currentclock = DateTime::getElapsedTime();
  unsigned int delay = math::clamp<int>( ref_delay - (currentclock - lastclock), 0, ref_delay );

  lastclock = currentclock;
  engine.delay( delay );
}

bool Base::isStopped() const{  return _isStopped;}
bool Base::installEventHandler(EventHandlerPtr) { return false; }

}//end namespace scene
