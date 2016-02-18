![CaesarIA](https://bitbucket-assetroot.s3.amazonaws.com/c/photos/2013/Nov/12/caesaria-logo-3368332030-11_avatar.png)

Master: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=master) Linux: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=stable) Windows: ![Master:Windows](https://travis-ci.org/dalerank/caesaria-game.svg?branch=windows-build) Android: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=android-build) Macos: ![Build Status](https://travis-ci.org/dalerank/caesaria-game.svg?branch=macos-build)

![IndieDb Rating](http://button.indiedb.com/popularity/medium/games/27823.png)

# CaesarIA 0.5

Work for CaesarIA began in 2009, it was a wide screen mod for Caesar III. In 2012 development moved to a standalone game with its own engine. In 2014, the game was released in steam, but still open-source software. It is an open-source remake of the popular citybuilder/economic strategy - Caesar III, videogame by Empressions Games. Now, CaesarIA is still in development phase, which means an early experimental stage. It is playable, and you can already download and test the game, but some features yet still missing. When CaesarIA will be released? It is very hard to quetion. Even after we are done implementing all the features, we will want to continue work for new features and testing. Information about release will be posted.

See more info at the ours [website](https://bitbucket.org/dalerank/caesaria/wiki/Home)

## Installation

CaesarIA requires some resources from a copy of the original game.
When installing manually, copy the Caesar III .sg2 and .555 files to 
CaesarIA's gfx folder: <game directory>\resources\gfx, 
videos to <game directory>\resources\smk and sound to <game directory>\resources\audio

The resources can be in a different folder as the CaesarIA data.
You can also specify your own path by passing the command-line
argument "-c3gfx <path to CaesarIII folder>" when running CaesarIA.

### Saves

On Windows, Mac OS X
CaesarIA will also check the following folders:

- <game directory>\saves (Windows 2000/XP/7/8)

On Linux

- <userhome>/.caesaria/saves (Linux)

### Resources

1. It's recommended you copy the resources to the "gfx" folder.
The installer does not automatically detect a Caesar III installation.

2. You may need to run the updater.[exe/linux/macos], which will download
all assets necessary for playing the game: configs, textures, sounds etc.

3. You may download the new set of graphics, which we use in steam version

## Configuration

CaesarIA has a some settings which can be
customized, both in-game and out-game. These options are global
and affect any old or new savegame.

For more details please check the [wiki](https://bitbucket.org/dalerank/caesaria/wiki/Configuration%20files).

## Development

CaesarIA does not require any specific frameworks - all the frameworks we need are placed in the game repository. The game engine used is a patched version of SDL2, which is also packaged as part of game. You may need to install gcc or any compiler to be able to build game.
We use "cmake" for creating configs and building the game.

It's also been tested on a variety of other tools on
Windows/Mac/Linux. More detailed compiling instructions
and pre-compiled dependencies are available at the [wiki](https://bitbucket.org/dalerank/caesaria/wiki/Compiling).