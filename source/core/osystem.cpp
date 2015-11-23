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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "osystem.hpp"
#include "platform.hpp"
#include "logger.hpp"
#include "core/stringarray.hpp"
#include "vfs/directory.hpp"
#include "vfs/path.hpp"
#include <fstream>
#include <ctime>

#if defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
  #include <limits.h>
  #include <unistd.h>
  #include <sys/stat.h>
#endif

#ifdef CAESARIA_PLATFORM_LINUX
#include <cstdlib>
#include <string.h>
const char* getDialogCommand()
{
  if (::system(NULL))
  {
    if(::system("which gdialog") == 0)
      return "gdialog";

    else if (::system("which kdialog") == 0)
      return "kdialog";
  }
  return NULL;
}
#elif defined(CAESARIA_PLATFORM_MACOSX)
  #include <cstdlib>
#elif defined(CAESARIA_PLATFORM_WIN)
  #include <windows.h>
#endif

void OSystem::error(const std::string& title, const std::string& text)
{
#if defined(CAESARIA_PLATFORM_LINUX)
  const char * dialogCommand = getDialogCommand();
  if (dialogCommand)
  {
    std::string command = dialogCommand;
    command += " --title \"" + title + "\" --msgbox \"" + text + "\"";
    int syserror = ::system(command.c_str());
    if( syserror )
    {
      Logger::warning( "WARNING: Cant execute command " + command );
    }
  }

  // fail-safe method here, using stdio perhaps, depends on your application
#elif defined(CAESARIA_PLATFORM_WIN)
  MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
#endif
}

#ifdef CAESARIA_PLATFORM_LINUX
/**
 * Get the parent PID from a PID
 * @param pid pid
 * @param ppid parent process id
 *
 * Note: init is 1 and it has a parent id of 0.
 */
int getParentPid(const pid_t pid) {
  char buffer[BUFSIZ];
  sprintf(buffer, "/proc/%d/stat", pid);
  int result;
  FILE* fp = fopen(buffer, "r");
  if (fp) {
    size_t size = fread(buffer, sizeof (char), sizeof (buffer), fp);
    if (size > 0) {
      // See: http://man7.org/linux/man-pages/man5/proc.5.html section /proc/[pid]/stat
      strtok(buffer, " "); // (1) pid  %d
      strtok(NULL, " "); // (2) comm  %s
      strtok(NULL, " "); // (3) state  %c
      char * s_ppid = strtok(NULL, " "); // (4) ppid  %d
      result = atoi(s_ppid);
    }
    fclose(fp);
  }

  return result;
}

/**
 * Get a process name from its PID.
 * @param pid PID of the process
 * @param name Name of the process
 *
 * Source: http://stackoverflow.com/questions/15545341/process-name-from-its-pid-in-linux
 */
std::string getProcessName(const pid_t pid) {
  char procfile[BUFSIZ];
  sprintf(procfile, "/proc/%d/cmdline", pid);
  char name[0xff];
  FILE* f = fopen(procfile, "r");
  if (f) {
    size_t size;
    size = fread(name, sizeof (char), sizeof (procfile), f);
    if (size > 0) {
      if ('\n' == name[size - 1])
        name[size - 1] = '\0';
    }
    fclose(f);
  }

  return name;
}
#endif

void OSystem::getProcessTree(int pid, StringArray& out)
{
#ifdef CAESARIA_PLATFORM_LINUX
  while (pid != 0)
  {
    std::string name = getProcessName(pid);
    Logger::warning( "{} - {}", pid, name.c_str() );
    pid = getParentPid( pid );
    vfs::Path pname( name );
    out.push_back( pname.baseName().toString() );
  }
#endif
}

void OSystem::openUrl(const std::string& url, const std::string& prefix)
{
#ifdef CAESARIA_PLATFORM_LINUX
  std::string command = prefix + "xdg-open '" + url + "'";
  Logger::warning( command );
  ::system( command.c_str() );
#elif defined(CAESARIA_PLATFORM_WIN)
  ShellExecuteA(0, "Open", url.c_str(), 0, 0 , SW_SHOW );
#elif defined(CAESARIA_PLATFORM_MACOSX)
  std::string result = "open \"" + url + "\" &";
  ::system( result.c_str() );
#endif
}

