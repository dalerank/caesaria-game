#include "console_utils.hpp"

ConsoleError::ConsoleError(const std::string str, const int code) : errorMessage(str), errorCode(code)
{
}

ConsoleError::ConsoleError(const ConsoleError& err){	*this = err;}
ConsoleError::~ConsoleError(){}

const ConsoleError& ConsoleError::operator = (const ConsoleError& err)
{
	setMessage(err.getMessage());
	setCode(err.getCode());
	return *this;
}

const std::string ConsoleError::getMessage() const{ 	return errorMessage; }
const int ConsoleError::getCode() const{	return errorCode; }
void ConsoleError::setMessage(const std::string str){	errorMessage = str;}
void ConsoleError::setCode(const int code){	errorCode = code;}

ConsoleParser::ConsoleParser(const std::string& line) : cmdLine(line)
{
	bShouldAddLast = false;
	bNameDone = false;
	bEscape = false;
	bQuote = false;
}

ConsoleParser::~ConsoleParser(){}

bool ConsoleParser::parse(std::string& cmdName, StringArray& args)
{
	//cout<< "Parsing : [" << cmdLine.c_str() << "]" <<endl;
	static const char spaceChar = ' ';
	args.clear();
	cmdName = "";

	if(cmdLine.find(spaceChar) == std::string::npos)
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

bool ConsoleParser::handleChar( char wc, std::string& cmdName, StringArray& args)
{
	static const char spaceChar = ' ';
	static const char escapeChar = '\\';
	static const char quoteChar = '\"';
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

void ConsoleParser::shoveTmpString(std::string& cmdName, StringArray& args)
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

void ConsoleParser::resetTmpString(){	tmpString = "";}
bool ConsoleParser::isQuoted(){	return bQuote;}
bool ConsoleParser::isEscaped(){	return bEscape;}
bool ConsoleParser::isNameDone(){	return bNameDone;}
void ConsoleParser::setQuoted(bool bVal){	bQuote = bVal;}
void ConsoleParser::setEscaped(bool bVal){ bEscape = bVal;}
void ConsoleParser::setNameDone(bool bVal){	bNameDone = bVal;}
