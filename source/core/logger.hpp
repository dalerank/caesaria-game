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

#ifndef __CAESARIA_LOGGER_H_INCLUDED__
#define __CAESARIA_LOGGER_H_INCLUDED__

#include <string>
#include "referencecounted.hpp"
#include "smartptr.hpp"
#include "scopedptr.hpp"

class LogWriter : public ReferenceCounted
{
public:
  virtual void write( const std::string&, bool newLine ) = 0;
  virtual bool isActive() const = 0;
};

typedef SmartPtr<LogWriter> LogWriterPtr;

class SimpleLogger {
public:
  SimpleLogger(std::string category);

  bool isDebugEnabled() const;

  void debug(const char *fmt, ...);

  void debug(const std::string &text);

  void info(const char *fmt, ...);

  void info(const std::string &text);

  void warn(const char *fmt, ...);

  void warn(const std::string &text);

  void error(const char *fmt, ...);

  void error(const std::string &text);

  void fatal(const char *fmt, ...);

  void fatal(const std::string &text);

private:
  void write(const std::string &message, bool newline = true);

  std::string category;

  enum class Severity : unsigned short {
    DBG,
    INFO,
    WARN,
    ERROR,
    FATAL
  };

  void log(Severity, const char *fmt, ...);
  void llog(Severity, const std::string &text);
  void vlog(Severity severity, const char *fmt, va_list args);

  const std::string toS(Severity severity);
};

class Logger
{
public:
  typedef enum { consolelog=0, filelog, count } Type;
  static void warning( const char* fmt, ...);
  static void warning( const std::string& text );
  static void warningIf( bool warn, const std::string& text );
  static void update( const std::string& text, bool newline=false );

  static void addFilter(const std::string& text);
  static bool hasFilter(const std::string& text);
  static bool removeFilter(const std::string& text);

  static void registerWriter(Type type , const std::string &param);
  static void registerWriter( std::string name, LogWriterPtr writer );

  static Logger& instance();
  ~Logger();
private:
  Logger();

  class Impl;
  ScopedPtr< Impl > _d;
  friend class SimpleLogger;
};

#endif //__CAESARIA_LOGGER_H_INCLUDED__
