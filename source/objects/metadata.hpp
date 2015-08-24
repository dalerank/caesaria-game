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

#ifndef _CAESARIA_OBJECTS_METADATA_H_INCLUDE_
#define _CAESARIA_OBJECTS_METADATA_H_INCLUDE_

#include "good/good.hpp"
#include "core/scopedptr.hpp"
#include "vfs/path.hpp"
#include "gfx/picture.hpp"
#include "core/variant.hpp"
#include "objects/overlay.hpp"

// contains some metaData for a building type
class MetaDataOptions
{
public:
  static const char* cost;
  static const char* requestDestroy;
  static const char* employers;
  static const char* c3logic;
};

class MetaData
{
  friend class MetaDataHolder;
  MetaData( const object::Type type, const std::string& name );

  void initialize(const VariantMap& options);

public:
  static MetaData invalid;

  MetaData( const MetaData& a );

  ~MetaData();

  std::string name() const;
  std::string sound() const;
  std::string prettyName() const;
  std::string description() const;
  bool checkWalkersOnBuild() const;
  object::Type type() const;
  object::Group group() const;
  gfx::Picture picture( int size=0 ) const;
  Desirability desirability() const;

  Variant getOption( const std::string& name, Variant defaultVal=Variant() ) const;

  MetaData& operator=( const MetaData& a );
private:
  class Impl;
  ScopedPtr<Impl> _d;
};

// contains some metaData for each building type
class MetaDataHolder
{
public:
  static MetaDataHolder& instance();

  void addData(const MetaData& data, bool force);
  static const MetaData& find(const object::Type buildingType);
  bool hasData(const object::Type buildingType) const;
  object::Types availableTypes() const;

  // return factory that consume good
  object::Type getConsumerType(const good::Product inGoodType) const;

  static object::Group findGroup( const std::string& name );
  static std::string findGroupname( object::Group group );

  static std::string findPrettyName( object::Type type );
  static std::string findDescription( object::Type type );
  static gfx::Picture randomPicture( object::Type type, Size size );

  void initialize(vfs::Path filename );
  void reload( const object::Type type );
  ~MetaDataHolder();
private:
  MetaDataHolder();
  void _loadConfig(object::Type type, const std::string &name, const VariantMap& options, bool force);

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_OBJECTS_METADATA_H_INCLUDE_
