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

#include "engine.hpp"

#include <cstdlib>
#include <string>
#include <set>
#include <sstream>
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "game/settings.hpp"
#include "core/exception.hpp"
#include "thread/mutex.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "vfs/filesystem.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "vfs/file.hpp"

using namespace vfs;

static void _resolveChannelFinished( int channel )
{
  audio::Engine::instance().stop( channel );
}

namespace audio
{

struct Sample
{
  int channel;
  std::string sound;
  audio::SoundType typeSound;
  int volume;
  bool finished;
  Mix_Chunk* chunk;

  void setVolume( int game, int type )
  {
    if( channel >= 0 )
    {
      float result = math::clamp<int>( volume, 0, 100 ) / 100.f;

      result = ( result * (type/100.f) * (game/100.f) ) * ( 2 * MIX_MAX_VOLUME );

      Mix_Volume( channel, (int)result );
    }
  }

  void destroy()
  {
    if( channel >= 0 )
      Mix_FreeChunk( chunk );
  }
};

class Samples : public std::map< unsigned int, Sample >
{
public:

};

class Engine::Impl
{
public:
  static const int maxSamplesNumber = 64;
  bool useSound;

  typedef std::map< audio::SoundType, int > Volumes;
  typedef std::map< unsigned int, std::string > Aliases;
  typedef std::list< Directory > Folders;
  typedef std::map< unsigned int, ByteArray > SoundCache;

  Samples samples;
  Aliases aliases;
  Volumes volumes;
  Folders folders;
  SoundCache cachedSounds;
  StringArray extensions;
  std::string currentTheme;

public:
  void clearFinishedChannels();
  void resetIfalias( std::string& sampleName );

