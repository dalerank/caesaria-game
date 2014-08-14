package org.mygame;

import org.libsdl.app.SDLActivity;

public class MyGame extends SDLActivity {
  //load .so
  static {
    System.loadLibrary( "aes" );
    System.loadLibrary( "bzip2" );
    System.loadLibrary( "lzma" );
    System.loadLibrary( "png" );
    System.loadLibrary( "sdl_ttf" );
    System.loadLibrary( "smk" );
    System.loadLibrary( "application" );
  }
}
