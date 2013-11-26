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

#ifndef __CAESARIA_LOGGER_H_INCLUDED__
#define __CAESARIA_LOGGER_H_INCLUDED__

#include <string>
#include "referencecounted.hpp"
#include "smartptr.hpp"
#include "scopedptr.hpp"

class LogWriter : public ReferenceCounted
{
public:
  virtual void write( std::string ) {}
  virtual void update( std::string ) {}
};

typedef SmartPtr<LogWriter> LogWriterPtr;

class Logger
{
public:
  typedef enum { consolelog=0, filelog, count } Type;
  static void warning( const char* fmt, ...);
  static void warning( const std::string& text );
  static void update( const std::string& text );

  static void registerWriter( Type type );
  static void registerWriter( std::string name, LogWriterPtr writer );

  static Logger& getInstance();
  ~Logger();
private:
  Logger();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_LOGGER_H_INCLUDED__
