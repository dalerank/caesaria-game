#ifndef _HEADER_MESSAGE_SINK_CONSOLE_
#define _HEADER_MESSAGE_SINK_CONSOLE_

#include <string>

//!the message sink interface
class MessageSink
{
public:
	//! constructor
	MessageSink() {}
	//! destructor
	virtual ~MessageSink() {}

	//! log message to the sink at info log level
	template< class RB > void LogInfo(const RB& message)
	{
    appendMessage( RB( "<info> " ) + message );
	}

	//! log message to the sink at warning log level
	template< class RB > void LogWarning(const RB& message)
	{
    appendMessage( RB( "<warning> ") + message);
	}
	//! log message to the sink at error log level
	template< class RB > void LogError(const RB& message)
	{
    appendMessage( RB( "<error> ") + message );
	}

	//! add a UTF-16 message to the sink
  virtual void appendMessage( const std::string& message )=0;
	//virtual void AppendMessage( const stringw& message ) {};
	//! clear all the messages in the sink
  virtual void clearMessages() = 0;
  virtual void toggleVisible() = 0;
};

#endif
