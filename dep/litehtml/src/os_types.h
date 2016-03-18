#pragma once

namespace litehtml
{
	#define LITEHTML_UTF8

	typedef std::string			tstring;
	typedef char				tchar_t;
	typedef void*				uint_ptr;
	typedef std::stringstream	tstringstream;

	#define _t(quote)			quote

	#define t_strlen			strlen
	#define t_strcmp			strcmp
	#define t_strncmp			strncmp

	#define t_strcasecmp		stricmp
	#define t_strncasecmp		strnicmp
	#define t_itoa(value, buffer, size, radix)	snprintf(buffer, size, "%d", value)

	#define t_strtol			strtol
	#define t_atoi				atoi
	#define t_strtod			strtod
	#define t_strstr			strstr
	#define t_tolower			tolower
	#define t_isdigit			isdigit
}
