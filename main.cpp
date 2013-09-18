#include "oc3_game.hpp"
#include "oc3_game_settings.hpp"
#include "oc3_exception.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_game_event_mgr.hpp"

int main(int argc, char* argv[])
{
   for (int i = 0; i < (argc - 1); i++)
   {
     if( !strcmp( argv[i], "-R" ) )
     {
       GameSettings::set( GameSettings::resourcePath, Variant( std::string( argv[i+1] ) ) );
       GameSettings::set( GameSettings::localePath, Variant( std::string( argv[i+1] ) + "/locale" ) );
       break;
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
