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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_EVENT_PLAYSOUND_H_INCLUDE_
#define _CAESARIA_EVENT_PLAYSOUND_H_INCLUDE_

#include "event.hpp"
#include "sound/constants.hpp"

namespace events
{

class PlaySound : public GameEvent
{
public:
  static GameEventPtr create(const std::string& rc, int index, int volume, audio::SoundType type=audio::ambient, bool force=false );
  static GameEventPtr create(const std::string& filename, int volume, audio::SoundType type=audio::ambient, bool force=false );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  PlaySound();

  std::string _sound;
  int _volume;
  bool _force;
  audio::SoundType _type;
};

}

#endif //_CAESARIA_EVENT_PLAYSOUND_H_INCLUDE_
