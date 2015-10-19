#include "console_dispatcher.hpp"
#include "console_command.hpp"
#include "message_sink.hpp"
#include "console_utils.hpp"
#include <exception>

//! constructor
CommandDispatcher::CommandDispatcher()
{
}

//! destructor
CommandDispatcher::~CommandDispatcher()
{
	deregisterAllCommands();
}

//! register a command (the cmd->getName() should not already be registered else an exception is thrown)
void CommandDispatcher::RegisterCommand(ConsoleCommand* cmd)
{
	if(cmd)
	{
		std::string name = cmd->GetName();
		
		if( !hasCommand(name) )
		{
			commands_[name] = cmd;
		}
		else
		{
			std::string wstr = std::string( "Command ") + name + " is already registered";
			throw ConsoleError( wstr );
		}
	}
}

//! check if a command is already registered
bool CommandDispatcher::hasCommand(const std::string& cmdName)
{
	return ( commands_.find( cmdName ) != commands_.end() );
}

//! dispatch a command with the name and the supplied args
void CommandDispatcher::Dispatch(const std::string& cmdName, const StringArray& args, MessageSink* pOutput)
{
	CommandsMap::iterator node = commands_.find(cmdName);

	if( node != commands_.end() )
	{
		try
		{
			node->second->invoke( args, this, pOutput);
			pOutput->appendMessage( std::string() );
		}
        catch( ConsoleError& err)
		{
			std::string wstr = "error of type ";
			wstr += err.getType();
			wstr += " in invoking command [";
			wstr += cmdName;
			wstr += "]";
			pOutput->LogError(wstr);
			pOutput->appendMessage( err.getMessage() );
		}
		catch( std::exception& ex )
		{
			std::string wstr = "error in invoking command [";
			wstr += cmdName;
			wstr += "]";
			pOutput->LogError<std::string>(wstr);
			pOutput->appendMessage( ex.what() );
		}
	}
	else
	{
		std::string wstr = "command [";
		wstr += cmdName;
		wstr += "] is not a valid command";
		pOutput->LogError<std::string>(wstr);
		pOutput->appendMessage( "for a list of messages type \"help\" or \"list\"" );
	}
}

//! deregister (remove) a command
void CommandDispatcher::DeregisterCommand(const std::string& cmdName)
{
	CommandsMap::iterator iter = commands_.find(cmdName);

	if( iter != commands_.end() )
	{
		delete iter->second;
		commands_.erase( iter );
	}
}

//! deregister all commands
void CommandDispatcher::deregisterAllCommands()
{
	CommandIterator iter = commands_.begin();
	
	for(; iter != commands_.end(); iter++)
		delete iter->second;

	commands_.clear();
}

//! get the list of registered commands
unsigned int  CommandDispatcher::GetRegisteredCommands( StringArray& cmdNames)
{
	cmdNames.clear();

	CommandIterator iter = commands_.begin();
	
	for(; iter != commands_.end(); iter++ )
		cmdNames.push_back( iter->first );

	return cmdNames.size();
}

//! print a list of commands
void CommandDispatcher::printCommandList( MessageSink* pOutput, bool bUsage )
{
	StringArray cmdNames;

	GetRegisteredCommands(cmdNames);

	if(bUsage)
	{
		for(unsigned int i = 0; i < cmdNames.size(); i++)
		{
			printCommandUsage(cmdNames[i],pOutput);
			pOutput->appendMessage( std::string() );
		}
	}
	else
	{
		for(unsigned int i = 0; i < cmdNames.size(); i++)
			pOutput->appendMessage( cmdNames[ i ] );
	}
}

//! print the command usage
void CommandDispatcher::printCommandUsage(const std::string& cmdName, MessageSink* pOutput)
{
	ConsoleCommand* cmd = commands_[cmdName];
	
	if(cmd)
	{
		cmd->PrintUsage(pOutput);
	}
}

//! print detailed description
void CommandDispatcher::printCommandDesc(const std::string& cmdName, MessageSink* pOutput)
{
	ConsoleCommand* cmd = commands_[cmdName];
	if(cmd)
	{
		cmd->PrintDesc(pOutput);
	}
}
