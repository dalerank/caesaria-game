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
#include "singleton.hpp"
#include "scopedptr.hpp"
#include "format.hpp"

class LogWriter : public ReferenceCounted
{
public:
  enum Severity {
    debug=0,
    info,
    warn,
    error,
    fatal
  };

  static const char* severity(LogWriter::Severity s);
  virtual void write(const std::string&, bool newLine) = 0;
  virtual bool isActive() const = 0;
};

typedef SmartPtr<LogWriter> LogWriterPtr;

class SimpleLogger
{
public:
  SimpleLogger(const std::string& category);

  bool isDebugEnabled() const;

#define DECL_LOGFUNC(name,severity) template<typename... Args> \
  void name(const std::string& f, const Args & ... args) { llog( severity, fmt::format( f,args... )); } \
  void name(const std::string& text) { llog( severity, text ); }

  DECL_LOGFUNC(warn,  LogWriter::warn)
  DECL_LOGFUNC(info,  LogWriter::info)
  DECL_LOGFUNC(debug, LogWriter::debug)
  DECL_LOGFUNC(error, LogWriter::error)
  DECL_LOGFUNC(fatal, LogWriter::fatal)
#undef DECL_LOGFUNC

  template <typename... Args>
  void log(LogWriter::Severity severity, const std::string& format, const Args & ... args)
  {
     llog(severity, fmt::format(format, args...));
  }

private:
  SimpleLogger() {}
  void write(const std::string &message, bool newline = true);

  std::string _category;

  void llog(LogWriter::Severity, const std::string &text);  
};

class Logger : public StaticSingleton<Logger>
{
  SET_STATICSINGLETON_FRIEND_FOR(Logger)
public:
  typedef enum {consolelog=0, filelog, count} Type;

#define DECL_LOGFUNC(severity,name) template<typename... Args> \
  static void name(const std::string& f, const Args & ... args) { _print(severity, fmt::format( f,args... )); } \
  static void name(const std::string& text) { _print( severity, text ); }

  DECL_LOGFUNC(LogWriter::info,  info)
  DECL_LOGFUNC(LogWriter::warn,  warning)
  DECL_LOGFUNC(LogWriter::debug, debug)
  DECL_LOGFUNC(LogWriter::error, error)
  DECL_LOGFUNC(LogWriter::fatal, fatal)
#undef DECL_LOGFUNC

  static void warningIf(bool warn, const std::string& text);
  static void update(const std::string& text, bool newline = false);

  static void addFilter(const std::string& text);
  static void addFilter(LogWriter::Severity s);
  static bool hasFilter(const std::string& text);
  static bool removeFilter(const std::string& text);

  static void registerWriter(Type type, const std::string &param);
  static void registerWriter(const std::string& name, LogWriterPtr writer);

  static Logger& instance();
  ~Logger();
private:
  static void _print(LogWriter::Severity s,const std::string& text);
  Logger();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_LOGGER_H_INCLUDED__
