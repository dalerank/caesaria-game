#include "packager.hpp"

#include "constants.hpp"
#include "util.hpp"
#include "inifile.hpp"
#include "core/stringhelper.hpp"

#include "vfs/file.hpp"
#include "vfs/directory.hpp"
#include "vfs/entries.hpp"
#include "vfs/entryinfo.hpp"

namespace updater
{

Packager::Packager(std::string baseset, std::string currentVersion)
  : _baseset( baseset ), _crver( currentVersion )
{
}

typedef std::vector<vfs::Path> FilePathList;

static void __gartherFiles( vfs::Directory basedir, vfs::Directory dir, FilePathList& files )
{
  vfs::Entries entries = dir.getEntries();
  for( vfs::Entries::ConstItemIt i=entries.begin(); i != entries.end(); i++ )
  {
    if( i->name.isDirectoryEntry() )
      continue;

    if( i->fullName.isFolder() )
    {
      __gartherFiles( basedir, i->fullName, files );
    }
    else
    {
      files.push_back( basedir.getRelativePathTo( i->fullName ) );
    }
  }
}

void Packager::createUpdate( bool release )
{
  FilePathList allFiles;

  vfs::Directory::changeCurrentDir( _baseset );
  vfs::Directory dir = vfs::Directory::getCurrent();

  __gartherFiles( dir, dir, allFiles );

  IniFilePtr vinfo = IniFile::Create();
  vinfo->SetValue( "version", "version", _crver );
  for( FilePathList::iterator i=allFiles.begin(); i != allFiles.end(); i++ )
  {
    std::string sectionName;
    if( release )
    {
      sectionName = StringHelper::format( 0xff, "File %s", (*i).toString().c_str() );
    }
    else
    {
      sectionName = StringHelper::format( 0xff, "Version%s File %s", _crver.c_str(), (*i).toString().c_str() );
    }

    ByteArray data = vfs::NFile::open( (*i).getAbsolutePath() ).readAll();

    unsigned int crc = data.crc32( 0 );
    vinfo->SetValue( sectionName, "crc", StringHelper::format( 0xff, "%x", crc ) );
    vinfo->SetValue( sectionName, "filesize", StringHelper::format( 0xff, "%d", data.size() ) );    
  }

  vinfo->ExportToFile( release
                        ? dir/STABLE_VERSION_FILE
                        : dir/UPDATE_VERSION_FILE );
}


}//end namespace updater
