#include "console_commands.hpp"
#include "message_sink.hpp"
#include "console_command.hpp"
#include "console_dispatcher.hpp"
#include "core/utils.hpp"

IC_Command_ECHO::IC_Command_ECHO() : ConsoleCommand("echo")
{
  SetUsage("echo <string>");
  AddDescLine( "print string");
}

IC_Command_ECHO::~IC_Command_ECHO() {}

bool IC_Command_ECHO::invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	if(args.size() > 0)
	{
		std::string wstr = "";
		for(unsigned int i = 0; i < args.size(); i++)
		{
			wstr.append( args[i] );
			wstr.append( " " );
		}
		pOutput->AppendMessage( wstr );
	}
	return true;
}

IC_Command_HELP::IC_Command_HELP() : ConsoleCommand("help")
{
  SetUsage("help <command>");
  AddDescLine("print help for command");
}

IC_Command_HELP::~IC_Command_HELP()
{
}

bool IC_Command_HELP::invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	if(args.size() == 0)
	{
		pDispatcher->printCommandList( pOutput, true );
	}
	else
	{
		std::string  wstr = args[0];
		for(unsigned int i = 1; i < args.size(); i++)
		{
			wstr.append( " " );
			wstr.append( args[i] );
		}

		if(pDispatcher->hasCommand(wstr))
		{
			pDispatcher->printCommandDesc(wstr,pOutput);
		}
		else
		{
			std::string msg = "Command not fonud " + wstr;
			pOutput->AppendMessage( msg );
		}
	}
	return true;
}

IC_Command_LIST::IC_Command_LIST(): ConsoleCommand("list")
{
  SetUsage( "list <detailed>");
  AddDescLine( "prind avalaible commands");
}

IC_Command_LIST::~IC_Command_LIST() {}

bool IC_Command_LIST::invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	pDispatcher->printCommandList(pOutput, args.size() > 0);
	return true;
}

IC_Command_INFO::IC_Command_INFO() : ConsoleCommand("info")
{
	SetUsage( "info");
	AddDescLine( "print system info");
	AddDescLine( "-system hardware info");
	AddDescLine( "-objlist objects info");
}

IC_Command_INFO::~IC_Command_INFO()
{
}

bool IC_Command_INFO::invoke(const StringArray& args,
									CommandDispatcher* pDispatcher, 
									MessageSink* pOutput)
{
	if( args.size() == 0 )
	{
		pOutput->AppendMessage( "no param" );
		return true;
	}

	if( args[ 0 ] == "-system" )
	{

	}

	if( args[ 0 ] == "-objlist" )
	{
		StringArray strings;
		//GGetListSystemObject( strings );

		for( size_t cnt=0; cnt < strings.size(); cnt++ )
			pOutput->AppendMessage( strings[ cnt ] );


		pOutput->AppendMessage( std::string( "Size of objlist = " ) + utils::format( 0xff, "%d", strings.size() ) );

		return true;
	}

	return false;
}

IC_Command_CLS::IC_Command_CLS() : ConsoleCommand("cls")
{
  SetUsage("cls");
  AddDescLine( "Clear screen");
}

IC_Command_CLS::~IC_Command_CLS() {}

bool IC_Command_CLS::invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	pOutput->ClearMessages();
	return true;
}

IC_Command_SCRIPT::IC_Command_SCRIPT(): ConsoleCommand("script")
{

}

IC_Command_SCRIPT::~IC_Command_SCRIPT() {}

bool IC_Command_SCRIPT::invoke( const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput )
{
	return true;
}
