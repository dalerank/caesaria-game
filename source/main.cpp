#include "game/game.hpp"
#include "game/settings.hpp"
#include "core/exception.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"

int main(int argc, char* argv[])
{
  Logger::registerWriter( Logger::consolelog );
  Logger::registerWriter( Logger::filelog );

  vfs::Directory workdir = vfs::Path( argv[0] ).directory();
  Logger::warning( "Working directory is " + workdir.toString() );

  GameSettings::getInstance().setwdir( workdir );

  for (int i = 0; i < (argc - 1); i++)
  {
    if( !strcmp( argv[i], "-R" ) )
    {
      vfs::Directory rpath( std::string( argv[i+1] ) );
      Logger::warning( "Setting workdir to " + rpath.toString() );
      GameSettings::getInstance().setwdir( rpath );
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
    Logger::warning( "FATAL ERROR: %s", e.getDescription().c_str() );
  }

  return 0;
}
