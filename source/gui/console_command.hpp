#ifndef _CAESARIA_CONSOLE_COMMAND_HPP_INCLUDE_
#define _CAESARIA_CONSOLE_COMMAND_HPP_INCLUDE_

#include "core/stringarray.hpp"


class MessageSink;
class CommandDispatcher;

//! the command class
class ConsoleCommand
{
public:
	//! the destructor
	virtual ~ConsoleCommand();

	//! invoke the command with supplied args
	virtual bool invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput) = 0;

	//! get command name
	const std::string GetName();
	//! get the usage string for the command
	const std::string GetUsage();
	//! get the description line count
	const unsigned int GetDescLineCount();
	//! get the desc line at
	const std::string GetDescLine(const unsigned int index);

	//! print the usage to the message sink
	void PrintUsage(MessageSink* pOutput);
	//! print the description to the message sink
	void PrintDesc(MessageSink* pOutput);

protected:
	//! constructor
	ConsoleCommand(const std::string& name);
	//! set usage
	virtual void SetUsage(const std::string& usage);
	//! append a line to the desc array
	virtual void AddDescLine(const std::string& line);
	//! set descrition to ""
	void ClearDescLines();
private:
	//! the name of the command
	std::string command_name_;
	//! the usage string for the command
	std::string command_usage_;
	//! the description lines
	StringArray command_declines_;
};

#endif //_CAESARIA_CONSOLE_COMMAND_HPP_INCLUDE_