  vfs::Path findFullPath(const std::string& sampleName );
};

void Engine::setVolume( audio::SoundType type, int value)
{
  _d->volumes[ type ] = value;
  _updateSamplesVolume();
}

void Engine::loadAlias(const vfs::Path& filename)
{
  VariantMap alias = config::load( filename );
  for( auto item : alias )
  {
    _d->aliases[ Hash( item.first ) ] = item.second.toString();
  }
}

void Engine::addFolder(Directory dir) {  _d->folders.push_back( dir ); }

int Engine::volume(audio::SoundType type) const
{
  Impl::Volumes::const_iterator it = _d->volumes.find( type );
  return it != _d->volumes.end() ? it->second : 0;
}

int Engine::maxVolumeValue() const { return 100; }

Engine::Engine() : _d( new Impl )
{
  _d->useSound = false;
  _d->volumes[ game ] = maxVolumeValue();
  _d->volumes[ theme ] = maxVolumeValue() / 2;
  _d->volumes[ ambient ] = maxVolumeValue() / 4;
  _d->volumes[ speech ] = maxVolumeValue() / 2;

  _d->extensions << ".ogg" << ".wav";
  addFolder( Directory() );
}

Engine::~Engine() {}

void Engine::init()
{
  bool sound_ok = false;
  if( _d->useSound )
  {
     return;        // avoid init twice
  }

  Logger::warning( "Game: initialize SDL sound subsystem" );
  if(SDL_InitSubSystem(SDL_INIT_AUDIO) != -1)
  {
    // open an audio channel

    int freq = 22050;
    int channels = 1;
    unsigned short int format = AUDIO_S16SYS;
    int samples = 1024;

    Logger::warning( "Game: try open audio" );
    if(Mix_OpenAudio(freq, format, channels, samples) != -1)
    {
      Logger::warning( "Game: sound check if we got the right audi format" );
      Mix_QuerySpec(&freq, &format, &channels);
      if (format == AUDIO_S16SYS)
      {
        Logger::warning( "Game: finished sound initializing" );
        sound_ok = true;

        Logger::warning( "Game: try allocate 16 mixing channels" );
        Mix_AllocateChannels(16);

        Logger::warning( "Game: start playing sounds" );
        Mix_ResumeMusic();

        Logger::warning( "Game: bind ChannelFinished" );
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

  Logger::warning( "Game: sound initialization ok" );
  _d->useSound = sound_ok;
}


void Engine::exit() {  Mix_CloseAudio(); }

Path Engine::Impl::findFullPath( const std::string& sampleName )
{
  const Path sPath( sampleName );
  Path rPath;

  if( sPath.extension().empty() )
  {
    Path fPath;
    for( auto ext : extensions )
    {
      fPath = sPath.toString() + ext;
      if( fPath.exist() )
        return fPath;

      for( auto folder : folders )
      {
        rPath = folder.find( fPath, Path::ignoreCase );
        if( !rPath.empty() )
          return rPath;
      }
    }
  }
  else
  {
    if( sPath.exist() )
      return sPath;

    for( auto folder : folders )
    {
      rPath = folder.find( sPath, Path::ignoreCase );
      if( !rPath.empty() )
        return rPath;
    }
  }

  return Path();
}

unsigned int Engine::_loadSound(const std::string& sampleName)
{
  if(!_d->useSound)
    return 0;

  std::string sampleCanonical = utils::localeLower( sampleName );

  unsigned int sampleHash = Hash( sampleCanonical );
  if( _d->samples.size()<Impl::maxSamplesNumber )
  {
    Samples::iterator i = _d->samples.find( sampleHash );

    if( i != _d->samples.end() )
    {
      return sampleHash;
    }

    vfs::Path realPath = _d->findFullPath( sampleCanonical );

    if( realPath.toString().empty() )
    {
      Logger::warning( "SoundEngine: could not find sound %s", sampleName.c_str() );
      return 0;
    }

    Sample sample;

    /* load the sample */
    NFile soundFile = NFile::open( realPath );
    ByteArray data = soundFile.readAll();

    if( data.empty() )
    {
      Logger::warning( "SoundEngine: could not load sound (%s)", realPath.toCString() );
      return 0;
    }

    sample.channel = -1;
    sample.chunk = Mix_LoadWAV_RW( SDL_RWFromMem( data.data(), data.size() ), 1 );
    sample.sound = realPath.toString();
    if(sample.chunk == NULL)
    {
      Logger::warning( "SoundEngine: could not load sound (%s) with error:%s", realPath.toCString(), SDL_GetError() );
      return 0;
    }

    _d->samples[ sampleHash ] = sample;
  }

  return sampleHash;
}

int Engine::play( std::string sampleName, int volValue, SoundType type )
{
  if(!_d->useSound )
    return -1;

  _d->clearFinishedChannels();
  _d->resetIfalias( sampleName );

  if( type == theme )
  {
    stop( _d->currentTheme );
    _d->currentTheme = sampleName;
  }

  unsigned int sampleHash = _loadSound( sampleName );

  if( sampleHash != 0 )
  {
    Samples::iterator i = _d->samples.find( sampleHash );

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

    i->second.typeSound = type;
    i->second.volume = volValue;
    i->second.finished = false;

    int typeVolume = volume( type );
    int gameVolume = volume( audio::game );
    i->second.setVolume( gameVolume, typeVolume );

    return i->second.channel;
  }

  return -1;
}

int Engine::play(const std::string &rc, int index, int volume, SoundType type)
{
  std::string filename = utils::format( 0xff, "%s_%05d", rc.c_str(), index );
  return play( filename, volume, type );
}

bool Engine::isPlaying(const std::string& sampleName) const
{
  if( !_d->useSound )
    return false;

  std::string rname = sampleName;
  _d->resetIfalias( rname );
  Samples::iterator i = _d->samples.find( Hash( rname ) );

  if( i == _d->samples.end() )
  {
    return false;
  }

  return (i->second.channel >= 0 && Mix_Playing( i->second.channel ) > 0);
}

void Engine::stop(const std::string& sampleName) const
{
  if( !_d->useSound )
    return;

  std::string rname = utils::localeLower( sampleName );
  _d->resetIfalias( rname );

  Samples::iterator i = _d->samples.find( Hash( rname ) );

  if( i == _d->samples.end() )
  {
    return;
  }

  Mix_HaltChannel( i->second.channel );
}

void Engine::stop(int channel)
{
  if( !_d->useSound )
    return;

  for( auto sample :_d->samples )
  {
    if( sample.second.channel == channel )
    {
      sample.second.finished = true;
      return;
    }
  }
}

void Engine::_updateSamplesVolume()
{
  if( !_d->useSound )
    return;

  int gameLvl = volume( audio::game );

  for( auto sample : _d->samples )
  {
    int typeVlm = volume( sample.second.typeSound );
    sample.second.setVolume( gameLvl, typeVlm );
  }
}

void Helper::initTalksArchive(const vfs::Path& filename)
{ 
  static Path saveFilename;

  FileSystem::instance().unmountArchive( saveFilename );

  saveFilename = filename;
  FileSystem::instance().mountArchive( saveFilename );
}

void Engine::Impl::clearFinishedChannels()
{
  for( Samples::iterator it=samples.begin(); it != samples.end();  )
  {
    if( it->second.finished ) { it->second.destroy(); samples.erase( it++ ); }
    else { ++it; }
  }
}

void  Engine::Impl::resetIfalias(std::string& sampleName)
{
  Aliases::iterator it = aliases.find( Hash( sampleName ) );
  if( it != aliases.end() )
    sampleName = it->second;
}

void Muter::activate(int value)
{
  Engine& ae = Engine::instance();
  _states[ ambient ] = ae.volume( ambient );
  _states[ theme ] = ae.volume( theme );

  ae.setVolume( audio::ambient, value );
  ae.setVolume( audio::theme, value );
}

Muter::~Muter()
{
  Engine& ae = Engine::instance();
  foreach( it, _states )
    ae.setVolume( it->first, it->second );
}

SampleDeleter::~SampleDeleter()
{
  if( !_sample.empty() )
    Engine::instance().stop( _sample );
}

void SampleDeleter::assign(const std::string& sampleName)
{
  _sample = sampleName;
}

}//end namespace audio
