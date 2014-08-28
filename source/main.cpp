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

#include "core/stringhelper.hpp"
#include "core/exception.hpp"
#include "vfs/directory.hpp"
#include "game/settings.hpp"
#include "game/game.hpp"
#include "core/logger.hpp"
#include "core/stacktrace.hpp"
#include "core/osystem.hpp"

#if defined(CAESARIA_PLATFORM_WIN)
  #undef main
#endif

#if defined(CAESARIA_PLATFORM_ANDROID)
#include <SDL.h>
#include <SDL_system.h>
int SDL_main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
  vfs::Directory workdir;
#ifdef CAESARIA_PLATFORM_ANDROID
  workdir  = vfs::Path( SDL_AndroidGetExternalStoragePath() );
#else
  workdir = vfs::Path( argv[0] ).directory();
#endif
  Logger::registerWriter( Logger::consolelog, "" );

  Logger::warning( "Options: working directory is " + workdir.toString() );

  GameSettings::instance().setwdir( workdir.toString() );
  for (int i = 0; i < (argc - 1); i++)
  {
    if( !strcmp( argv[i], "-R" ) )
    {
      const char* opts = argv[i+1];
      workdir = vfs::Path( opts );
      Logger::warning( "Options: setting workdir to %s", opts  );
      GameSettings::instance().setwdir( std::string( opts, strlen( opts ) ) );
      i++;
    }
    else if( !strcmp( argv[i], "-Lc" ) )
    {
      const char* opts = argv[i+1];
      Logger::warning( "Options: setting language to %s", opts );
      GameSettings::set( GameSettings::language, Variant( opts ) );
      i++;
    }
    else if( !strcmp( argv[i], "-c3gfx" ) )
    {
      const char* opts = argv[i+1];
      Logger::warning( "Options: using native C3 resources from %s", opts );
      GameSettings::set( GameSettings::c3gfx, Variant( opts ) );
      i++;
    }
  }

  Logger::registerWriter( Logger::filelog, workdir.toString() );

  try
  {
    Game game;
    game.initialize();
    game.exec();
  }
  catch( Exception& e )
  {
    Logger::warning( "Critical error: " + e.getDescription() );
    Stacktrace::print();
  }

  return 0;
}
