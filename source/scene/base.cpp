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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "base.hpp"

#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "core/event.hpp"
#include "core/eventconverter.hpp"
#include "core/timer.hpp"

namespace scene
{

//1000 / 30
#define DELAY_33_FPS 33

Base::Base()
{
  _isStopped = false;
  _delayTicks = 0;
}

Base::~Base() {}

void Base::drawFrame(gfx::Engine& engine )
{
   engine.startRenderFrame();
   
   draw();
   
   engine.endRenderFrame();
}

void Base::stop(){ _isStopped = true;}

void Base::update(gfx::Engine& engine )
{
  static unsigned int lastTimeUpdate = DebugTimer::ticks();

  drawFrame( engine );
  afterFrame();

  NEvent nEvent;
  while( engine.haveEvent( nEvent )  )
  {
    handleEvent( nEvent );
  }

  _delayTicks = DebugTimer::ticks() - lastTimeUpdate;
  if( _delayTicks < DELAY_33_FPS )
  {
    engine.delay( std::max<int>( DELAY_33_FPS - _delayTicks, 0 ) );
  }

  lastTimeUpdate = DebugTimer::ticks();
}

bool Base::isStopped() const{  return _isStopped;}
bool Base::installEventHandler(EventHandlerPtr) { return false; }

}//end namespace scene
