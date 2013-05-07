openCaesar3 
===========

openCaesar3 is a remake of the classic Caesar3 PC game, a city building game developed by Impression Games and published by Sierra Entertainment, in 1998.


The game will be a near clone to the original Caesar3 game. Notorious changes are:
 * new AI for walkers. They just go where they are needed
 * actors just out of the actor school do not provide access to the entertainment (same for gladiators, ...)

The game is still not quite playable

Dependencies
============
  * SDL - http://www.libsdl.org/
    * SDL_image - http://www.libsdl.org/projects/SDL_image/
    * SDL_mixer - http://www.libsdl.org/projects/SDL_mixer/
    * SDL_ttf - http://www.libsdl.org/projects/SDL_ttf/
  * Boost - http://www.boost.org/
    * System - http://www.boost.org/doc/libs/release/libs/system/
    * Filesystem - http://www.boost.org/doc/libs/release/libs/filesystem/
  * LibArchive - http://www.libarchive.org/

Linux
-----

### Ubuntu/Debian

	sudo apt-get install libsdl1.2-dev libsdl-image1.2 libsdl-mixer1.2-dev libsdl-ttf2.0-dev
	sudo apt-get install libboost-system-dev libboost-filesystem-dev 
	sudo apt-get install libarchive-dev

Building openCaesar3 - Using CMake
==================================

You can build openCaesar using following commands

	$ mkdir build
	$ cd build      
	$ cmake ..
	$ cmake --build .

