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

#include "logger.hpp"
#include "crashHandler.hpp"
#include <stdio.h>
#include <signal.h>
#include "stacktrace.hpp"

CrashHandler::CrashHandler()
{
  signal( SIGABRT, CrashHandler_handleCrash);
  signal( SIGSEGV, CrashHandler_handleCrash);
  signal( SIGILL , CrashHandler_handleCrash);
  signal( SIGFPE , CrashHandler_handleCrash);
}

void CrashHandler_handleCrash(int signum)
{

  switch(signum)
  {
    case SIGABRT: Logger::warning("SIGABRT: abort() called somewhere in the program."); break;
    case SIGSEGV: Logger::warning("SIGSEGV: Illegal memory access."); break;
    case SIGILL: Logger::warning("SIGILL: Executing a malformed instruction."); break;
    case SIGFPE: Logger::warning("SIGFPE: Illegal floating point instruction (possibly division by zero)."); break;
  }

  Stacktrace::print();

/*

#if (WIN32)
  CaptureStackBackTrace(0, )
#else
  Logger::warning("Sorry, call stack retrieving not implement for this platform yet.");
#endif*/
  exit(signum);
}
