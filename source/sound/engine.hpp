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

#ifndef _CAESARIA_SOUND_ENGINE_H_INCLUDE_
#define _CAESARIA_SOUND_ENGINE_H_INCLUDE_

#include <string>

#include "vfs/path.hpp"
#include "core/variant.hpp"

namespace audio
{

class Engine
{
public:
  typedef enum { ambient=0, theme, game } SoundType;

  static Engine& instance();

  void setVolume(SoundType type , int value);
  int volume(SoundType type) const;

  int maxVolumeValue() const;

  ~Engine();

  void init();
  void exit();

  VariantMap save() const;
  void load(const VariantMap& stream);

  int play( vfs::Path filename, int volume, SoundType type );
  int play( std::string rc, int index, int volume, SoundType type );

  bool isPlaying( vfs::Path filename ) const;

  void stop( vfs::Path filename );
  void stop( int channel );
private:
  Engine();
  bool _loadSound( vfs::Path filename );

  class Impl;
  ScopedPtr< Impl > _d;
};

} //end namespace audio

#endif //_CAESARIA_SOUND_ENGINE_H_INCLUDE_

