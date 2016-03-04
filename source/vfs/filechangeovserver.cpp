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
// Copyright 2012-2016 Dalerank, dalerankn8@gmail.com

#include "filechangeovserver.hpp"
#include <GameThreads>
#include <GameLogger>

#include "vfs/fileinfo.hpp"

#ifdef GAME_PLATFORM_WIN
#include "windows.h"
#endif

namespace vfs
{

#ifdef GAME_PLATFORM_WIN
namespace internal
{
int watchDirectory(const char* lpDir)
{
   DWORD dwWaitStatus;
   HANDLE dwChangeHandles[2];
   //_tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

   // Watch the directory for file creation and deletion.

   dwChangeHandles[0] = FindFirstChangeNotification(
      lpDir,                         // directory to watch
      FALSE,                         // do not watch subtree
      FILE_NOTIFY_CHANGE_SIZE); // watch file modify

   if (dwChangeHandles[0] == INVALID_HANDLE_VALUE)
   {
     Logger::warning("ERROR: FindFirstChangeNotification function failed er={}", GetLastError() );
     return 0;
   }

// Watch the subtree for directory creation and deletion.

   dwChangeHandles[1] = FindFirstChangeNotification(
      lpDir,                         // directory to watch
      FALSE,                          // watch the subtree
      FILE_NOTIFY_CHANGE_LAST_WRITE);  // watch file size change

   if (dwChangeHandles[1] == INVALID_HANDLE_VALUE)
   {
     Logger::warning("ERROR: FindFirstChangeNotification function failed er={}", GetLastError());
     return 0;
   }


// Make a final validation check on our handles.

   if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
   {
     Logger::warning("ERROR: Unexpected NULL from FindFirstChangeNotification er={}", GetLastError());
     return 0;
   }

// Change notification is set. Now wait on both notification
// handles and refresh accordingly.

   while (true)
   {
   // Wait for notification.

      Logger::warning("Waiting for notification...");

      dwWaitStatus = WaitForMultipleObjects(2, dwChangeHandles, FALSE, INFINITE);

      switch (dwWaitStatus)
      {
         case WAIT_OBJECT_0:

         // A file was created, renamed, or deleted in the directory.
         // Refresh this directory and restart the notification.

             if ( FindNextChangeNotification(dwChangeHandles[0]) == FALSE )
             {
               Logger::warning("ERROR: FindNextChangeNotification function failed er={}", GetLastError());
               return 0;
             }
             return 2;
         break;

         case WAIT_OBJECT_0 + 1:

         // A directory was created, renamed, or deleted.
         // Refresh the tree and restart the notification.

             if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE )
             {
               Logger::warning("ERROR: FindNextChangeNotification function failed er={}", GetLastError());
               return 0;
             }
             return 3;
         break;

         case WAIT_TIMEOUT:

         // A timeout occurred, this would happen if some value other
         // than INFINITE is used in the Wait call and no changes occur.
         // In a single-threaded environment you might not want an
         // INFINITE wait.

            Logger::warning("No changes in the timeout period.");
            break;

         default:
            Logger::warning("ERROR: Unhandled dwWaitStatus.");
            return 0;
            break;
      }
   }

   return 1;
}

}//end namespace internal
#elif defined(GAME_PLATFORM_LINUX) || defined(GAME_PLATFORM_ANDROID)
#include <sys/inotify.h>
#include <unistd.h>
namespace  internal {

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
int watchDirectory(const char* lpDir)
{
  int length, i = 0;
  int fd;
  int wd;
  char buffer[BUF_LEN];

  fd = inotify_init();

  if ( fd < 0 )
  {
    Logger::warning( "WARNING !!! Cant init inotify for {}", lpDir );
    return 0;
  }

  wd = inotify_add_watch( fd, lpDir, IN_MODIFY );
  length = read( fd, buffer, BUF_LEN );

  if ( length < 0 )
  {
    Logger::warning( "WARNING !!! Cant read inotify event {}", lpDir );
    return 0;
  }

  int result = 1;
  while ( i < length )
  {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len )
    {
      if ( event->mask & IN_MODIFY )
      {
        if ( event->mask & IN_ISDIR )
        {
          Logger::warning( "WARNING !!! The directory {} was modified.", event->name );
        }
        else
        {
          Logger::warning( "WARNING !!! The file {} was modified.", event->name );
          result = 2;
          break;
        }
      }
    }
    i += EVENT_SIZE + event->len;
  }

  inotify_rm_watch( fd, wd );
  close( fd );

  return result;
}
}//end namespace internal
#elif defined(GAME_PLATFORM_MACOSX)
namespace internal {

int watchDirectory(const char* lpDir)
{
  return 1;
}
}
#endif

class FileChangeObserver::Impl
{
public:
  std::map<std::string,int> files;
  vfs::Directory dir;
  threading::SafeThreadPtr thread;
  Signal1<vfs::Path> onFileChangeSignalA;

  void createSnapshot();
  void checkSnapshot();
};

FileChangeObserver::FileChangeObserver()
  : _d(new Impl)
{

}

FileChangeObserver::~FileChangeObserver()
{
  if( _d->thread.isValid() )
    _d->thread->stop();
}

void FileChangeObserver::watch(const std::string& dir)
{
  watch( Directory(dir) );
}

void FileChangeObserver::watch(Directory dir)
{
  _d->dir = dir;
  _d->createSnapshot();

  _d->thread = threading::SafeThread::create( threading::SafeThread::WorkFunction( this, &FileChangeObserver::run ) );
  _d->thread->setDelay( 500 );
}

void FileChangeObserver::run(bool& continues)
{
  int result = internal::watchDirectory( _d->dir.toCString() );
  switch( result )
  {
  case 0:
    continues = false;
  break;

  case 3:
  case 2:
    _d->checkSnapshot();
    continues = true;
  break;

  default : continues = true; break;
  }
}

Signal1<Path>&FileChangeObserver::onFileChange() { return _d->onFileChangeSignalA; }

void FileChangeObserver::Impl::checkSnapshot()
{
  vfs::Entries::Items items = dir.entries().items();
  for( const auto& item : items )
  {
    auto it = files.find(item.fullpath.toString());
    if( it != files.end() )
    {
      unsigned int newTime = item.fullpath.info().modified().hashtime();
      unsigned int oldTime = files[item.fullpath.toString()];
      if( newTime != oldTime )
      {
        files[item.fullpath.toString()] = newTime;
        emit onFileChangeSignalA(item.fullpath);
      }
    }
  }
}

void FileChangeObserver::Impl::createSnapshot()
{
  files.clear();
  vfs::Entries::Items items = dir.entries().items();
  for( const auto& item : items )
    files[ item.fullpath.toString() ] = item.fullpath.info().modified().hashtime();
}


}//end namespace vfs
