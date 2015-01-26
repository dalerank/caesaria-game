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

#ifndef _CAESARIA_SCENE_BASE_H_INCLUDE_
#define _CAESARIA_SCENE_BASE_H_INCLUDE_

#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"

struct NEvent;

namespace gfx
{
  class Engine;
  class Camera;
}

namespace scene
{

class EventHandler : public ReferenceCounted
{
public:
  virtual void handleEvent( NEvent& event ) = 0;
  virtual bool finished() const = 0;
};
typedef SmartPtr< EventHandler >  EventHandlerPtr;

class Base
{
public:
  virtual ~Base();

  virtual void handleEvent( NEvent& event) {}
  virtual void draw() = 0;

  // this method is executed after every frame. default: do nothing
  virtual void afterFrame() {}

  // runs the screen (main loop), returns _wevent
  void update( gfx::Engine& engine );
  void stop();

  // draws the complete frame
  void drawFrame( gfx::Engine &engine );

  virtual void initialize() = 0;
  virtual bool isStopped() const;

  virtual int result() const = 0;

  virtual bool installEventHandler( EventHandlerPtr );
protected:
  Base();

  //WidgetEvent _wevent;  // event to pass to the main loop
  bool _isStopped;  // screen needs to stop its loop
  int _delayTicks;
};

}//end namespace scene

#endif //_CAESARIA_SCENE_BASE_H_INCLUDE_
