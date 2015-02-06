// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "archive_sg2.hpp"

#include "core/utils.hpp"
#include "core/logger.hpp"
#include "gfx/picture.hpp"
#include "gfx/engine.hpp"
#include "gfx/pictureconverter.hpp"
#include "filenative_impl.hpp"
#include "core/rectangle.hpp"
#include "core/color.hpp"
#include "vfs/directory.hpp"
#include "vfs/memfile.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>

using namespace gfx;

namespace vfs
{

enum {
	sgHeagerSize = 680,
	maxBitmapRecords = 100,
	sgRecordSize = 200,
	ISOMETRIC_TILE_WIDTH = 58,
	ISOMETRIC_TILE_HEIGHT = 30,
	ISOMETRIC_TILE_BYTES = 1800,
	ISOMETRIC_LARGE_TILE_WIDTH = 78,
	ISOMETRIC_LARGE_TILE_HEIGHT = 40,
	ISOMETRIC_LARGE_TILE_BYTES = 3200
};


namespace {
static const std::string readerTypename=CAESARIA_STR_EXT(Sg2ArchiveReader);
static const char* arch555ext = "555";
}

Sg2ArchiveLoader::Sg2ArchiveLoader(vfs::FileSystem*)
{
}

bool Sg2ArchiveLoader::isALoadableFileFormat(const Path& filename) const
{
  return filename.isMyExtension( ".sg2", false );
}

bool Sg2ArchiveLoader::isALoadableFileFormat(vfs::NFile file) const
{
  SgHeader header;
  file.read(&header, sizeof(header));

  // SG2 file: filesize = 74480 or 522680 (depending on whether it's
  // a "normal" sg2 or an enemy sg2
  if (header.version == 0xd3)
    if (header.sg_filesize == 74480 || header.sg_filesize == 522680)
      return true;

  return false;
}

bool Sg2ArchiveLoader::isALoadableFileFormat(vfs::Archive::Type fileType) const
{
  return fileType == Archive::sg2;
}

ArchivePtr Sg2ArchiveLoader::createArchive(const Path& filename, bool ignoreCase, bool ignorePaths) const
{
  ArchivePtr archive;
  NFile file = NFile::open( filename, Entity::fmRead );

  if( file.isOpen() )
  {
    archive = createArchive(file, ignoreCase, ignorePaths);
  }

  return archive;
}

ArchivePtr Sg2ArchiveLoader::createArchive(NFile file, bool ignoreCase, bool ignorePaths) const
{
  ArchivePtr archive;
  if( file.isOpen() )
  {
    file.seek(0);

    SgHeader header;
    file.read(&header, sizeof(header));

    archive = new Sg2ArchiveReader(file);
    archive->drop();
  }
  return archive;
}

Sg2ArchiveReader::Sg2ArchiveReader(NFile file) : _file( file )
{
  SgHeader header;
  file.seek(0);
  file.read(&header, sizeof(header));
  file.seek(sgHeagerSize);

  Logger::warning( "Read header, num bitmaps = %d, num images = %d",
                   header.num_bitmap_records, header.num_image_records);

  // Read bitmaps
  for( int bn = 0; bn < header.num_bitmap_records; ++bn)
  {
    SgBitmapRecord sbr;
    file.seek( sgHeagerSize + sgRecordSize * bn );
    file.read(&sbr,sizeof(sbr));
    // Terminate strings
    sbr.filename[64] = 0;
    sbr.comment[50] = 0;

    strcpy( sbr.filename, utils::localeLower( sbr.filename ).c_str() );
    std::string bmp_name_full = sbr.filename;
    std::string bmp_name = bmp_name_full.substr(0, bmp_name_full.length() - 4);

    Logger::warning( "Start reading info for section: " + bmp_name );
    if( bmp_name == "system" )
      continue;

    // Load images
    for(uint32_t i = sbr.start_index; i < sbr.start_index+sbr.num_images; ++i)
    {
      SgImageRecord sir;
      file.seek(sgHeagerSize + maxBitmapRecords*sgRecordSize + (i * sizeof(SgImageRecord)) );
      file.read(&sir, sizeof(sir));


      // Construct name
      std::string name = utils::format( 0xff, "%s_%05d.png", bmp_name.c_str(), i - sbr.start_index + 1);
      // Locate appropriate 555 file
      Path p555;
      if( sir.flags[0] > 0 ) //is external resource file???
      {
        Directory p555_d = file.path().directory();
        Path p555_filename = Path( sbr.filename ).changeExtension( arch555ext );
        Path tmpPath = p555_d/p555_filename;
        SgFileEntry tmpEntry = { tmpPath.toString(), sir };
        p555 = _find555File( tmpEntry );
        //std::string p555_2 = p555 + "555/" + sbr.filename;
        //p555_1[p555_1.length()-3] = p555_1[p555_1.length()-2] = p555_1[p555_1.length()-1] = '5';
        //p555_2[p555_2.length()-3] = p555_2[p555_2.length()-2] = p555_2[p555_2.length()-1] = '5';

        //Logger::warning( "File %s maybe in external 555 file: %s", name.c_str(), p555.toString().c_str() );
      }
      else
      {
        p555 = file.path().changeExtension( arch555ext );
      }

      if( !p555.exist() )
      {
          Logger::warning("Cannot found 555 file for image %s in file %s", name.c_str(), p555.toString().c_str());
          continue; // skip to next bitmap
      }

      _fileInfo[name];
      _fileInfo[name].fn = p555.toString();
      _fileInfo[name].sr = sir;

      addItem( name, sir.offset, sir.length, false);
    } // image loop
  } // bitmap loop
  sort();
}

Sg2ArchiveReader::~Sg2ArchiveReader() {}

const std::string &Sg2ArchiveReader::getTypeName() const { return readerTypename;}
Archive::Type Sg2ArchiveReader::getType() const{  return Archive::sg2;}
const Entries* Sg2ArchiveReader::entries() const{  return this;}

NFile Sg2ArchiveReader::createAndOpenFile(unsigned int index)
{
  FileInfo::iterator it = _fileInfo.begin();
  std::advance( it, index );
  return NFile();
}

void Sg2ArchiveReader::_loadSpriteImage( Picture& img, const SgFileEntry& rec)
{
	ByteArray buffer = _readData( rec );
	_writeTransparentImage( img, (unsigned char*)buffer.data(), rec.sr.length);
}

std::string Sg2ArchiveReader::_find555File( const SgFileEntry& rec )
{
	// Fetch basename of the file
	// either the same name as sg(2|3) or from file record
	Path filename;
  std::string retValue = rec.fn;
	if( rec.sr.flags[0] > 0 )
	{
		filename = rec.fn;
	}
	else
	{
		filename = _file.path();
	}

	// Change the extension to .555
	filename = Path( filename.removeExtension() + ".555" );

	Path path = _findFilenameCaseInsensitive( _file.path().directory(), filename.baseName().toString() );
	if( path.exist() )
	{
		retValue = path.toString();
	}
	else
	{
		path = _findFilenameCaseInsensitive((Path(_file.path().directory()) + Path("/555")).toString(), filename.baseName().toString());    
		if (path.exist())
		{
			retValue = path.toString();
		}
	}

	return retValue;
}

std::string Sg2ArchiveReader::_findFilenameCaseInsensitive( const std::string& dir, std::string filename )
{
  Directory directory( dir );
  filename = utils::localeLower( filename );

  Entries::Items files = directory.getEntries().items();
  for( unsigned int i = 0; i < files.size(); i++)
  {
    if( files[i].name.canonical() == filename )
    {
      return files[i].fullpath.toString();
    }
  }

  return std::string();
}

ByteArray Sg2ArchiveReader::_readData(const SgFileEntry& rec )
{
	std::string filename = rec.fn;
	unsigned int start = rec.sr.offset - rec.sr.flags[0];
	unsigned int data_length = rec.sr.length;

	FileNative z5file( filename, Entity::fmRead );
	if (!z5file.isOpen() )
	{
		Logger::warning( "Unable to open 555 file %s", filename.c_str() );
		return ByteArray();
	}

	z5file.seek( start );
	if( data_length <= 0 )
	{
		Logger::warning( "Data length: %d", data_length); // not an error per se
	}

	ByteArray data = z5file.read( data_length );

	unsigned int real_read = data.size();
	if( real_read + 4 == data_length && z5file.isEof() )
	{
		// Exception for some C3 graphics: last image is 'missing' 4 bytes
		data.resize( real_read + 4 );
		data[real_read] = data[real_read+1] = 0;
		data[real_read+2] = data[real_read+3] = 0;
	}

	return data;
}

void Sg2ArchiveReader::_loadIsometricImage( Picture& pic, const SgFileEntry& rec )
{
	ByteArray buffer = _readData( rec );
	_writeIsometricBase( pic, rec.sr, (unsigned char*)buffer.data() );
	_writeTransparentImage( pic, (unsigned char*)(&buffer[0] + rec.sr.uncompressed_length),
				rec.sr.length - rec.sr.uncompressed_length);
}

void Sg2ArchiveReader::_writeIsometricBase( Picture& img, const SgImageRecord& rec, const unsigned char* buffer )
{
	int i = 0, x, y;
	int width, height, height_offset;
	int size = rec.flags[3];
	int x_offset, y_offset;
	int tile_bytes, tile_height, tile_width;

	width = img.width();
	height = (width + 2) / 2; /* 58 -> 30, 118 -> 60, etc */
	height_offset = img.height() - height;
	y_offset = height_offset;

	if (size == 0) {
		/* Derive the tile size from the height (more regular than width) */
		/* Note that this causes a problem with 4x4 regular vs 3x3 large: */
		/* 4 * 30 = 120; 3 * 40 = 120 -- give precedence to regular */
		if (height % ISOMETRIC_TILE_HEIGHT == 0)
		{
			size = height / ISOMETRIC_TILE_HEIGHT;
		}
		else if (height % ISOMETRIC_LARGE_TILE_HEIGHT == 0)
		{
			size = height / ISOMETRIC_LARGE_TILE_HEIGHT;
		}
	}

	/* Determine whether we should use the regular or large (emperor) tiles */
	if (ISOMETRIC_TILE_HEIGHT * size == height)
	{
		/* Regular tile */
		tile_bytes  = ISOMETRIC_TILE_BYTES;
		tile_height = ISOMETRIC_TILE_HEIGHT;
		tile_width  = ISOMETRIC_TILE_WIDTH;
	}
	else if (ISOMETRIC_LARGE_TILE_HEIGHT * size == height)
	{
		/* Large (emperor) tile */
		tile_bytes  = ISOMETRIC_LARGE_TILE_BYTES;
		tile_height = ISOMETRIC_LARGE_TILE_HEIGHT;
		tile_width  = ISOMETRIC_LARGE_TILE_WIDTH;
	}
	else
	{
		Logger::warning( "Unknown tile size: %d (height %d, width %d, size %d)",
											2 * height / size, height, width, size );
		return;
	}

	/* Check if buffer length is enough: (width + 2) * height / 2 * 2bpp */
	if( (width + 2) * height != (int)rec.uncompressed_length)
	{
		Logger::warning(
			"Data length doesn't match footprint size: %0 vs %1 (%2) %3",
			(width + 2) * height,
			rec.uncompressed_length,
			rec.length,
			rec.invert_offset );
		return;
	}

	i = 0;
	for (y = 0; y < (size + (size - 1)); y++)
	{
		x_offset = (y < size ? (size - y - 1) : (y - size + 1)) * tile_height;
		for (x = 0; x < (y < size ? y + 1 : 2 * size - y - 1); x++, i++)
		{
			_writeIsometricTile( img, &buffer[i * tile_bytes],
					     x_offset, y_offset, tile_width, tile_height);
			x_offset += tile_width + 2;
		}
		y_offset += tile_height / 2;
	}
}

void Sg2ArchiveReader::_writeIsometricTile( Picture& img, const unsigned char* buffer,
																						int offset_x, int offset_y,
																						int tile_width, int tile_height )
{
	int half_height = tile_height / 2;
	int x, y, i = 0;

	unsigned int* pixels = img.lock();

	for (y = 0; y < half_height; y++)
	{
		int start = tile_height - 2 * (y + 1);
		int end = tile_width - start;
		for (x = start; x < end; x++, i += 2)
		{
			unsigned int* bufp32;
			bufp32 = pixels + (offset_y + y) * img.width() + offset_x + x;
			*bufp32 = _555toRGBA( (buffer[i+1] << 8) | buffer[i] );
		}
	}

	for (y = half_height; y < tile_height; y++)
	{
		int start = 2 * y - tile_height;
		int end = tile_width - start;
		for (x = start; x < end; x++, i += 2)
		{
			unsigned int* bufp32;
			bufp32 = pixels + (offset_y + y) * img.width() + offset_x + x;
			*bufp32 = _555toRGBA( (buffer[i+1] << 8) | buffer[i] );
		}
	}

	img.unlock();
}

void Sg2ArchiveReader::_writeTransparentImage( Picture& img, const unsigned char* buffer, int length)
{
	int i = 0;
	int x = 0, y = 0, j;
	int width = img.width();

	unsigned int* pixels = img.lock();
	while (i < length)
	{
		unsigned char c = buffer[i++];
		if (c == 255)
		{
			/* The next byte is the number of pixels to skip */
			x += buffer[i++];
			while (x >= width)
			{
				y++; x -= width;
			}
		}
		else
		{
			/* `c' is the number of image data bytes */
			for (j = 0; j < c; j++, i += 2)
			{
				unsigned int* bufp32;
				bufp32 = pixels + y * img.width() + x;
				*bufp32 = _555toRGBA( buffer[i] | (buffer[i+1] << 8) );

				x++;
				if (x >= width)
				{
					y++; x = 0;
				}
			}
		}
	}
	img.unlock();
}

void Sg2ArchiveReader::_loadPlainImage( Picture& pic, const SgFileEntry& rec)
{
	// Check whether the image data is OK
	if (rec.sr.height * rec.sr.width * 2 != (int)rec.sr.length)
	{
		Logger::warning( "Image data length doesn't match image size" );
		return;
	}

	unsigned int need_length = rec.sr.length;
	ByteArray data = _readData( rec );
	if( data.size() != need_length )
	{
		Logger::warning( "Unable to read %d bytes from file (read %d bytes)",
										 data.size(), need_length );
		return;
	}

	int i = 0;
	unsigned int* pixels = pic.lock();

	unsigned char* rdata = (unsigned char*)data.data();
	for (int y = 0; y < (int)rec.sr.height; y++)
	{
		for (int x = 0; x < (int)rec.sr.width; x++, i+= 2)
		{
			unsigned int* bufp32;
			bufp32 = pixels + y * pic.width() + x;
			*bufp32 = _555toRGBA( rdata[i] | (rdata[i+1] << 8) );
		}
	}

	pic.unlock();
}

unsigned int Sg2ArchiveReader::_555toRGBA( unsigned short color)
{
	if(color == 0xf81f)
	{
		return 0;
	}

	NColor rgb( 0xff000000 );

	// Red: bits 11-15, should go to bits 17-24
	unsigned int red = ((color & 0x7c00) << 9) | ((color & 0x7000) << 4);

	// Green: bits 6-10, should go to bits 9-16
	unsigned int green = ((color & 0x3e0) << 6) | ((color & 0x300));

	// Blue: bits 1-5, should go to bits 1-8
	unsigned int blue = ((color & 0x1f) << 3) | ((color & 0x1c) >> 2);

	rgb.setRed( (red >> 16) & 0xff );
	rgb.setGreen( (green >> 8) & 0xff );
	rgb.setBlue( blue & 0xff );

	return rgb.abgr();
}

NFile Sg2ArchiveReader::createAndOpenFile(const Path& filename)
{
  FileInfo::iterator it = _fileInfo.find( filename.toString() );

  if( it != _fileInfo.end() )
  {
    SgImageRecord& sir = it->second.sr;

    PictureRef result;
    result.init( Size( sir.width, sir.height ) );
    result->fill( 0, Rect() ); // Transparent black

    switch(sir.type)
    {
      case 0:
      case 1:
      case 10:
      case 12:
      case 13:
        //Logger::warning( "Find plain image " + filename.toString() );
        _loadPlainImage( *result, it->second );
        //PictureConverter::save( *result, "base/" + filename.removeExtension() + ".png" );
        break;

      case 30:
        //Logger::warning( "Find isometric image " + filename.toString() );
        _loadIsometricImage( *result, it->second );
        //PictureConverter::save( *result, "base/" + filename.removeExtension() + ".png" );
      break;

      case 256:
      case 257:
      case 276:
        //Logger::warning( "Find sprite image " + filename.toString() );
        _loadSpriteImage( *result, it->second );
        //PictureConverter::save( *result, "base/" + filename.removeExtension() + ".png" );
      break;

      default:
        Logger::warning("Unknown image type: %d", sir.type);
      break;
    }

    ByteArray data = PictureConverter::save( *result, "PNG" );
    NFile memfile = MemoryFile::create( data, filename );
    return memfile;

    /*if( sir.alpha_length )
    {
      quint8 *alpha_buffer = &(buffer[workRecord->length]);
      loadAlphaMask(&result, alpha_buffer);
    }*/
  }

  return NFile();
}

}//end namespace vfs
