![CaesarIA](https://bitbucket-assetroot.s3.amazonaws.com/c/photos/2013/Nov/12/caesaria-logo-3368332030-11_avatar.png)

Master: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=stable) Linux: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=stable) Windows: ![Master:Windows](https://travis-ci.org/dalerank/caesaria-game.svg?branch=windows-build) Android: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=android-build) Macos: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=macos-build)

![IndieDb Rating](http://button.indiedb.com/popularity/medium/games/27823.png)

# CaesarIA 0.5

CaesarIA is an open-source remake of the popular
citybuilder and economic sim Caesar III, videogame by
Empressions Games, written in C++ / SDL.

See more info at the ours [website](https://bitbucket.org/dalerank/caesaria/wiki/Home)

## Installation

CaesarIA requires a some items from copy of the original game resources.
When installing manually, copy the Caesar III .sg2 and .555 files to 
CaesarIA's gfx folder: <game directory>\resources\gfx, 
videos to <game directory>\resources\smk and sound to <game directory>\resources\audio

The resources can be in a different folder as the CaesarIA data.
You can also specify your own path by passing the command-line
argument "-c3gfx <path to CaesarIII folder>" when running CaesarIA.

### Windows, Mac OS X

CaesarIA will also check the following folders:

- <game directory>\saves (Windows 2000/XP/7/8)

It's recommended you copy the resources to the "gfx" folder.
The installer not automatically detect a Caesar III installation.

### Linux

CaesarIA requires the following libraries:

- [SDL2](http://www.libsdl.org) (libsdl2)
- [SDL2_mixer](http://www.libsdl.org/projects/SDL_mixer/) (libsdl2-mixer)

Check your distribution's package manager or the library
website on how to install them.

## Configuration

CaesarIA has a some settings which can be
customized, both in-game and out-game. These options are global
and affect any old or new savegame.

For more details please check the [wiki](https://bitbucket.org/dalerank/caesaria/wiki/Configuration%20files).

## Development

CaesarIA requires the following developer libraries:

- [SDL2](http://www.libsdl.org) (libsdl2)
- [SDL2_mixer](http://www.libsdl.org/projects/SDL_mixer/) (libsdl2-mixer)
- Cmake

It's also been tested on a variety of other tools on
Windows/Mac/Linux. More detailed compiling instructions
and pre-compiled dependencies are available at the [wiki](https://bitbucket.org/dalerank/caesaria/wiki/Compiling).