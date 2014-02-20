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

#include "archive_sg2.hpp"

#include "core/stringhelper.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>

vfs::Sg2ArchiveLoader::Sg2ArchiveLoader(vfs::FileSystem* fileSystem) :
  _fileSystem(fileSystem)
{
}

bool vfs::Sg2ArchiveLoader::isALoadableFileFormat(const Path& filename) const
{
  std::string fileExtension = filename.getExtension();
  return StringHelper::isEquale( fileExtension, ".sg2", StringHelper::equaleIgnoreCase );
}

bool vfs::Sg2ArchiveLoader::isALoadableFileFormat(vfs::NFile file) const
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

bool vfs::Sg2ArchiveLoader::isALoadableFileFormat(vfs::Archive::Type fileType) const
{
  return fileType == Archive::sg2;
}

vfs::ArchivePtr vfs::Sg2ArchiveLoader::createArchive(const Path& filename, bool ignoreCase, bool ignorePaths) const
{
  ArchivePtr archive;
  NFile file = _fileSystem->createAndOpenFile(filename, Entity::fmRead );

  if( file.isOpen() )
  {
    archive = createArchive(file, ignoreCase, ignorePaths);
  }

  return archive;
}

vfs::ArchivePtr vfs::Sg2ArchiveLoader::createArchive(vfs::NFile file, bool ignoreCase, bool ignorePaths) const
{
  ArchivePtr archive;
  if( file.isOpen() )
  {
    file.seek(0);

    SgHeader header;
    file.read(&header, sizeof(header));

    //archive = new Sg2ArchiveReader(file);
    archive->drop();
  }
  return archive;
}

vfs::Sg2ArchiveReader::Sg2ArchiveReader(NFile file)
{
  SgHeader header;
  file.seek(0);
  file.read(&header, sizeof(header));
  file.seek(680);

  // Read bitmaps
  for (int i = 0; i < header.num_bitmap_records; ++i)
  {
    SgBitmapRecord sbr;
    file.read(&sbr,sizeof(sbr));
    // Terminate strings
    sbr.filename[64] = 0;
    sbr.comment[50] = 0;

    std::string bmp_name_full = sbr.filename;
    std::string bmp_name = bmp_name_full.substr(0, bmp_name_full.length() - 4);

    // Load images
    for(uint32_t i = sbr.start_index; i < sbr.end_index; ++i)
    {
      SgImageRecord sir;
      file.seek(680 + 100*200 + (i * sizeof(SgImageRecord)));
      file.read(&sir, sizeof(sir));

      // FIXME: Skip non-plain images
      if (sir.type > 13)
        continue;
      //FIXME: Skip external 555 files
      if (sir.flags[0] != 0)
        continue;

      // Locate appropriate 555 file
      std::string p555;
      if (sir.flags[0])
      {
        p555 = file.getFileName().directory();
        p555.push_back('/');
        std::string p555_1 = p555 + sbr.filename;
        std::string p555_2 = p555 + "555/" + sbr.filename;
        p555_1[p555_1.length()-3] = p555_1[p555_1.length()-2] = p555_1[p555_1.length()-1] = '5';
        p555_2[p555_2.length()-3] = p555_2[p555_2.length()-2] = p555_2[p555_2.length()-1] = '5';

        if ( Path(p555_2).exist())
          p555 = p555_2;
        else if (Path(p555_2).exist())
          p555 = p555_2;
        else
          continue; // skip to next bitmap
      }
      else
      {
        p555 = file.getFileName().toString();
        p555[p555.length()-3] = p555[p555.length()-2] = p555[p555.length()-1] = '5';

        if (!Path(p555).exist())
          continue; // skip to next bitmap
      }

      // Construct name
      std::ostringstream oss;
      oss << bmp_name << '_';
      oss << std::setfill('0') << std::setw(6) << i - sbr.start_index;
      oss << ".bmp";

      FileInfo[oss.str()];
      FileInfo[oss.str()].fileName555 = p555;
      FileInfo[oss.str()].sgImageRecord = sir;

      this->addItem(oss.str(), sir.offset, sir.length, false);
      std::cout << oss.str() << std::endl;
    } // image loop
  } // bitmap loop
  sort();
}

vfs::Sg2ArchiveReader::~Sg2ArchiveReader()
{
}

std::string vfs::Sg2ArchiveReader::getTypeName() const
{
  return "Sg2Reader";
}

vfs::Archive::Type vfs::Sg2ArchiveReader::getType() const
{
  return Archive::sg2;
}

const vfs::Entries* vfs::Sg2ArchiveReader::getFileList() const
{
  return this;
}

vfs::NFile vfs::Sg2ArchiveReader::createAndOpenFile(unsigned int index)
{
  // TODO: Implement createandopenfile
  return NFile();
}

vfs::NFile vfs::Sg2ArchiveReader::createAndOpenFile(const Path& filename)
{
  // TODO: Implement createandopenfile
  return NFile();
}


