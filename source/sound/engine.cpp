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

#include "engine.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "game/settings.hpp"
#include "core/exception.hpp"
#include "core/logger.hpp"
#include "core/stringhelper.hpp"
#include "vfs/file.hpp"

static void _resolveChannelFinished(int channel)
{
  audio::Engine::instance().stop( channel );
}

namespace audio
{

struct Sample
{
  int channel;
  std::string sound;
  Mix_Chunk* chunk;
};

class Engine::Impl
{
public:
  static const int maxSamplesNumner = 64;
  bool useSound;

  typedef std::map< std::string, Sample > Samples;
  typedef std::map< Engine::SoundType, int > Volumes;
  Samples samples;
  Volumes volumes;
};

Engine& Engine::instance()
{
   static Engine _instance;
   return _instance;
}

void Engine::setVolume(Engine::SoundType type, int value)
{
  _d->volumes[ type ] = value;
}

int Engine::volume(Engine::SoundType type) const
{
  Impl::Volumes::const_iterator it = _d->volumes.find( type );
  return it != _d->volumes.end() ? it->second : 0;
}

int Engine::maxVolumeValue() const
{
  return MIX_MAX_VOLUME;
}

Engine::Engine() : _d( new Impl )
{
  _d->useSound = false;
  _d->volumes[ game ] = maxVolumeValue();
  _d->volumes[ theme ] = maxVolumeValue() / 2;
  _d->volumes[ ambient ] = maxVolumeValue() / 4;
}

Engine::~Engine() {}

void Engine::init()
{
  bool sound_ok = false;
  if( _d->useSound )
  {
     return;        // avoid init twice
  }

  // initialize SDL sound subsystem
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != -1)
  {
    // open an audio channel

    int freq = 22050;
    int channels = 1;
    unsigned short int format = AUDIO_S16SYS;
    int samples = 1024;

    if (Mix_OpenAudio(freq, format, channels, samples) != -1)
    {
      Mix_QuerySpec(&freq, &format, &channels);
      // check if we got the right audi format
      if (format == AUDIO_S16SYS)
      {
        // finished initializing
        sound_ok = true;

        // allocate 16 mixing channels
        Mix_AllocateChannels(16);

        // start playing sounds
        Mix_ResumeMusic();
        Mix_ChannelFinished( &_resolveChannelFinished );
      }
      else
      {
        Logger::warning( "Open audio channel doesn't meet requirements. Muting" );
        Mix_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
      }
    }
    else
    {
      Logger::warning( "Could not open required audio channel. Muting ");
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
  }
  else
  {
    Logger::warning( "Could not initialize sound system. Muting ");
  }

  _d->useSound = sound_ok;
}


void Engine::exit()
{
  Mix_CloseAudio();
}

VariantMap Engine::save() const
{
  VariantMap ret;
  ret[ "ambient" ] = _d->volumes[ ambient ];
  ret[ "theme"   ] = _d->volumes[ theme ];
  ret[ "game"    ] = _d->volumes[ game ];

  return ret;
}

void Engine::load(const VariantMap& stream)
{
  _d->volumes[ ambient ] = stream.get( "ambient" );
  _d->volumes[ theme ] = stream.get( "theme" );
  _d->volumes[ game ] = stream.get( "game" );
}

bool Engine::_loadSound(vfs::Path filename)
{
  if(_d->useSound>0 && _d->samples.size()<Impl::maxSamplesNumner)
  {
    Impl::Samples::iterator i = _d->samples.find( filename.toString() );

    if( i != _d->samples.end() )
    {
      return true;
    }

    Sample sample;

    /* load the sample */
    vfs::NFile wavFile = vfs::NFile::open( filename );
    ByteArray data = wavFile.readAll();

    if( data.empty() )
    {
      return false;
    }

    sample.channel = -1;
    sample.chunk = Mix_LoadWAV_RW( SDL_RWFromMem( data.data(), data.size() ), 1 );
    sample.sound = filename.toString();
    if(sample.chunk == NULL)
    {
      Logger::warning( "could not load wav (%s)", SDL_GetError() );
      return false;
    }

    _d->samples[ filename.toString() ] = sample;
    //Logger::warning( " Loaded %s to sample %i.",filename.toString().c_str(), _d->samples.size() );
  }

  return true;
}

int Engine::play( vfs::Path filename, int volValue, SoundType type )
{
  if(_d->useSound )
  {
    bool isLoading = _loadSound( filename );

    if( isLoading )
    {
      Impl::Samples::iterator i = _d->samples.find( filename.toString() );

      if( i == _d->samples.end() )
      {
        return -1;
      }

      if( (i->second.channel == -1 )
          || (i->second.channel >= 0 && Mix_Playing( i->second.channel ) <= 0) )
      {

        // sdl_mixer finds free channel, we then play at correct volume
        i->second.channel = Mix_PlayChannel(-1, i->second.chunk, 0);
      }

      int result = math::clamp( volValue, 0, maxVolumeValue() );
      float typeVolume = volume( type ) / 100.f;
      float gameVolume = volume( Engine::game ) / 100.f;

      result = (int)(result * typeVolume * gameVolume );
      Mix_Volume( i->second.channel, result);
      return i->second.channel;
    }
  }

  return -1;
}

int Engine::play(std::string rc, int index, int volume, SoundType type)
{
  std::string filename = StringHelper::format( 0xff, "%s_%05d.wav", rc.c_str(), index );
  return play( filename, volume, type );
}

void Engine::stop( vfs::Path filename )
{
  Impl::Samples::iterator i = _d->samples.find( filename.toString() );

  if( i == _d->samples.end() )
  {
    return;
  }

  Mix_HaltChannel( i->second.channel );
  Mix_FreeChunk( i->second.chunk );

  _d->samples.erase( i );
}

void Engine::stop(int channel)
{
  for( Impl::Samples::iterator it=_d->samples.begin(); it != _d->samples.end(); it++ )
  {
    if( it->second.channel == channel )
    {
      Mix_FreeChunk( it->second.chunk );

      _d->samples.erase( it );
      return;
    }
  }
}

}//end namespace audio
