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

#include "archive_zip.hpp"

#include "entries.hpp"
#include "memfile.hpp"
#include "core/logger.hpp"

#include <zlib.h>
#include <LzmaDec.h>
#include <bzlib.h>
#include <fileenc.h>
#include "core/utils.hpp"

namespace vfs
{

namespace {
static const std::string readerTypename = CAESARIA_STR_EXT(ZipArchiveReader);
}

ZipArchiveLoader::ZipArchiveLoader(vfs::FileSystem* fs)
: _fileSystem(fs)
{
	#ifdef _DEBUG
        setDebugName("ZipLoader");
	#endif
}

//! returns true if the file maybe is able to be loaded by this class
bool ZipArchiveLoader::isALoadableFileFormat(const Path& filename) const
{
    std::string fileExtension = filename.extension();
    Logger::warning( "ZipArchiveLoader: extension is " + fileExtension );
    return utils::isEquale( fileExtension, ".zip", utils::equaleIgnoreCase )
           || utils::isEquale( fileExtension, ".pk3", utils::equaleIgnoreCase )
           || utils::isEquale( fileExtension, ".gz", utils::equaleIgnoreCase )
           || utils::isEquale( fileExtension, ".tgz", utils::equaleIgnoreCase );
}

//! Check to see if the loader can create archives of this type.
bool ZipArchiveLoader::isALoadableFileFormat( Archive::Type fileType) const
{
    return (fileType == Archive::zip || fileType == Archive::gzip );
}


//! Creates an archive from the filename
/** \param file File handle to check.
\return Pointer to newly created archive, or 0 upon error. */
ArchivePtr ZipArchiveLoader::createArchive(const Path& filename, bool ignoreCase, bool ignorePaths) const
{
  ArchivePtr archive;
  NFile file = _fileSystem->createAndOpenFile(filename, Entity::fmRead );

  if( file.isOpen() )
  {
    archive = createArchive(file, ignoreCase, ignorePaths);
  }

  return archive;
}

//! creates/loads an archive from the file.
//! \return Pointer to the created archive. Returns 0 if loading failed.
ArchivePtr ZipArchiveLoader::createArchive( NFile file, bool ignoreCase, bool ignorePaths) const
{
  if( file.isOpen() )
  {
    file.seek(0);

    unsigned short sig;
    file.read( &sig, 2);

    file.seek(0);

    bool isGZip = (sig == 0x8b1f);

    ArchivePtr archive( new ZipArchiveReader( file, ignoreCase, ignorePaths, isGZip ) );
    archive->drop();
    return archive;
  }

  return ArchivePtr();
}

//! Check if the file might be loaded by this class
/** Check might look into the file.
\param file File handle to check.
\return True if file seems to be loadable. */
bool ZipArchiveLoader::isALoadableFileFormat( NFile file ) const
{
  SZIPFileHeader header;

  file.read( &header.Sig, 4 );

  return header.Sig == 0x04034b50 || // ZIP
         (header.Sig&0xffff) == 0x8b1f; // gzip
}

// -----------------------------------------------------------------------------
// zip archive
// -----------------------------------------------------------------------------
class ZipArchiveReader::Impl
{
public:
  bool isGZip;
};

ZipArchiveReader::ZipArchiveReader( NFile file, bool ignoreCase, bool ignorePaths, bool isGZip)
 : Entries( (file.isOpen() ? file.path() : Path("") ), ignoreCase ? Path::ignoreCase : Path::equaleCase, ignorePaths),
   File(file), _d( new Impl )
{
  #ifdef _DEBUG
    //setDebugName("ZipReader");
  #endif

  _d->isGZip = isGZip;

  if( file.isOpen() )
	{
		// load file entries
    if( _d->isGZip )
			while (scanGZipHeader()) { }
		else
			while (scanZipHeader()) { }

		sort();
	}
}

ZipArchiveReader::~ZipArchiveReader(){}

//! get the archive type
Archive::Type ZipArchiveReader::getType() const
{
  return _d->isGZip
            ? Archive::gzip
            : Archive::zip;
}

const Entries* ZipArchiveReader::entries() const {	return this; }


//! scans for a local header, returns false if there is no more local file header.
//! The gzip file format seems to think that there can be multiple files in a gzip file
//! but none
bool ZipArchiveReader::scanGZipHeader()
{
	SZipFileEntry entry;
	entry.Offset = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	// read header
	SGZIPMemberHeader header;
    if (File.read(&header, sizeof(SGZIPMemberHeader)) == sizeof(SGZIPMemberHeader))
	{
		// check header value
		if (header.sig != 0x8b1f)
			return false;

		// now get the file info
		if (header.flags & EGZF_EXTRA_FIELDS)
		{
			// read lenth of extra data
            unsigned short dataLen;

            File.read(&dataLen, 2);

/*#ifdef __BIG_ENDIAN__
			dataLen = os::Byteswap::byteswap(dataLen);
#endif*/

			// skip it
            File.seek(dataLen, true);
		}

        Path ZipFileName = "";

		if (header.flags & EGZF_FILE_NAME)
		{
            char c;
            File.read(&c, 1);
			while (c)
			{
                ZipFileName += c;
                File.read(&c, 1);
			}
		}
		else
		{
            ZipFileName = File.path().baseName();

			// rename tgz to tar or remove gz extension
            int length = ZipFileName.toString().size();
            if( ZipFileName.isMyExtension( "tgz", false ) )
			{
                ZipFileName[ length - 2 ] = 'a';
                ZipFileName[ length - 1 ] = 'r';
			}
            else if ( ZipFileName.extension(), "gz" )
			{
                ZipFileName[ length - 3] = 0;
			}
		}

		if (header.flags & EGZF_COMMENT)
		{
            char c='a';
			while (c)
            {
                File.read(&c, 1);
            }
		}

		if (header.flags & EGZF_CRC16)
        {
            File.seek(2, true);
        }

		// we are now at the start of the data blocks
        entry.Offset = File.getPos();

        entry.header.FilenameLength = ZipFileName.toString().size();

		entry.header.CompressionMethod = header.compressionMethod;
        entry.header.DataDescriptor.CompressedSize = (File.size() - 8) - File.getPos();

		// seek to file end
        File.seek(entry.header.DataDescriptor.CompressedSize, true);

		// read CRC
        File.read(&entry.header.DataDescriptor.CRC32, 4);
		// read uncompressed size
        File.read(&entry.header.DataDescriptor.UncompressedSize, 4);

		// now we've filled all the fields, this is just a standard deflate block
		addItem(ZipFileName, entry.Offset, entry.header.DataDescriptor.UncompressedSize, false, 0);
		FileInfo.push_back(entry);
	}

	// there's only one block of data in a gzip file
	return false;
}

//! scans for a local header, returns false if there is no more local file header.
bool ZipArchiveReader::scanZipHeader(bool ignoreGPBits)
{
    Path ZipFileName = "";
	SZipFileEntry entry;
	entry.Offset = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

    File.read(&entry.header, sizeof(SZIPFileHeader));

	if (entry.header.Sig != 0x04034b50)
    {
		return false; // local file headers end here.
    }

	// read filename
	{
        ByteArray tmp;
        tmp.resize( entry.header.FilenameLength + 2);
        File.read(tmp.data(), entry.header.FilenameLength);
        tmp[ entry.header.FilenameLength ] = 0;
        ZipFileName = tmp.data();
	}

	// AES encryption
	if ((entry.header.GeneralBitFlag & ZIP_FILE_ENCRYPTED) && (entry.header.CompressionMethod == 99))
	{
        short restSize = entry.header.ExtraFieldLength;
		SZipFileExtraHeader extraHeader;
		while (restSize)
		{
            File.read(&extraHeader, sizeof(extraHeader));

			restSize -= sizeof(extraHeader);
            if (extraHeader.ID==(short)0x9901)
			{
				SZipFileAESExtraData data;
                File.read(&data, sizeof(data));

				restSize -= sizeof(data);
				if (data.Vendor[0]=='A' && data.Vendor[1]=='E')
				{
					// encode values into Sig
					// AE-Version | Strength | ActualMode
					entry.header.Sig =
						((data.Version & 0xff) << 24) |
						(data.EncryptionStrength << 16) |
						(data.CompressionMode);
                    File.seek(restSize, true);
					break;
				}
			}
		}
	}
	// move forward length of extra field.
	else
	{
		if (entry.header.ExtraFieldLength)
        {
            File.seek(entry.header.ExtraFieldLength, true);
        }
	}
	// if bit 3 was set, use CentralDirectory for setup
	if (!ignoreGPBits && entry.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRIPTOR)
	{
		SZIPFileCentralDirEnd dirEnd;
		FileInfo.clear();
        _items().clear();
		// First place where the end record could be stored
        File.seek(File.size()-22);
		const char endID[] = {0x50, 0x4b, 0x05, 0x06, 0x0};
		char tmp[5]={'\0'};
		bool found=false;
		// search for the end record ID
        while (!found && File.getPos()>0)
		{
			int seek=8;
            File.read(tmp, 4);
			switch (tmp[0])
			{
			case 0x50:
				if (!strcmp(endID, tmp))
				{
					seek=4;
					found=true;
				}
				break;
			case 0x4b:
				seek=5;
				break;
			case 0x05:
				seek=6;
				break;
			case 0x06:
				seek=7;
				break;
			}
            File.seek(-seek, true);
		}
        File.read(&dirEnd, sizeof(dirEnd));

        FileInfo.resize( dirEnd.TotalEntries );
        File.seek(dirEnd.Offset);
		while (scanCentralDirectoryHeader()) { }
		return false;
	}

	// store position in file
    entry.Offset = File.getPos();
	// move forward length of data
    File.seek(entry.header.DataDescriptor.CompressedSize, true);

    char lastChar = *ZipFileName.toString().rbegin();
    addItem(ZipFileName, entry.Offset, entry.header.DataDescriptor.UncompressedSize, lastChar=='/', FileInfo.size());
	FileInfo.push_back(entry);

	return true;
}


//! scans for a local header, returns false if there is no more local file header.
bool ZipArchiveReader::scanCentralDirectoryHeader()
{
    Path ZipFileName = "";
	SZIPFileCentralDirFileHeader entry;
    File.read(&entry, sizeof(SZIPFileCentralDirFileHeader));

	if (entry.Sig != 0x02014b50)
    {
		return false; // central dir headers end here.
    }

    const long pos = File.getPos();
    File.seek(entry.RelativeOffsetOfLocalHeader);
	scanZipHeader(true);
    File.seek(pos+entry.FilenameLength+entry.ExtraFieldLength+entry.FileCommentLength);
    FileInfo.back().header.DataDescriptor.CompressedSize=entry.CompressedSize;
    FileInfo.back().header.DataDescriptor.UncompressedSize=entry.UncompressedSize;
    FileInfo.back().header.DataDescriptor.CRC32=entry.CRC32;
    _items().back().size=entry.UncompressedSize;
	return true;
}


//! opens a file by file name
NFile ZipArchiveReader::createAndOpenFile(const Path& filename)
{
	int index = findFile( filename, false );

  if (index != -1)
  {
    return createAndOpenFile(index);
  }

  return NFile();
}

//! Used for LZMA decompression. The lib has no default memory management
namespace
{
  void *SzAlloc(void *p, size_t size) { p; return malloc(size); }
  void SzFree(void *p, void *address) { p; free(address); }
  ISzAlloc lzmaAlloc = { SzAlloc, SzFree };
}

//! opens a file by index
NFile ZipArchiveReader::createAndOpenFile(unsigned int index)
{
  //Supports 0, 8, 12, 14, 99
  //0 - The file is stored (no compression)
  //1 - The file is Shrunk
  //2 - The file is Reduced with compression factor 1
  //3 - The file is Reduced with compression factor 2
  //4 - The file is Reduced with compression factor 3
  //5 - The file is Reduced with compression factor 4
  //6 - The file is Imploded
  //7 - Reserved for Tokenizing compression algorithm
  //8 - The file is Deflated
  //9 - Reserved for enhanced Deflating
  //10 - PKWARE Date Compression Library Imploding
  //12 - bzip2 - Compression Method from libbz2, WinZip 10
  //14 - LZMA - Compression Method, WinZip 12
  //96 - Jpeg compression - Compression Method, WinZip 12
  //97 - WavPack - Compression Method, WinZip 11
  //98 - PPMd - Compression Method, WinZip 10
  //99 - AES encryption, WinZip 9

  const SZipFileEntry &e = FileInfo[ _items()[index].uid ];

  short actualCompressionMethod=e.header.CompressionMethod;
  NFile decrypted;
  ByteArray decryptedBuf;
  unsigned int decryptedSize=e.header.DataDescriptor.CompressedSize;

  if ((e.header.GeneralBitFlag & ZIP_FILE_ENCRYPTED) && (e.header.CompressionMethod == 99))
  {
    Logger::warning( "Reading encrypted file." );
    unsigned char salt[16]={0};
    const unsigned short saltSize = (((e.header.Sig & 0x00ff0000) >>16)+1)*4;
    File.seek(e.Offset);
    File.read(salt, saltSize);
    char pwVerification[2];
    char pwVerificationFile[2];

    File.read(pwVerification, 2);
    fcrypt_ctx zctx; // the encryption context
    int rc = fcrypt_init( (e.header.Sig & 0x00ff0000) >>16,
                          (const unsigned char*)Password.c_str(), // the password
                          Password.size(), // number of bytes in password
                          salt, // the salt
                          (unsigned char*)pwVerificationFile, // on return contains password verifier
                          &zctx); // encryption context
    if (strncmp(pwVerificationFile, pwVerification, 2))
    {
      Logger::warning( "Wrong password" );
      return NFile();
    }
    decryptedSize= e.header.DataDescriptor.CompressedSize-saltSize-12;
    decryptedBuf.resize( decryptedSize );
    unsigned int c = 0;
    while ((c+32768)<=decryptedSize)
    {
      File.read(decryptedBuf.data()+c, 32768);
      fcrypt_decrypt( (unsigned char*)decryptedBuf.data()+c, // pointer to the data to decrypt
                      32768,   // how many bytes to decrypt
                      &zctx); // decryption context
                      c+=32768;
    }

    File.read(decryptedBuf.data()+c, decryptedSize-c);
    fcrypt_decrypt( (unsigned char*)decryptedBuf.data()+c, // pointer to the data to decrypt
                    decryptedSize-c,   // how many bytes to decrypt
                    &zctx); // decryption context

    char fileMAC[10];
    char resMAC[10];
    rc = fcrypt_end( (unsigned char*)resMAC, // on return contains the authentication code
                    &zctx); // encryption context
    if (rc != 10)
    {
      Logger::warning( "Error on encryption closing" );
      return NFile();
    }

    File.read(fileMAC, 10);
    if (strncmp(fileMAC, resMAC, 10))
    {
      Logger::warning( "Error on encryption check" );
      return NFile();
    }

    decrypted = MemoryFile::create( decryptedBuf, item( index ).fullpath );
    actualCompressionMethod = (e.header.Sig & 0xffff);
#if 0
    if ((e.header.Sig & 0xff000000)==0x01000000)
    {
    }
    else if ((e.header.Sig & 0xff000000)==0x02000000)
    {
    }
    else
    {
            os::Printer::log("Unknown encryption method");
            return 0;
    }
#endif
  }

  switch(actualCompressionMethod)
  {
    case 0: // no compression
    {
      if( decrypted.isOpen() )
          return decrypted;
      else
      {
          File.seek( e.Offset );
          ByteArray data = File.read( decryptedSize );
          return MemoryFile::create( data, item( index ).fullpath );
      }
    }
    break;

    case 8:
    {
      const unsigned int uncompressedSize = e.header.DataDescriptor.UncompressedSize;
      ByteArray pBuf;
      pBuf.resize( uncompressedSize );

      ByteArray pcData;
      pcData.resize( decryptedSize );

      if( pBuf.empty() || pcData.empty() )
      {
        Logger::warning( "Not enough memory for decompressing " + item( index ).fullpath.toString() );
        return NFile();
      }

      File.seek(e.Offset);
      pcData = File.read(decryptedSize);

      // Setup the inflate stream.
      z_stream stream;
      int err;

      stream.next_in = (Bytef*)pcData.data();
                        stream.avail_in = (uInt)decryptedSize;
      stream.next_out = (Bytef*)pBuf.data();
			stream.avail_out = uncompressedSize;
			stream.zalloc = (alloc_func)0;
			stream.zfree = (free_func)0;

      // Perform inflation. wbits < 0 indicates no zlib header inside the data.
      err = inflateInit2(&stream, -MAX_WBITS);
      if (err == Z_OK)
      {
        err = inflate(&stream, Z_FINISH);
        inflateEnd(&stream);

	if (err == Z_STREAM_END)
			err = Z_OK;
	err = Z_OK;
	inflateEnd(&stream);
      }

      if (err != Z_OK)
      {
        Logger::warning( "Error decompressing " + item( index ).fullpath.toString() );
        return NFile();
      }
      else
      {
        return MemoryFile::create( pBuf, item( index ).fullpath );
      }
    }
    break;

    case 12:
    {
      const unsigned int uncompressedSize = e.header.DataDescriptor.UncompressedSize;
      ByteArray pBuf;
      pBuf.resize( uncompressedSize );

      ByteArray pcData;
      pcData.resize( decryptedSize );

      if( pBuf.empty() || pcData.empty() )
      {
        Logger::warning( "Not enough memory for decompressing " + item( index ).fullpath.toString() );
        return NFile();
      }

      File.seek(e.Offset);
      pcData = File.read(decryptedSize);

      bz_stream bz_ctx={0};
			/* use BZIP2's default memory allocation
			bz_ctx->bzalloc = NULL;
			bz_ctx->bzfree  = NULL;
			bz_ctx->opaque  = NULL;
			*/
      int err = BZ2_bzDecompressInit(&bz_ctx, 0, 0); /* decompression */
      if(err != BZ_OK)
      {
        Logger::warning( "bzip2 decompression failed. File cannot be read." );
        return NFile();
      }

      bz_ctx.next_in = (char*)pcData.data();
			bz_ctx.avail_in = decryptedSize;
			/* pass all input to decompressor */
      bz_ctx.next_out = pBuf.data();
			bz_ctx.avail_out = uncompressedSize;
			err = BZ2_bzDecompress(&bz_ctx);
			err;

			err = BZ2_bzDecompressEnd(&bz_ctx);

      if (err != BZ_OK)
      {
        Logger::warning( "Error decompressing +" + item( index ).fullpath.toString() );
        return NFile();
      }
      else
      {
        return MemoryFile::create( pBuf, item( index ).fullpath );
      }
    }
    break;

    case 14:
    {
      unsigned int uncompressedSize = e.header.DataDescriptor.UncompressedSize;
      ByteArray pBuf;
      pBuf.resize( uncompressedSize );
      ByteArray pcData;
      pcData.resize( decryptedSize );
      if( pBuf.empty() || pcData.empty() )
      {
        Logger::warning( "Not enough memory for decompressing " + item( index ).fullpath.toString() );
        return NFile();
      }

      File.seek(e.Offset);
      pcData = File.read(decryptedSize);

                          ELzmaStatus status;
                          SizeT tmpDstSize = uncompressedSize;
                          SizeT tmpSrcSize = decryptedSize;

      unsigned int propSize = (pcData[3]<<8)+pcData[2];
      int err = LzmaDecode((Byte*)pBuf.data(), &tmpDstSize,
              (Byte*)pcData.data()+4+propSize, &tmpSrcSize,
              (Byte*)pcData.data()+4, propSize,
                                          e.header.GeneralBitFlag&0x1?LZMA_FINISH_END:LZMA_FINISH_ANY, &status,
                                          &lzmaAlloc);
                          uncompressedSize = tmpDstSize; // may be different to expected value

      if (err != SZ_OK)
      {
        Logger::warning( "Error decompressing " + item( index ).fullpath.toString() );
        return NFile();
      }
      else
      {
        return MemoryFile::create( pBuf, item( index ).fullpath );
      }
    }
    break;

    case 99:
    {
      return NFile();
    }

    default:
    {
      Logger::warning( "file %s has unsupported compression method", item( index ).fullpath.toString().c_str() );
      return NFile();
    }
  }

  Logger::warning( "Can't read file " + item( index ).fullpath.toString() );
  return NFile();
}

const std::string& ZipArchiveReader::getTypeName() const { return readerTypename; }

}
