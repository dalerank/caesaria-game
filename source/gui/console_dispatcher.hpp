#ifndef _HEADER_COMMAND_DISPATCHER_
#define _HEADER_COMMAND_DISPATCHER_

#include "core/stringarray.hpp"
#include <map>

//! the dispatcher interface

class ConsoleCommand;
class MessageSink;

class CommandDispatcher
{
public:
	//! destructor
	virtual ~CommandDispatcher();
	//! register a command (the cmd->getName() should not already be registered else an exception is thrown)
	void RegisterCommand(ConsoleCommand* cmd);
	//! check if a command is already registered
	bool hasCommand(const std::string& cmdName);
	//! dispatch a command with the name and the supplied args
	void Dispatch(const std::string& cmdName, const StringArray& args, MessageSink* pOutput);
	//! deregister (remove) a command
	void DeregisterCommand(const std::string& cmdName);
	//! get the list of registered commands
	unsigned int  GetRegisteredCommands( StringArray& cmdNames);
	//! deregister all commands
	void deregisterAllCommands();

	//! print a list of commands
	void printCommandList( MessageSink* pOutput, bool bUsage);
	//! print the command usage
	void printCommandUsage(const std::string& cmdName, MessageSink* pOutput);
	//! print detailed description
	void printCommandDesc(const std::string& cmdName, MessageSink* pOutput);
protected:
	//! constructor
	CommandDispatcher();
private:
	//! the map of command name to command pointer
	typedef std::map< std::string, ConsoleCommand* > CommandsMap;
	typedef CommandsMap::iterator CommandIterator;

	CommandsMap commands_;
};

#endif
