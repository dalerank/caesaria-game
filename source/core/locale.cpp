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
#include "stringhelper.hpp"
#include "saveadapter.hpp"

namespace {
  typedef std::map< int, std::string > Translator;
  Translator translator;
  vfs::Directory directory;
  std::string invalidText = "";

static void __loadTranslator( vfs::Path filename )
{  
  translator.clear();
  VariantMap trs = SaveAdapter::load( directory/filename );
  Logger::warning( (directory/filename).toString() );

  for( VariantMap::iterator it=trs.begin(); it != trs.end(); it++ )
  {
    int hash = StringHelper::hash( it->first );
    translator[ hash ] = it->second.toString();
  }
}

}

void Locale::setDirectory(vfs::Directory dir)
{
  directory = dir;
}

void Locale::setLanguage(std::string language)
{
  vfs::Path filename = "caesar." + language;
  __loadTranslator( filename );
}

const char* Locale::translate(const char* text)
{
  int hash = StringHelper::hash( text );
  Translator::iterator it = translator.find( hash );

  return ( it != translator.end() ? it->second.c_str() : text );
}
