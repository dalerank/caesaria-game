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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_OBJECTS_METADATAHOLDER_H_INCLUDE_
#define _CAESARIA_OBJECTS_METADATAHOLDER_H_INCLUDE_

#include "metadata.hpp"
#include "gfx/picture.hpp"

// contains some metaData for a building type
namespace object
{

// contains some metaData for each building type
class InfoDB
{
public:
  static InfoDB& instance();

  void addData(const object::Info& data, bool force);
  static const Info& find(const object::Type buildingType);
  bool hasData(const object::Type buildingType) const;
  object::Types availableTypes() const;

  // return factory that consume good
  object::Type getConsumerType(const good::Product inGoodType) const;

  static object::Group findGroup( const std::string& name );
  static std::string findGroupname( object::Group group );

  static std::string findPrettyName( object::Type type );
  static std::string findDescription( object::Type type );

  void initialize(vfs::Path filename );
  void reload( const object::Type type );
  ~InfoDB();
private:
  InfoDB();
  void _loadConfig(object::Type type, const std::string &name, const VariantMap& options, bool force);

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namepsace object

#endif //_CAESARIA_OBJECTS_METADATAHOLDER_H_INCLUDE_
