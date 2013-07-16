openCaesar3
-----------
openCaesar3 is a remake of the classic Caesar3 PC game, a city building game developed by Impression Games and published by Sierra Entertainment, in 1998.

The game will be a near clone of the original Caesar3 game. Notable changes are:
 * A new AI for walkers. They will walk where they are needed the most.
 * Actors coming from the actor school do not provide access to the entertainment. The same goes for lion tamers, gladiators and horse wagons.

The game is currently under heavy development and not in a playable state. Developers are invited to participate.

Building openCaesar3
-------------------
See INSTALL_LINUX.md or INSTALL_WINDOWS.md depending on which operating system you are running.

####External dependencies
openCaesar3 is build using following excellent libraries:
  * SDL - http://www.libsdl.org/
    * SDL_image - http://www.libsdl.org/projects/SDL_image/
    * SDL_mixer - http://www.libsdl.org/projects/SDL_mixer/
    * SDL_ttf - http://www.libsdl.org/projects/SDL_ttf/
  * Boost - http://www.boost.org/
    * System - http://www.boost.org/doc/libs/release/libs/system/
    * Filesystem - http://www.boost.org/doc/libs/release/libs/filesystem/
  * LibArchive - http://www.libarchive.org/

Releases:
---------
##### 2013-05-11
  * Fixed a lot of bugs.
  * Minimap is introduced.
  * Migration to cmake building system.
  * Houses now can evolve/devolve (a little bit buggy).

##### 2013-04-03
  * Added many buildings.

##### 2013-03-02
  * First binary release for windows.
  * Start implement the menubar with money/population/date.
  * Use the slim interface for build menu (regression: no more access to save/load game).
  * Start implement funds and taxes.

##### 2013-02-09
  * Implement clear land.
  * Add copyright notice.

##### 2012-12-01
  * Save/load game complete.
  * Implement nearly all entertainment buildings.

##### 2012-11-10
  * Save/load game starts to work.

##### 2012-10-12
  * Start implement load game.

##### 2012-09-17
  * Start implement save game.

##### 2012-06-14
  * Big code refactoring of the GUI. prepare for game music.

##### 2012-05-21
  * Add an installation procedure.

##### 2012-05-15
  * Implement some buildings (water, entertainment, education). 
  * Start implementation of house evolution/regression.

##### 2012-04-20
  * First public release.

