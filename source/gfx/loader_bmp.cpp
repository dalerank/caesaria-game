#include "loader_bmp.hpp"
#include "core/logger.hpp"
#include "vfs/path.hpp"
#include "gfx/engine.hpp"

#include <SDL.h>


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool PictureLoaderBmp::isALoadableFileExtension(const vfs::Path& filename) const
{
  return filename.isMyExtension( ".bmp" );
}

//! returns true if the file maybe is able to be loaded by this class
bool PictureLoaderBmp::isALoadableFileFormat( vfs::NFile file) const
{
  if( !file.isOpen() )
    return false;

  unsigned short checkWord;
  file.seek( 0 );
  file.read( &checkWord, 2 );

  return checkWord == 0x4D42;
}

Picture PictureLoaderBmp::load(vfs::NFile file) const
{
	//Get the bitmap's buffer and size from the resource file
	if( !file.isOpen() )
		return Picture::getInvalid();

	int filesize = file.size();
	ByteArray data = file.readAll();
	char *buffer = data.data();

	//Load the buffer into a surface using RWops
	SDL_RWops *rw = SDL_RWFromMem(buffer, filesize);
	SDL_Surface *temp = SDL_LoadBMP_RW(rw, 1);

	//Were we able to load the bitmap?
	if (temp == NULL)
	{
		Logger::warning( "Unable to load bitmap: %s", SDL_GetError());
		return Picture::getInvalid();
	}

	Picture pic;
	pic.init( temp, Point(0, 0) );
	Engine::instance().loadPicture( pic );

  if( pic.size().area() == 0 )
  {
    Logger::warning( "LOAD BMP: Internal create image struct failure " + file.path().toString() );
    return Picture::getInvalid();
  }

	//Return our loaded image
	return pic;
}


