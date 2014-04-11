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

#include "logger.hpp"
#include "requirements.hpp"
#include "stringhelper.hpp"
#include "mutex.hpp"
#include "time.hpp"
#include "foreach.hpp"

#include <cstdarg>
#include <cfloat>
#include <stdio.h>
#include <climits>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <map>

#ifdef CAESARIA_PLATFORM_ANDROID
#include <android/log.h>
#endif

class FileLogWriter : public LogWriter
{
private:
	FILE* _logFile;

	Mutex _mutex;

public:
	FileLogWriter(const std::string& path)
	{
		DateTime t = DateTime::getCurrenTime();

		_logFile = fopen(path.c_str(), "w");

		if( _logFile )
		{
			fputs("Caesaria logfile created: ", _logFile);
			fputs(StringHelper::format( 0xff, "%02d:%02d:%02d",
																	t.hour(), t.minutes(), t.seconds()).c_str(),
						_logFile);
			fputs("\n", _logFile);
		}
	}

	~FileLogWriter()
	{
		DateTime t = DateTime::getCurrenTime();

		if( _logFile )
		{
			fputs("Caesaria logfile closed: ", _logFile);
			fputs( StringHelper::format( 0xff, "%02d:%02d:%02d",
																	t.hour(), t.minutes(), t.seconds()).c_str(),
						 _logFile);
			fputs("\n", _logFile);

			fflush(_logFile);
		}
	}

	virtual bool isActive() const { return _logFile != 0; }

	virtual void write( std::string str, bool )
	{
		// Don't write progress stuff into the logfile
		// Make sure only one thread is writing to the file at a time
		if( _logFile )
		{
			MutexLocker locker(&_mutex);

			fputs(str.c_str(), _logFile);
			fputs("\n", _logFile);
			fflush(_logFile);
		}
	}
};

class ConsoleLogWriter : public LogWriter
{
public:
	virtual void write( std::string str, bool newline )
	{
#ifdef CAESARIA_PLATFORM_ANDROID
      str.append( newline ? "\n" : "" );
      __android_log_print(ANDROID_LOG_DEBUG, CAESARIA_PLATFORM_NAME, str.c_str() );
#else
    std::cout << str;
    if( newline ) std::cout << std::endl;
    else std::cout << std::flush;
#endif
	}

	virtual bool isActive() const { return true; }
};

class Logger::Impl
{
public:
  typedef std::map<std::string,LogWriterPtr> Writers;

  Writers writers;

  void write( const std::string& message, bool newline=true )
  {
    foreach( i, writers )
    {
      if( i->second.isValid() )
      {
        i->second->write( message, newline );
      }
    }
  }
};

void Logger::warning( const char* fmt, ... )
{
  va_list argument_list;

  va_start(argument_list, fmt);

  std::string ret;
  StringHelper::vformat( ret, 512, fmt, argument_list );

  va_end(argument_list);

  instance()._d->write( ret );
}

void Logger::warning(const std::string& text) {  instance()._d->write( text );}
void Logger::warningIf(bool warn, const std::string& text){  if( warn ) warning( text ); }
void Logger::update(const std::string& text, bool newline){  instance()._d->write( text, newline ); }

void Logger::registerWriter(Logger::Type type)
{
  switch( type )
  {
  case consolelog:
  {
    LogWriterPtr wr( new ConsoleLogWriter() );
    wr->drop();
    registerWriter( "__console", wr );
  }
  break;

  case filelog:
  {
    LogWriterPtr wr( new FileLogWriter( "stdout.txt" ) );
    wr->drop();
    registerWriter( "__log", wr );
  }
  break;

  case count: break;
  }
}

Logger& Logger::instance()
{
  static Logger inst;
  return inst;
}

Logger::~Logger() {}

Logger::Logger() : _d( new Impl )
{
}

void Logger::registerWriter(std::string name, LogWriterPtr writer)
{
  if( writer.isValid() && writer->isActive() )
  {
    instance()._d->writers[ name ] = writer;
  }
}

