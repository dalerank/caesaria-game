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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "game/game.hpp"
#include "game/settings.hpp"
#include "core/exception.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "core/stacktrace.hpp"
#include "core/osystem.hpp"

#if defined(CAESARIA_PLATFORM_WIN)
  #undef main
#endif

#if defined(CAESARIA_PLATFORM_ANDROID)
#include <SDL.h>
int SDL_main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
  Logger::registerWriter( Logger::consolelog );
  Logger::registerWriter( Logger::filelog );

#ifdef CAESARIA_PLATFORM_ANDROID
  vfs::Directory workdir( std::string("/sdcard/Android/data/net.dalerank.caesaria/files") );
#else
  vfs::Directory workdir = vfs::Path( argv[0] ).directory();
#endif
  Logger::warning( "Options: working directory is " + workdir.toString() );

  GameSettings::getInstance().setwdir( workdir.toString() );
  bool native_resources = false;
  for (int i = 0; i < (argc - 1); i++)
  {
    if( !strcmp( argv[i], "-R" ) )
    {
      const char* opts = argv[i+1];
      Logger::warning( "Options: setting workdir to %s", opts  );
      GameSettings::getInstance().setwdir( std::string( opts, strlen( opts ) ) );
      i++;
    }

    if( !strcmp( argv[i], "-Lc" ) )
    {
      Logger::warning( "Options: setting language to " + std::string( argv[i+1] ) );
      GameSettings::set( GameSettings::language, Variant( std::string( argv[i+1] ) ) );
      i++;
    }

    if( !strcmp( argv[i], "-c3gfx" ) )
    {
      Logger::warning( "Options: using native C3 resources from" + std::string( argv[i+1] ) );
      GameSettings::set( GameSettings::c3gfx, Variant( std::string( argv[i+1] ) ) );
      native_resources = true;
      i++;
    }
  }

  vfs::Path testPics = GameSettings::rcpath( "/pics/pics.zip" );
  if( !testPics.exist() && native_resources == false )
  {
    Logger::warning( "Critical: Not found graphics data. Use precompiled CaesarIA archive or set\n"
                     "-c3gfx flag to set absolute path to Caesar III(r) installation folder,\n"
                     "forexample, \"-c3gfx c:/games/caesar3/\"" );
    return 0;
  }

  try
  {
    Game game;
    game.initialize();
    game.exec();
  }
  catch( Exception& e )
  {
    Logger::warning( "Critical error: %s", e.getDescription().c_str() );
    Stacktrace::print();
  }

  return 0;
}
