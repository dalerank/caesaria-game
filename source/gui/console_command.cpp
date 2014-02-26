#include "console_command.hpp"
#include "message_sink.hpp"
#include "console_utils.hpp"

//! constructor
ConsoleCommand::ConsoleCommand(const std::string& name) : command_name_(name)
{

}

//! the destructor
ConsoleCommand::~ConsoleCommand(){}

//! get command name
const std::string ConsoleCommand::GetName(){	return command_name_;}

//! get the usage string for the command
const std::string ConsoleCommand::GetUsage(){	return command_usage_;}

//! get the description line count
const unsigned int ConsoleCommand::GetDescLineCount(){	return command_declines_.size();}

//! get the desc line at
const std::string ConsoleCommand::GetDescLine(const unsigned int index){	return command_declines_[index];}

//! print the usage to the message sink
void ConsoleCommand::PrintUsage(MessageSink* pOutput)
{
	std::string wstr = "Command : ";
	wstr += GetName();
	pOutput->AppendMessage( wstr );

	wstr = "usage : ";
	pOutput->AppendMessage( wstr );

	wstr = " ";
	wstr += GetUsage();
	pOutput->AppendMessage( wstr );

}

//! print the description to the message sink
void ConsoleCommand::PrintDesc(MessageSink* pOutput)
{
	PrintUsage( pOutput );

	std::string wstr = "description";
	pOutput->AppendMessage( wstr );

	for(unsigned int i = 0; i < command_declines_.size(); i++)
	{
		wstr = " ";
		wstr += command_declines_[i];
		pOutput->AppendMessage( wstr );
	}
}

//! set usage
void ConsoleCommand::SetUsage(const std::string& usage) {	command_usage_ = usage;}

//! append a line to the desc array
void ConsoleCommand::AddDescLine(const std::string& line){	command_declines_.push_back(line);}

//! set descrition to ""
void ConsoleCommand::ClearDescLines(){	command_declines_.clear();}

