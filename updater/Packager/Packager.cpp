#include "Packager.h"

#include "../Constants.h"
#include "../Util.h"

#include "vfs/filelist.hpp"

Packager::Packager(std::string baseset, std::string currentVersion)
  : _baseset( baseset ), _crver( currentVersion )
{
}

typedef std::vector<io::FilePath> FilePathList;

static void __gartherFiles( io::FileDir dir, FilePathList& files )
{
  io::FileList entries = dir.getEntries();
  for( io::FileList::ConstItemIt i=entries.begin(); i != entries.end(); i++ )
  {

  }
}

void Packager::run()
{
  FilePathList allFiles;

  io::FileDir dir( _baseset );

}


