#include "oc3_picture_loader_png.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_filepath.hpp"
#include "oc3_gfx_engine.hpp"

#include <SDL.h>

#if defined(NO_USE_SYSTEM_LIBPNG)
    #include "utils/libpng/png.h"
#else
    #include <png.h>
#endif

// PNG function for error handling
static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
  StringHelper::debug( 0xff, "PNG fatal error %s", msg );
  longjmp( png_jmpbuf(png_ptr), 1 );
}

// PNG function for warning handling
static void png_cpexcept_warn(png_structp png_ptr, png_const_charp msg)
{
  StringHelper::debug( 0xff, "PNG warning %s", msg );
}

// PNG function for file reading
void PNGAPI user_read_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  png_size_t check;

  // changed by zola {
  io::NFile* file = (io::NFile*)png_get_io_ptr(png_ptr);
  check=(png_size_t)file->read((void*)data,(unsigned int)length);
  // }

  if( check != length )
  {
    png_error(png_ptr, "Read Error");
  }
}

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool PictureLoaderPng::isALoadableFileExtension(const io::FilePath& filename) const
{
  return filename.isExtension( ".png" );
}


//! returns true if the file maybe is able to be loaded by this class
bool PictureLoaderPng::isALoadableFileFormat( io::NFile file) const
{
  if( !file.isOpen() )
    return false;

  png_byte buffer[8];
  // Read the first few bytes of the PNG file
  if( file.read(buffer, 8) != 8 )
      return false;

  // Check if it really is a PNG file
  return !png_sig_cmp(buffer, 0, 8);
}


// load in the image data
Picture PictureLoaderPng::load( io::NFile file ) const
{
  if(!file.isOpen())
    return Picture::getInvalid();

  png_byte buffer[8];
  // Read the first few bytes of the PNG file
  if( file.read(buffer, 8) != 8 )
  {
    StringHelper::debug( 0xff, "LOAD PNG: can't read file %s", file.getFileName().toString().c_str() );
    return Picture::getInvalid();
  }

  // Check if it really is a PNG file
  if( png_sig_cmp(buffer, 0, 8) )
  {
    StringHelper::debug( 0xff, "LOAD PNG: not really a png %s", file.getFileName().toString().c_str() );
    return Picture::getInvalid();
  }

  // Allocate the png read struct
  png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
                                                NULL, (png_error_ptr)png_cpexcept_error,
                                                (png_error_ptr)png_cpexcept_warn);
  if( !png_ptr )
  {
    StringHelper::debug( 0xff, "LOAD PNG: Internal PNG create read struct failure %s", file.getFileName().toString().c_str() );
    return Picture::getInvalid();
  }

  // Allocate the png info struct
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    StringHelper::debug( 0xff, "LOAD PNG: Internal PNG create info struct failure 5s", file.getFileName().toString().c_str() );
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return Picture::getInvalid();
  }

  // for proper error handling
  if (setjmp(png_jmpbuf(png_ptr)))
  {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        /*
        if( RowPointers )
                                delete [] RowPointers;
                        */
        return Picture::getInvalid();
  }

  // changed by zola so we don't need to have public FILE pointers
  png_set_read_fn(png_ptr, &file, user_read_data_fcn);

  png_set_sig_bytes(png_ptr, 8); // Tell png that we read the signature

  png_read_info(png_ptr, info_ptr); // Read the info section of the png file

  unsigned int Width;
  unsigned int Height;
  int BitDepth;
  int ColorType;
  {
        // Use temporary variables to avoid passing casted pointers
        png_uint_32 w,h;
        // Extract info
        png_get_IHDR(png_ptr, info_ptr,
                &w, &h,
                &BitDepth, &ColorType, NULL, NULL, NULL);
        Width=w;
        Height=h;
  }

  // Convert palette color to true color
  if (ColorType==PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

  // Convert low bit colors to 8 bit colors
  if (BitDepth < 8)
  {
        if (ColorType==PNG_COLOR_TYPE_GRAY || ColorType==PNG_COLOR_TYPE_GRAY_ALPHA)
                png_set_expand_gray_1_2_4_to_8(png_ptr);
        else
                png_set_packing(png_ptr);
  }

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

  // Convert high bit colors to 8 bit colors
  if (BitDepth == 16)
        png_set_strip_16(png_ptr);

  // Convert gray color to true color
  if (ColorType==PNG_COLOR_TYPE_GRAY || ColorType==PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

  int intent;
  const double screen_gamma = 2.2;

  if (png_get_sRGB(png_ptr, info_ptr, &intent))
        png_set_gamma(png_ptr, screen_gamma, 0.45455);
  else
  {
        double image_gamma;
        if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
                png_set_gamma(png_ptr, screen_gamma, image_gamma);
        else
                png_set_gamma(png_ptr, screen_gamma, 0.45455);
  }

  // Update the changes in between, as we need to get the new color type
  // for proper processing of the RGBA type
  png_read_update_info(png_ptr, info_ptr);
  {
     // Use temporary variables to avoid passing casted pointers
     png_uint_32 w,h;
     // Extract info
     png_get_IHDR(png_ptr, info_ptr,
             &w, &h,
             &BitDepth, &ColorType, NULL, NULL, NULL);
     Width=w;
     Height=h;
  }

  // Convert RGBA to BGRA
  if (ColorType==PNG_COLOR_TYPE_RGB_ALPHA)
  {
    png_set_bgr(png_ptr);
  }

  // Create the image structure to be filled by png data
  Picture* pic = GfxEngine::instance().createPicture( Size( Width, Height ) );
  GfxEngine::instance().loadPicture( *pic );

  if( pic->getSize().getArea() == 0 )
  {
    StringHelper::debug( 0xff, "LOAD PNG: Internal PNG create image struct failure %s", file.getFileName().toString().c_str() );
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return Picture::getInvalid();
  }

  if( !Height )
  {
    StringHelper::debug( 0xff, "LOAD PNG: Internal PNG create row pointers failure %s", file.getFileName().toString().c_str() );
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return Picture::getInvalid();
  }

  // Create array of pointers to rows in image data
  ScopedPtr<unsigned char*> RowPointers( (unsigned char**)new png_bytep[ Height ] );

  // Fill array of pointers to rows in image data
  SDL_LockSurface( pic->getSurface() );
  unsigned char* data = (unsigned char*)pic->getSurface()->pixels;
  for(unsigned int i=0; i<Height; ++i)
  {
    RowPointers.data()[i] = data;
    data += pic->getSurface()->pitch;
  }

  // for proper error handling
  if( setjmp( png_jmpbuf( png_ptr ) ) )
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    GfxEngine::instance().deletePicture( pic );
    return Picture::getInvalid();
  }

  // Read data using the library function that handles all transformations including interlacing
  png_read_image( png_ptr, RowPointers.data() );

  png_read_end( png_ptr, NULL );
  png_destroy_read_struct( &png_ptr, &info_ptr, 0 ); // Clean up memory

  SDL_UnlockSurface(pic->getSurface());

  return *pic;
}
