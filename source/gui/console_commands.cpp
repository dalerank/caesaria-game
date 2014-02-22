#include "console_commands.hpp"

/*
#include "NrpConsoleCmds.h"
#include "IScriptEngine.h"

NERPA_MODULE_BEGIN(core)

IC_Command_ECHO::IC_Command_ECHO() : ConsoleCommand(L"echo")
{
	SetUsage(L"echo <string>");
        AddDescLine(L"print string");
}

IC_Command_ECHO::~IC_Command_ECHO() {}

bool IC_Command_ECHO::invoke(const Array<String>& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	if(args.size() > 0)
	{
		String wstr = L"";
		for(u32 i = 0; i < args.size(); i++)
		{
			wstr.append( args[i] );
			wstr.append( L" " );
		}
		pOutput->AppendMessage( wstr );
	}
	return true;
}

IC_Command_HELP::IC_Command_HELP() : ConsoleCommand(L"help")
{
        SetUsage(L"help <command>");
        AddDescLine(L"print help for command");
}

IC_Command_HELP::~IC_Command_HELP()
{
}

bool IC_Command_HELP::invoke(const Array<String>& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	if(args.size() == 0)
	{
		pDispatcher->printCommandList( pOutput, true );
	}
	else
	{
		String  wstr = args[0];
		for(u32 i = 1; i < args.size(); i++)
		{
			wstr.append( L" " );
			wstr.append( args[i] );
		}

		if(pDispatcher->hasCommand(wstr))
		{
			pDispatcher->printCommandDesc(wstr,pOutput);
		}
		else
		{
                        String msg = String( L"Command not fonud " ) + String( wstr );
			pOutput->AppendMessage( msg );
		}
	}
	return true;
}

IC_Command_LIST::IC_Command_LIST(): ConsoleCommand(L"list")
{
	SetUsage(L"list <detailed>");
        AddDescLine(L"prind avalaible commands");
}

IC_Command_LIST::~IC_Command_LIST() {}

bool IC_Command_LIST::invoke(const Array<String>& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	pDispatcher->printCommandList(pOutput, args.size() > 0);
	return true;
}

IC_Command_INFO::IC_Command_INFO(irr::IrrlichtDevice *pDevice) 
					   : ConsoleCommand(L"info"),device(pDevice)
{
	SetUsage(L"info");
        AddDescLine(L"print system info");
        AddDescLine(L"-system hardware info");
        AddDescLine(L"-objlist objects info");
}

IC_Command_INFO::~IC_Command_INFO()
{
	device = NULL;
}

bool IC_Command_INFO::invoke(const Array<String>& args, 
									CommandDispatcher* pDispatcher, 
									MessageSink* pOutput)
{
	if( args.size() == 0 )
	{
                pOutput->AppendMessage( L"no param" );
		return true;
	}

	if( args[ 0 ] == L"-system" && device)
	{

	}

	if( args[ 0 ] == L"-objlist" )
	{
		StringArray strings;
		//GGetListSystemObject( strings );

		for( size_t cnt=0; cnt < strings.size(); cnt++ )
			pOutput->AppendMessage( strings[ cnt ] );


        pOutput->AppendMessage( String( L"Size of objlist = " ) + String::fromInt( strings.size() ) );

		return true;
	}

	return false;
}

IC_Command_CLS::IC_Command_CLS() : ConsoleCommand(L"cls")
{
	SetUsage(L"cls");
        AddDescLine(L"Clear screen");
}

IC_Command_CLS::~IC_Command_CLS() {}

bool IC_Command_CLS::invoke(const Array< String >& args, CommandDispatcher* pDispatcher, MessageSink* pOutput)
{
	pOutput->ClearMessages();
	return true;
}

IC_Command_SCRIPT::IC_Command_SCRIPT(): ConsoleCommand(L"script")
{

}

IC_Command_SCRIPT::~IC_Command_SCRIPT() {}

bool IC_Command_SCRIPT::invoke( const Array< String >& args, CommandDispatcher* pDispatcher, MessageSink* pOutput )
{
	return true;
}

NERPA_MODULE_END(core)
*/
