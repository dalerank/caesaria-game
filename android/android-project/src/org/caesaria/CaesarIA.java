package net.dalerank.caesaria;

import org.libsdl.app.SDLActivity;

public class CaesarIA extends SDLActivity {
   protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "aes",
            "bzip2",
            "lzma",
            "pnggo",
            "sdl_ttf",
            "smk",
            "main"
        };
    }
}

