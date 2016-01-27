#!/usr/bin/env bash    
SDLFR=~/Library/Frameworks/SDL2.framework
SAVEDIR=saves
LIBDIR=~/Library/Frameworks/


# if the file doesn't exist, try to create folder
if [ ! -d $SDLFR ]
then
  hdiutil mount SDL2_mixer-2.0.0.dmg
  hdiutil mount SDL2-2.0.3.dmg
  mkdir -p $LIBDIR
  cp -r /Volumes/SDL2/SDL2.framework $LIBDIR
  cp -r /Volumes/SDL2_mixer/SDL2_mixer.framework $LIBDIR
  hdiutil unmount /Volumes/SDL2
  hdiutil unmount /Volumes/SDL2_mixer
fi

if [ ! -d $SAVEDIR ]
then
  rm -f $SAVEDIR
  mkdir $SAVEDIR
fi

./caesaria.macos &
exit 0