void OSystem::openDir(const std::string& path, const std::string& prefix)
{
  std::string result;
#ifdef CAESARIA_PLATFORM_LINUX
  result = prefix + "nautilus '" + path + "' &";
  ::system( result.c_str() );
#elif defined(CAESARIA_PLATFORM_WIN)
  ShellExecute(GetDesktopWindow(), "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(CAESARIA_PLATFORM_MACOSX)
  result = "open \"" + path + "\" &";
  ::system( result.c_str() );
#endif
}

int OSystem::gmtOffsetMs()
{
#if defined(CAESARIA_PLATFORM_LINUX) || defined(CAESARIA_PLATFORM_MACOSX)
  std::time_t current_time;
  std::time(&current_time);
  struct std::tm *timeinfo = std::localtime(&current_time);
  return timeinfo->tm_gmtoff;
#elif defined(CAESARIA_PLATFORM_WIN)
  time_t now = time(NULL);
  struct tm lcl = *localtime(&now);
  struct tm gmt = *gmtime(&now);
  return (lcl.tm_hour - gmt.tm_hour);
#endif
}

bool OSystem::is(OSystem::Type type)
{
  switch( type )
  {
  case windows:
#ifdef CAESARIA_PLATFORM_WIN
    return true;
#endif
  break;
  case win32:
#ifdef CAESARIA_PLATFORM_WIN32
    return true;
#endif
  break;
  case win64:
#ifdef CAESARIA_PLATFORM_WIN64
    return true;
#endif
  break;
  case linux:
#ifdef CAESARIA_PLATFORM_LINUX
    return true;
#endif
  break;
  case unix:
#ifdef CAESARIA_PLATFORM_UNIX
    return true;
#endif
  break;
  case android:
#ifdef CAESARIA_PLATFORM_ANDROID
    return true;
#endif
  break;
  case macos:
#ifdef CAESARIA_PLATFORM_MACOSX
    return true;
#endif
  break;
  case bsd:
#ifdef CAESARIA_PLATFORM_XBSD
    return true;
#endif
  break;
  case haiku:
#ifdef CAESARIA_PLATFORM_HAIKU
    return true;
#endif
  break;
  case beos:
#ifdef CAESARIA_PLATFORM_BEOS
    return true;
#endif
  break;
  default: break;
  }

  return false;
}

static const std::string tempFilePrefix = "__";
static std::string _prepareUpdateBatchFile( const std::string& executableFp, const std::string& dir, const StringArray& cmds)
{
  // Create a new batch file in the target location
  //vfs::Directory targetdir = getTargetDir();
  vfs::Path executable( executableFp );
  vfs::Directory targetdir( dir );

  vfs::Path temporaryUpdater = tempFilePrefix + executable.baseName().toString();

  std::string restartBatchFile = OSystem::isWindows() ? "update_updater.cmd" : "update_updater.sh";

  vfs::Path updateBatchFile =  targetdir.getFilePath( restartBatchFile );

  Logger::warning( "Preparing CaesarIA update batch file in " + updateBatchFile.toString() );

  std::ofstream batch(updateBatchFile.toCString());

  vfs::Path tempUpdater = temporaryUpdater.baseName();
  vfs::Path updater = executable.baseName();

  // Append the current set of command line arguments to the new instance
  std::string arguments;

  for(const auto& optionItem : cmds )
  {
    arguments += " " + optionItem;
  }

#ifdef CAESARIA_PLATFORM_WIN
  batch << "@ping 127.0.0.1 -n 2 -w 1000 > nul" << std::endl; // # hack equivalent to Wait 2
  batch << "@copy " << tempUpdater.toString() << " " << updater.toString() << " >nul" << std::endl;
  batch << "@del " << tempUpdater.toString() << std::endl;
  batch << "@echo CaesarIA Updater executable has been updated." << std::endl;

  batch << "@echo Re-launching CaesarIA Updater executable." << std::endl << std::endl;

  batch << "@start " << updater.toString() << " " << arguments;
#else // POSIX
  // grayman - accomodate spaces in pathnames
  tempUpdater = targetdir.getFilePath( tempUpdater );
  updater = targetdir.getFilePath( updater );

  batch << "#!/bin/bash" << std::endl;
  batch << "echo \"Upgrading CaesarIA Updater executable...\"" << std::endl;
  batch << "cd \"" << targetdir.toString() << "\"" << std::endl;
  batch << "sleep 2s" << std::endl;
  batch << "mv -f \"" << tempUpdater.toString() << "\" \"" << updater.toString() << "\"" << std::endl;
  batch << "chmod +x \"" << updater.toString() << "\"" << std::endl;
  batch << "echo \"CaesarIA Updater executable has been updated.\"" << std::endl;
  batch << "echo \"Re-launching CaesarIA Updater executable.\"" << std::endl;

  batch << "\"" << updater.toString() << "\" " << arguments;
#endif

  batch.close();

  // Mark the shell script as executable in *nix
  if( OSystem::isUnix() )
    OSystem::markFileAsExecutable(updateBatchFile.toString());

  return updateBatchFile.toString();
}

void OSystem::markFileAsExecutable( const std::string& filename )
{
  vfs::Path path( filename );
#if defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
  Logger::warning( "Marking file as executable: " + path.toString() );

  struct stat mask;
  stat(path.toCString(), &mask);

  mask.st_mode |= S_IXUSR|S_IXGRP|S_IXOTH;

  if( chmod(path.toCString(), mask.st_mode) == -1)
  {
    Logger::warning( "Could not mark file as executable: " + path.toString() );
  }
#endif
}

void OSystem::restartProcess( const std::string& filename, const std::string& dir, const StringArray& cmds)
{
  Logger::warning( "Preparing restart...");
  std::string _updateBatchFile = _prepareUpdateBatchFile( filename, dir, cmds );  

#ifdef CAESARIA_PLATFORM_WIN
  if (!_updateBatchFile.empty())
  {
    Logger::warning( "Update batch file pending, launching process.");

    // Spawn a new process

    // Create a caesaria updater process, setting the working directory to the target directory
    STARTUPINFOA siStartupInfo;
    PROCESS_INFORMATION piProcessInfo;

    memset(&siStartupInfo, 0, sizeof(siStartupInfo));
    memset(&piProcessInfo, 0, sizeof(piProcessInfo));

    siStartupInfo.cb = sizeof(siStartupInfo);

    vfs::Path batchFilePath( _updateBatchFile );
    vfs::Directory parentPath = batchFilePath.directory();

    Logger::warning( "Starting batch file " + batchFilePath.toString() + " in " + parentPath.toString() );

    BOOL success = CreateProcessA( NULL, (LPSTR) batchFilePath.toString().c_str(), NULL, NULL,  false, 0, NULL,
                                   parentPath.toString().c_str(), &siStartupInfo, &piProcessInfo);

    if (!success)
    {
      LPVOID lpMsgBuf;

      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL);

      throw std::ios_base::failure( "Could not start new process: " + std::string((LPCSTR)lpMsgBuf));

      LocalFree(lpMsgBuf);
    }
    else
    {
      Logger::warning( "Process started");
      exit(0);
    }
  }
#else
  if (!_updateBatchFile.empty() )
  {
    Logger::warning( "Relaunching CaesarIA updater via shell script " + _updateBatchFile );

    // Perform the system command in a fork
    //int r = fork();
    //if( r >= 0 )
    {
      // Don't wait for the subprocess to finish
      system((_updateBatchFile + " &").c_str());
      exit(EXIT_SUCCESS);
      return;
    }

    Logger::warning( "Process spawned.");

    // Done here too
    return;
  }
#endif
}

bool OSystem::isAndroid() { return is( android ); }
bool OSystem::isLinux() { return is( linux ); }
bool OSystem::isUnix() { return is( unix ); }
bool OSystem::isMac() { return is( macos ); }
bool OSystem::isWindows() { return is( windows ); }
