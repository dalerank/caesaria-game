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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "locale.hpp"
#include "bytearray.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "saveadapter.hpp"

namespace {
  typedef std::map< int, std::string > Translator;
  Translator translator;
  vfs::Directory directory;
  std::string currentLanguage;
  std::string invalidText = "";

static void __loadTranslator( vfs::Path filename )
{  
  VariantMap trs = SaveAdapter::load( directory/filename );
  Logger::warning( "Locale: load translation from " + (directory/filename).toString() );

  foreach( it, trs )
  {
    int hash = utils::hash( it->first );
    Translator::iterator trIt = translator.find( hash );
    Logger::warningIf( trIt != translator.end(), "Locale: also have translation for " + it->first );

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
  translator.clear();
  currentLanguage = language;
  addTranslation( "caesar" );
}

std::string Locale::current()
{
  return currentLanguage;
}

void Locale::addTranslation(std::string filename)
{
  vfs::Path realPath = filename + "." + currentLanguage;
  __loadTranslator( realPath );
}

const char* Locale::translate( const std::string& text)
{
  int hash = utils::hash( text );
  Translator::iterator it = translator.find( hash );

  return ( it != translator.end()
                 ? (it->second.empty() ? text.c_str() : it->second.c_str())
                 : text.c_str() );
}
