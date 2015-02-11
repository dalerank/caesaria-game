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

#include "name_generator.hpp"
#include "core/stringarray.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"

namespace {
static const char* defaultExt = "en";
}

class NameGenerator::Impl
{
public:
  StringArray male;
  StringArray female;
  StringArray surname;

  vfs::Path fileTemplate;
};

NameGenerator& NameGenerator::instance()
{
  static NameGenerator inst;
  return inst;
}

NameGenerator::~NameGenerator(){}

std::string NameGenerator::rand( NameType type )
{
  const NameGenerator& ng = instance();

  StringArray* names;
  switch( type )
  {
  case male: names = &ng._d->male; break;
  case female: names = &ng._d->female; break;
  case patricianMale: names = &ng._d->male; break;
  case patricianFemale: names = &ng._d->female; break;
  }

  return names->random() + " " + ng._d->surname.random();
}

void NameGenerator::initialize( vfs::Path filename)
{
  _d->fileTemplate = filename;
}

void NameGenerator::setLanguage(const std::string& language)
{
  vfs::Path filename = _d->fileTemplate.changeExtension( language );
  if( !filename.exist() )
    filename.changeExtension( defaultExt );

  VariantMap names = config::load( filename );

  _d->female.clear();
  _d->male.clear();
  _d->surname.clear();

  VariantMap ctNames = names.get( "citizens" ).toMap();
  _d->male << ctNames.get( "male" ).toList();
  _d->female << ctNames.get( "female" ).toList();
  _d->surname << ctNames.get( "surname" ).toList();
}

NameGenerator::NameGenerator() : _d( new Impl ) {}
