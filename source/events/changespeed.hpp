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
// along with Caesa    std::string text = rIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_CHANGE_SPEED_EVENT_H_INCLUDE_
#define _CAESARIA_CHANGE_SPEED_EVENT_H_INCLUDE_

#include "event.hpp"

class Game;

namespace events
{

class Pause : public GameEvent
{
public:
  typedef enum { toggle, pause, play, hidepause, hideplay, unknown } Mode;
  static GameEventPtr create( Mode mode );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  Pause();
  Mode _mode;
};

class Step : public GameEvent
{
public:
  static GameEventPtr create(unsigned int count);

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  Step(unsigned int count);
  unsigned int _count;
};

class ChangeSpeed : public GameEvent
{
public:
  static GameEventPtr create( int value );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec( Game& game, unsigned int ) const;

private:
  ChangeSpeed();
  int _value;
};

} //end namespace events
#endif //_CAESARIA_CHANGE_SPEED_EVENT_H_INCLUDE_
