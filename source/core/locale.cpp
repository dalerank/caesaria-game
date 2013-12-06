// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "locale.hpp"
#include "bytearray.hpp"
#include "logger.hpp"

#include <libintl.h>

extern int  _nl_msg_cat_cntr;

void Locale::loadTranslator(vfs::Directory directory)
{
  bindtextdomain( "caesar", directory.toString().c_str() );
  bind_textdomain_codeset( "caesar", "UTF-8" );
  textdomain( "caesar" );
}

void Locale::setLanguage(std::string language)
{
#ifdef CAESARIA_PLATFORM_WIN
  std::string localeData = StringHelper::format( 0xff, "LANGUAGE=%s", language.c_str() );
  putenv( localeData.c_str() );
#elif defined(CAESARIA_PLATFORM_UNIX)
  setenv( "LANGUAGE", language.c_str(), 1 );
#endif

  ++_nl_msg_cat_cntr;
}
