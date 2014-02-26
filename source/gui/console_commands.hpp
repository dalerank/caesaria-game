#ifndef _Console_Default_Cmds_h_
#define _Console_Default_Cmds_h_

#include "console_utils.hpp"
#include "console.hpp"
#include "console_command.hpp"

//! Отображает введенный текст
class IC_Command_ECHO : public ConsoleCommand
{
public:
	IC_Command_ECHO();
	virtual ~IC_Command_ECHO();
	bool invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput);
};

//! Отображает помощь по указанной команде
class IC_Command_HELP : public ConsoleCommand
{
public:
	IC_Command_HELP();
	virtual ~IC_Command_HELP();
	bool invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput);
};

class IC_Command_LIST : public ConsoleCommand
{
public:
	IC_Command_LIST();
	virtual ~IC_Command_LIST();
	bool invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput);
};

class IC_Command_INFO : public ConsoleCommand
{
public:
	IC_Command_INFO();
	virtual ~IC_Command_INFO();
	bool invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput);
};

class IC_Command_CLS : public ConsoleCommand
{
public:
	IC_Command_CLS();
	virtual ~IC_Command_CLS();
	bool invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput);

};

class IC_Command_SCRIPT : public ConsoleCommand
{
public:
	IC_Command_SCRIPT();
	virtual ~IC_Command_SCRIPT();
	bool invoke(const StringArray& args, CommandDispatcher* pDispatcher, MessageSink* pOutput);

};

#endif
