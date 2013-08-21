#include "oc3_picture_loader.hpp"
#include "oc3_picture_loader_png.hpp"

class PictureLoader::Impl
{
public:
  void initLoaders();

  typedef std::vector< AbstractPictureLoader* > Loaders;
  typedef Loaders::iterator LoaderIterator;
  Loaders loaders;
};

PictureLoader::PictureLoader(void) : _d( new Impl )
{
  _d->initLoaders();
}

void PictureLoader::Impl::initLoaders()
{
  //_d->loaders.push_back( new PixmapLoaderBmp() );
  loaders.push_back( new PictureLoaderPng() );
  //_d->loaders.push_back( new PixmapLoaderPsd() );
  //_d->loaders.push_back( new PixmapLoaderJpeg() );
}

PictureLoader::~PictureLoader(void)
{
}

PictureLoader& PictureLoader::instance()
{
  static PictureLoader instanceLoader;
  return instanceLoader;
}

Picture PictureLoader::load( io::NFile file )
{
  if( !file.isOpen() )
     return Picture::getInvalid();

  // try to load file based on file extension
  Impl::LoaderIterator it = _d->loaders.begin();
  for( ; it != _d->loaders.end(); ++it)
  {
    if( (*it)->isALoadableFileExtension(file.getFileName()) ||
        (*it)->isALoadableFileFormat(file) )
    {
      // reset file position which might have changed due to previous loadImage calls
      file.seek(0);
      return (*it)->load( file );
    }
  }

  return Picture::getInvalid(); // failed to load
}
