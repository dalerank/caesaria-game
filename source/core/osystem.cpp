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

#ifdef CAESARIA_PLATFORM_LINUX
#include <cstdlib>
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
    ::system(command.c_str());
  }

  // fail-safe method here, using stdio perhaps, depends on your application
#elif defined(CAESARIA_PLATFORM_WIN)
  MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
#endif
}

void OSystem::openUrl(const std::string& url)
{
#ifdef CAESARIA_PLATFORM_LINUX
  std::string command = "xdg-open " + url;
  ::system( command.c_str() );
#elif defined(CAESARIA_PLATFORM_WIN)
  ShellExecuteA(0, 0, url.c_str(), 0, 0 , SW_SHOW );
#endif
}

void OSystem::openDir(const std::string& path)
{
  std::string result;
#ifdef CAESARIA_PLATFORM_LINUX
  result = "nautilus " + path + " &";
  ::system( result.c_str() );
#elif defined(CAESARIA_PLATFORM_WINDOWS)
  ShellExecute(GetDesktopWindow(), "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(CAESARIA_PLATFORM_MACOSX)
  result = "open " + path + " &";
  ::system( result.c_str() );
#endif
}
