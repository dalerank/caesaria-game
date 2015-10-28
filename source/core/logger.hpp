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
  virtual void write( const std::string&, bool newLine ) = 0;
  virtual bool isActive() const = 0;
};

typedef SmartPtr<LogWriter> LogWriterPtr;

class SimpleLogger
{
  enum class Severity : unsigned short {
    DBG,
    INFO,
    WARN,
    ERR,
    FATAL
  };
public:
  SimpleLogger(const std::string& category);

  bool isDebugEnabled() const;

#define DECL_LOGFUNC(name,severity) template<typename... Args> \
  void name(const std::string& f, const Args & ... args) { llog( severity, fmt::format( f,args... )); } \
  void name(const std::string& text) { llog( severity, text ); }

  DECL_LOGFUNC(warn,Severity::WARN)
  DECL_LOGFUNC(info,Severity::INFO)
  DECL_LOGFUNC(debug,Severity::DBG)
  DECL_LOGFUNC(error,Severity::ERR)
  DECL_LOGFUNC(fatal,Severity::FATAL)

#undef DECL_LOGFUNC

  template <typename... Args>
  void log(Severity severity, const std::string& format, const Args & ... args)
  {
     llog( severity, fmt::format(format, args...) );
  }

private:
  SimpleLogger() {}
  void write(const std::string &message, bool newline = true);

  std::string _category;

  void llog(Severity, const std::string &text);
  const std::string toS(Severity severity);
};

class Logger : public StaticSingleton<Logger>
{
  SET_STATICSINGLETON_FRIEND_FOR(Logger)
public:
  typedef enum { consolelog=0, filelog, count } Type;
  static void warning( const std::string& text );
  static void warningIf( bool warn, const std::string& text );
  static void update( const std::string& text, bool newline=false );

  static void addFilter(const std::string& text);
  static bool hasFilter(const std::string& text);
  static bool removeFilter(const std::string& text);

  static void registerWriter(Type type , const std::string &param);
  static void registerWriter(const std::string& name, LogWriterPtr writer );

  template <typename... Args>
  static void warning(const std::string& f, const Args & ... args)
  {
     _print( fmt::format(f, args...) );
  }

  static Logger& instance();
  ~Logger();
private:
  static void _print( const std::string& text );
  Logger();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_LOGGER_H_INCLUDED__
