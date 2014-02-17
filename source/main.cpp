#include "game/game.hpp"
#include "game/settings.hpp"
#include "core/exception.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "core/stacktrace.hpp"

int main(int argc, char* argv[])
{
  Logger::registerWriter( Logger::consolelog );
  Logger::registerWriter( Logger::filelog );

  vfs::Directory workdir = vfs::Path( argv[0] ).directory();
  Logger::warning( "Working directory is " + workdir.toString() );

  GameSettings::getInstance().setwdir( workdir.toString() );

  for (int i = 0; i < (argc - 1); i++)
  {
    if( !strcmp( argv[i], "-R" ) )
    {
      const char* opts = argv[i+1];
      Logger::warning( "Setting workdir to %s", opts  );
      GameSettings::getInstance().setwdir( std::string( opts, strlen( opts ) ) );
      i++;
    }

    if( !strcmp( argv[i], "-Lc" ) )
    {      
      Logger::warning( "Setting language to " + std::string( argv[i+1] ) );
      GameSettings::set( GameSettings::language, Variant( std::string( argv[i+1] ) ) );
      i++;
    }
  }

  try
  {
    Game game;
    game.initialize();
    game.exec();
  }
  catch( Exception e )
  {
    Logger::warning( "Critical error: %s", e.getDescription().c_str() );
    Stacktrace::print();
  }

  return 0;
}
