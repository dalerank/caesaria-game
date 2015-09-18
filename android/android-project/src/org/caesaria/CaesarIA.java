package net.dalerank.caesaria;

import org.libsdl.app.SDLActivity;

public class CaesarIA extends SDLActivity {
  //load .so
  static {
    System.loadLibrary( "aes" );
    System.loadLibrary( "bzip2" );
    System.loadLibrary( "lzma" );
    System.loadLibrary( "pnggo" );
    System.loadLibrary( "sdl_ttf" );
    System.loadLibrary( "smk" );
    System.loadLibrary( "main" ); 	 	
  }
}
