#ifndef _Console_Utils_h_
#define _Console_Utils_h_

/*
NERPA_MODULE_BEGIN(core)

//! a basic error class
class ConsoleError
{
public:
	ConsoleError(const core::String str=L"", const s32 code=-1);
	ConsoleError(const ConsoleError& err);

	virtual ~ConsoleError();
	
	const ConsoleError& operator = (const ConsoleError& err);
	virtual const core::String getType() const{ return L"Generic Error"; }
	const core::String getMessage() const;
	const s32 getCode() const;
protected:
	void setMessage(const core::String str);
	void setCode(const s32 code);
private:
	core::String errorMessage;
	s32		   errorCode;
};

class ConsoleParser
{
public:
	ConsoleParser(const String& line);
	virtual ~ConsoleParser();
	bool parse( String& cmdName, Array< String >& args);
private:
	void shoveTmpString( String& cmdName, Array< String >& args);
	bool handleChar( wchar_t wc, String& cmdName, Array< String >& args);
	void resetTmpString();
	bool isQuoted();
	bool isEscaped();
	bool isNameDone();
	void setQuoted(bool bVal);
	void setEscaped(bool bVal);
	void setNameDone(bool bVal);

	String cmdLine;
	bool bQuote,bEscape, bNameDone,bShouldAddLast;
	String tmpString;
};

NERPA_MODULE_END(core)

//! overloaded operator for inserting a wide string to the stl ostream
inline std::ostream& operator<<(std::ostream& os, const nrp::core::String& s)
{
	return (os << s.c_str());
}
*/
#endif
