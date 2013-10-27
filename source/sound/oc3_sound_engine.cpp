// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#include "oc3_sound_engine.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <SDL.h>

#include "core/exception.hpp"


SoundEngine* SoundEngine::_instance = NULL;

SoundEngine& SoundEngine::instance()
{
   if (_instance == NULL)
   {
      THROW("Error, no sound engine instance");
   }
   return *_instance;
}


SoundEngine::SoundEngine()
{
   _instance = this;
   _currentMusic = NULL;
}

SoundEngine::~SoundEngine()
{
   _instance = NULL;
}

void SoundEngine::init()
{
   int rc;
   rc = SDL_Init(SDL_INIT_AUDIO);
   if (rc != 0) THROW("Unable to initialize SDL AUDIO: " << SDL_GetError());

   // Open the audio device, 16-bit mono at 22Khz
   int desired_freq = 22050;
   int desired_format = AUDIO_S16;
   int desired_channels = 0;
   int desired_samples = 512;

   rc = Mix_OpenAudio(desired_freq, desired_format, desired_channels, desired_samples);
   if (rc != 0) THROW("Cannot start sound mixer, error: " << SDL_GetError());
}


void SoundEngine::exit()
{
   Mix_CloseAudio();
}

void SoundEngine::play_music(const std::string &filename)
{
   if (_currentMusic == NULL)
   {
      // load the music
      _currentMusic = Mix_LoadMUS(filename.c_str());
      Mix_PlayMusic(_currentMusic, 0);
      Mix_HookMusicFinished(callback_music_end);
   }
}


void SoundEngine::free_music()
{
   // std::cout << "free music!" << std::endl;
   Mix_HaltMusic();
   Mix_FreeMusic(_currentMusic);
   _currentMusic = NULL;
}


void callback_music_end()
{
   SoundEngine::instance().free_music();
}
