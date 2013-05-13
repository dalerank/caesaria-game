Building with CMake
-------------------
CMake (http://www.cmake.org) is a cross-platform, open-source make system. It is a program which, from a set of CMake scripts,
creates a native build system for your platform that allows you to build openCaesar3. 

CMake is out-of-source build system:

> When your build generates files, they have to go somewhere.
> An in-source build puts them in your source tree.
> An out-of-source build puts them in a completely separate directory, so that your source tree is unchanged.

###openCaesar3 Dependencies

openCaesar3 is build using following libraries:
  * SDL - http://www.libsdl.org/
    * SDL_image - http://www.libsdl.org/projects/SDL_image/
    * SDL_mixer - http://www.libsdl.org/projects/SDL_mixer/
    * SDL_ttf - http://www.libsdl.org/projects/SDL_ttf/
  * Boost - http://www.boost.org/
    * System - http://www.boost.org/doc/libs/release/libs/system/
    * Filesystem - http://www.boost.org/doc/libs/release/libs/filesystem/
  * LibArchive - http://www.libarchive.org/

#### Windows
Binary dependencies for Visual Studio can be downloaded from http://opencaesar3.org/files/dependencies.7z .
After archive has been downloaded extract its content into *dependencies* folder of *openCaesar3 source root*.

#### Linux
To build project you need to install development version of the respective packages.

##### Ubuntu/Debian
You can use this commands to install all dependencies on Debian based systems

	sudo apt-get install libsdl1.2-dev libsdl-image1.2 libsdl-mixer1.2-dev libsdl-ttf2.0-dev
	sudo apt-get install libboost-system-dev libboost-filesystem-dev 
	sudo apt-get install libarchive-dev

##### OpenSUSE
Use zypper or YaST to install all necessary packages

	sudo zypper install libarchive-devel libSDL-devel libSDL_ttf-devel libSDL_image-devel libSDL_mixer-devel 

###Building openCaesar3

To build openCaesar navigate to source root directory and issue:

	$ mkdir build
	$ cd build    
	$ cmake ..
	$ make

This will generate standard unix *Makefile* in the *build* directory. Build directory could be completely out of source directory.

On windows you might want to use *cmake-gui* instead of *cmake*.
