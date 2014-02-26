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
		AppendMessage( RB( "<info> " ) + message );
	}

	//! log message to the sink at warning log level
	template< class RB > void LogWarning(const RB& message)
	{
		AppendMessage( RB( "<warning> ") + message);
	}
	//! log message to the sink at error log level
	template< class RB > void LogError(const RB& message)
	{
		AppendMessage( RB( "<error> ") + message );
	}

	//! add a UTF-16 message to the sink
	virtual void AppendMessage( const std::string& message )=0;
	//virtual void AppendMessage( const stringw& message ) {};
	//! clear all the messages in the sink
	virtual void ClearMessages() = 0;
	virtual void ToggleVisible() = 0;
};

#endif
