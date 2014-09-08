#!/bin/sh
# Thanks for Anakros from habrahabr.ru
# Run this sript for download dependencies and install game
# then run ./caesaria.macos from console
 
LIBDIR=~/Library/Frameworks/
 
curl -O https://www.libsdl.org/release/SDL2-2.0.3.dmg
curl -O http://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.0.dmg

hdiutil mount SDL2_mixer-2.0.0.dmg
hdiutil mount SDL2-2.0.3.dmg
 
mkdir -p $LIBDIR
cp -r /Volumes/SDL2/SDL2.framework $LIBDIR
cp -r /Volumes/SDL2_mixer/SDL2_mixer.framework $LIBDIR
 
chmod +x caesaria.macos && chmod +x updater.macos
