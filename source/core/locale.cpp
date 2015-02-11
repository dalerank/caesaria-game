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
#include "variant_map.hpp"
#include "saveadapter.hpp"

namespace {
  struct tr
  {
    std::string def_text;
    std::string text;
  };
  
  typedef std::map< int, tr > Translator;
  Translator translator;
  vfs::Directory directory;
  std::string currentLanguage;
  std::string invalidText = "";

static void __loadTranslator( vfs::Path filename )
{  
  VariantMap trs = config::load( directory/filename );
  Logger::warning( "Locale: load translation from " + (directory/filename).toString() );

  foreach( it, trs )
  {
    int hash = utils::hash( it->first );
    translator[ hash ].text = it->second.toString();
  }
}

static void __loadDefault()
{
  vfs::Path filename( "caesar.en" );
  VariantMap trs = config::load( directory/filename );
  Logger::warning( "Locale: load default translation from " + (directory/filename).toString() );

  foreach( it, trs )
  {
    int hash = utils::hash( it->first );
    translator[ hash ].def_text = it->second.toString();
  }
}

}

void Locale::setDirectory(vfs::Directory dir)
{
  directory = dir;
  translator.clear();
  __loadDefault();
}

void Locale::setLanguage(std::string language)
{
  currentLanguage = language;

  foreach( it, translator )
  {
    it->second.text.clear();
  }

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

  if( it == translator.end() )
  {
    return text.c_str();
  }
  else
  {
    const tr& item = it->second;
    const std::string& ret = item.text.empty() ? item.def_text : item.text;
    return ( ret.empty() ? text : ret ).c_str();
  }
}
