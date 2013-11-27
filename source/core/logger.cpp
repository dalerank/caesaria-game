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
#include <limits>
#include <climits>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <map>

class FileLogWriter : public LogWriter
{
private:
	FILE* _logFile;

	Mutex _mutex;

public:
	FileLogWriter(const std::string& path) :
		_logFile(fopen(path.c_str(), "w"))
	{
		DateTime t = DateTime::getCurrenTime();

		fputs("Caesaria logfile created: ", _logFile);
		fputs(StringHelper::format( 0xff, "%02:%02d:%02d",
																t.getHour(), t.getMinutes(), t.getSeconds()).c_str(),
					_logFile);
		fputs("\n", _logFile);
	}

	~FileLogWriter()
	{
		DateTime t = DateTime::getCurrenTime();

		fputs("Caesaria logfile closed: ", _logFile);
		fputs( StringHelper::format( 0xff, "%02:%02d:%02d",
																t.getHour(), t.getMinutes(), t.getSeconds()).c_str(),
					 _logFile);
		fputs("\n", _logFile);

		fflush(_logFile);
	}

	void write( std::string str )
	{
		// Don't write progress stuff into the logfile
		// Make sure only one thread is writing to the file at a time
		MutexLocker locker(&_mutex);

		fputs(str.c_str(), _logFile);
		fflush(_logFile);
	}
};

class ConsoleLogWriter : public LogWriter
{
public:
	void write( std::string str)
	{
		std::cout << str << std::endl;
	}

	void update( std::string str )
	{
		std::cout << "\r";
		std::cout << str ;
		std::cout << "\r"  << std::flush;
	}
};

class Logger::Impl
{
public:
  typedef std::map<std::string,LogWriterPtr> Writers;

  Writers writers;

  void write( const std::string& message, bool newLine=true )
  {
    for( Writers::iterator i=writers.begin(); i != writers.end(); i++  )
    {
      if( i->second.isValid() )
      {
        if( newLine ) i->second->write( message );
        else i->second->update( message );
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

  getInstance()._d->write( ret );
}

void Logger::warning(const std::string& text)
{
  getInstance()._d->write( text );
}

void Logger::update(const std::string& text)
{  
  getInstance()._d->write( text, false );
}

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

Logger& Logger::getInstance()
{
  static Logger inst;
  return inst;
}

Logger::~Logger()
{

}

Logger::Logger() : _d( new Impl )
{
}

void Logger::registerWriter(std::string name, LogWriterPtr writer)
{
  getInstance()._d->writers[ name ] = writer;
}

