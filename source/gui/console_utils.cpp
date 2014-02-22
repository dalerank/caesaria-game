#include "console_utils.hpp"

/*
NERPA_MODULE_BEGIN(core)

ConsoleError::ConsoleError(const core::String str, const s32 code) : errorMessage(str), errorCode(code)
{
}
//=====================================================================================
ConsoleError::ConsoleError(const ConsoleError& err)
{
	*this = err;
}
//=====================================================================================
ConsoleError::~ConsoleError()
{
}
//=====================================================================================	
const ConsoleError& ConsoleError::operator = (const ConsoleError& err)
{
	setMessage(err.getMessage());
	setCode(err.getCode());
	return *this;
}
//=====================================================================================
const core::String ConsoleError::getMessage() const
{ 
	return errorMessage; 
}
//=====================================================================================
const s32 ConsoleError::getCode() const
{
	return errorCode; 
}
//=====================================================================================
void ConsoleError::setMessage(const core::String str)
{
	errorMessage = str;
}
//=====================================================================================
void ConsoleError::setCode(const s32 code)
{
	errorCode = code;
}
//=====================================================================================

ConsoleParser::ConsoleParser(const core::String& line) : cmdLine(line)
{
	bShouldAddLast = false;
	bNameDone = false;
	bEscape = false;
	bQuote = false;
}
ConsoleParser::~ConsoleParser()
{
}
bool ConsoleParser::parse(core::String& cmdName, core::Array<core::String>& args)
{
	//cout<< "Parsing : [" << cmdLine.c_str() << "]" <<endl;
	static const wchar_t spaceChar = (wchar_t)' ';
	args.clear();
	cmdName = L"";

	if(cmdLine.findFirst(spaceChar) == -1)
	{
		cmdName = cmdLine;
		return true;
	}
	
	setQuoted(false);
	setEscaped(false);
	setNameDone(false);
	bShouldAddLast = true;
	resetTmpString();

	for(size_t x = 0; x < cmdLine.size(); x++)
	{
		if(!handleChar(cmdLine[x],cmdName,args))
		{
			return false;
		}
	}
	if(bShouldAddLast)
	{
		shoveTmpString(cmdLine,args);
	}
	return true;
}
bool ConsoleParser::handleChar(wchar_t wc, core::String& cmdName, core::Array<core::String>& args)
{
	static const wchar_t spaceChar = (wchar_t)' ';
	static const wchar_t escapeChar = (wchar_t)'\\';
	static const wchar_t quoteChar = (wchar_t)'\"';
	if(wc == spaceChar)
	{
		if(!isQuoted())
		{
			shoveTmpString(cmdName,args);
		}
		else
		{
			tmpString += (spaceChar);
			bShouldAddLast = true;
		}
	}
	else if(wc == quoteChar)
	{
		if(isEscaped())
		{
			
			tmpString += quoteChar;
			bShouldAddLast = true;
			setEscaped(false);
		}
		else if(isQuoted())
		{
			shoveTmpString(cmdName,args);
			setQuoted(false);
		}
		else
		{
			setQuoted(true);
		}

	}
	else if(wc == escapeChar)
	{
		if(isEscaped())
		{
			tmpString += escapeChar;
			bShouldAddLast = true;
			setEscaped(false);
		}
		else
		{
			setEscaped(true);
		}
	}
	else
	{
		if(isEscaped())
		{
			return false;
		}
		tmpString += wc;
		bShouldAddLast = true;
	}
	return true;
}
void ConsoleParser::shoveTmpString(core::String& cmdName, core::Array<core::String>& args)
{
	if(bNameDone)
	{
		//cout << "arg : " << tmpString.c_str() <<endl;
		args.push_back(tmpString);
	}
	else 
	{
		//cout <<"name : " << tmpString.c_str() << endl;
		cmdName = tmpString;
		setNameDone(true);
	}
	bShouldAddLast = false;
	resetTmpString();
}
void ConsoleParser::resetTmpString()
{
	tmpString = L"";
}
bool ConsoleParser::isQuoted()
{
	return bQuote;
}
bool ConsoleParser::isEscaped()
{
	return bEscape;
}
bool ConsoleParser::isNameDone()
{
	return bNameDone;
}
void ConsoleParser::setQuoted(bool bVal)
{
	bQuote = bVal;
}
void ConsoleParser::setEscaped(bool bVal)
{
	bEscape = bVal;
}
void ConsoleParser::setNameDone(bool bVal)
{
	bNameDone = bVal;
}
//=====================================================================================

NERPA_MODULE_END(core)
*/
