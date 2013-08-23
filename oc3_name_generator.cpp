// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.


#include "oc3_name_generator.hpp"
#include "oc3_stringarray.hpp"
#include "oc3_saveadapter.hpp"
#include "oc3_variant.hpp"

class NameGenerator::Impl
{
public:
  StringArray male;
  StringArray female;
  StringArray surname;
};

NameGenerator& NameGenerator::instance()
{
  static NameGenerator inst;
  return inst;
}

NameGenerator::~NameGenerator()
{

}

std::string NameGenerator::rand( NameType type )
{
  const NameGenerator& ng = instance();

  const StringArray& names = type == male ? ng._d->male : ng._d->female;
  int nameIndex = std::rand() % names.size();
  int surIndex = std::rand() % ng._d->surname.size();

  return names[ nameIndex ] + " " + ng._d->surname[ surIndex ];
}

void NameGenerator::initialize(const io::FilePath &filename)
{
  VariantMap names = SaveAdapter::load( filename );

  NameGenerator& ng = instance();
  ng._d->female.clear();
  ng._d->male.clear();
  ng._d->surname.clear();

  VariantMap ctNames = names.get( "citizens" ).toMap();
  ng._d->male << ctNames.get( "male" ).toList();
  ng._d->female << ctNames.get( "female" ).toList();
  ng._d->surname << ctNames.get( "surname" ).toList();
}

NameGenerator::NameGenerator() : _d( new Impl )
{

}
