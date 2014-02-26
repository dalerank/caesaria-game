#ifndef _CAESARIA_CONSOLE_UTILS_HPP_INCLUDE_
#define _CAESARIA_CONSOLE_UTILS_HPP_INCLUDE_

#include "core/stringarray.hpp"

//! a basic error class
class ConsoleError
{
public:
	ConsoleError(const std::string str="", const int code=-1);
	ConsoleError(const ConsoleError& err);

	virtual ~ConsoleError();
	
	const ConsoleError& operator = (const ConsoleError& err);
	virtual const std::string getType() const{ return "Generic Error"; }
	const std::string getMessage() const;
	const int getCode() const;
protected:
	void setMessage(const std::string str);
	void setCode(const int code);
private:
	std::string errorMessage;
	int		      errorCode;
};

class ConsoleParser
{
public:
	ConsoleParser(const std::string& line);
	virtual ~ConsoleParser();
	bool parse( std::string& cmdName, StringArray& args);
private:
	void shoveTmpString( std::string& cmdName, StringArray& args);
	bool handleChar( char wc, std::string& cmdName, StringArray& args);
	void resetTmpString();
	bool isQuoted();
	bool isEscaped();
	bool isNameDone();
	void setQuoted(bool bVal);
	void setEscaped(bool bVal);
	void setNameDone(bool bVal);

	std::string cmdLine;
	bool bQuote,bEscape, bNameDone,bShouldAddLast;
	std::string tmpString;
};

#endif //_CAESARIA_CONSOLE_UTILS_HPP_INCLUDE_
