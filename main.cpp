#include "oc3_game.hpp"
#include "oc3_game_settings.hpp"
#include "oc3_exception.hpp"
#include "oc3_stringhelper.hpp"

int main(int argc, char* argv[])
{
   for (int i = 0; i < (argc - 1); i++)
   {
     if( !strcmp( argv[i], "-R" ) )
     {
       std::string path = argv[i+1];
       GameSettings::set( GameSettings::resourcePath, Variant( path ) );
       GameSettings::set( GameSettings::localePath, Variant( path + "/locale" ) );
       i++;
     }

     if( !strcmp( argv[i], "-Lc" ) )
     {
       GameSettings::set( GameSettings::localeName, Variant( std::string( argv[i+1] ) ) );
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
     StringHelper::debug( 0xff, "FATAL ERROR: %s", e.getDescription().c_str() );
   }

   return 0;
}
