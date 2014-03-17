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

#ifndef _CAESARIA_ARCHIVE_SG2_HPP_INCLUDE_
#define _CAESARIA_ARCHIVE_SG2_HPP_INCLUDE_

#include "filesystem.hpp"
#include "file.hpp"
#include "archive.hpp"
#include "entries.hpp"

#include <map>

class Picture;

namespace vfs
{

typedef u_int32_t uint32_t;
typedef u_int16_t uint16_t;
typedef u_int8_t uint8_t;
typedef int int32_t;
typedef short int16_t;

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif


struct SgHeader
{
    uint32_t sg_filesize;
    uint32_t version;
    uint32_t unknown1;

    int32_t max_image_records;
    int32_t num_image_records;
    int32_t num_bitmap_records;
    int32_t num_bitmap_records_without_system;

    uint32_t total_filesize;
    uint32_t filesize_555;
    uint32_t filesize_external;
} PACK_STRUCT;

struct SgBitmapRecord
{
    char filename[65];
    char comment[51];
    uint32_t width;
    uint32_t height;
    uint32_t num_images;
    uint32_t start_index;
    uint32_t end_index;
    /* 4 bytes - quint32 between start & end */
    /* 16b, 4x int with unknown purpose */
    /*  8b, 2x int with (real?) width & height */
    /* 12b, 3x int: if any is non-zero: internal image */
    /* 24 more misc bytes, most zero */
    char misc[64];
} PACK_STRUCT;

struct SgImageRecord
{
    uint32_t offset;
    uint32_t length;
    uint32_t uncompressed_length;
    uint32_t unknnown1;
    int32_t invert_offset;
    int16_t width;
    int16_t height;
    char unknown2[26];
    uint16_t type;
    char flags[4];
    uint8_t bitmap_id;
    /* 3 bytes + 4 zero bytes */
    uint8_t unknnown3[7];
    /* For D6 and up SG3 versions: alpha masks */
    //uint32_t alpha_offset;
    //uint32_t alpha_length;
} PACK_STRUCT;


#if defined(_MSC_VER) || defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
# pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

//! Loader for original sg2 files shipped with Caesar3
class Sg2ArchiveLoader : public ArchiveLoader
{
public:
    Sg2ArchiveLoader(vfs::FileSystem* fs);

    virtual bool isALoadableFileFormat(const Path &filename) const;
    virtual bool isALoadableFileFormat(NFile file) const;
    virtual bool isALoadableFileFormat(Archive::Type fileType) const;

    virtual ArchivePtr createArchive(const Path &filename, bool ignoreCase, bool ignorePaths) const;
    virtual ArchivePtr createArchive(NFile file, bool ignoreCase, bool ignorePaths) const;
private:
    vfs::FileSystem* _fileSystem;
};

struct SgFileEntry
{
  std::string fn;
  SgImageRecord sr;
};

class Sg2ArchiveReader : public virtual Archive, virtual Entries
{
public:
  Sg2ArchiveReader(NFile file);
  ~Sg2ArchiveReader();

  virtual NFile createAndOpenFile(const Path& filename);
  virtual NFile createAndOpenFile( unsigned int index);
  virtual const Entries* getFileList() const;
  virtual std::string getTypeName() const;

  Archive::Type getType() const;

private:
  //std::map<std::string, std::ifstream> _555List;
  typedef std::map<std::string, SgFileEntry> FileInfo;
  FileInfo _fileInfo;
  NFile _file;

  void _loadSpriteImage(Picture& img, const SgFileEntry& rec);
  void _writeTransparentImage(Picture& img, const unsigned char* buffer, int length);
  void _writeIsometricTile(Picture& img, const unsigned char* buffer, int offset_x, int offset_y, int tile_width, int tile_height);
  void _writeIsometricBase(Picture& img, const SgImageRecord& rec, const unsigned char* buffer);
  ByteArray _readData( const SgFileEntry& rec );
  void _loadIsometricImage(Picture& pic, const SgFileEntry& rec);
  void _loadPlainImage( Picture& pic, const SgFileEntry& rec);
  void _set555Pixel( Picture& img, int x, int y, unsigned short color);
  std::string _findFilenameCaseInsensitive(std::string directory, std::string filename);
  std::string _find555File(const SgFileEntry& rec);
}; // class Sg2ArchiveReader

} //end namespace vfs

#endif // _CAESARIA_ARCHIVE_SG2_HPP_INCLUDE_
