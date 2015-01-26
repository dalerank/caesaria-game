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
#include "gfx/tileoverlay.hpp"

// contains some metaData for a building type
class MetaDataOptions
{
public:
  static const char* cost;
  static const char* requestDestroy;
  static const char* employers;
};

class MetaData
{
  friend class MetaDataHolder;

  static MetaData invalid;
public:
  MetaData( const gfx::TileOverlay::Type type, const std::string& name );
  MetaData( const MetaData& a );

  ~MetaData();

  std::string name() const;
  std::string sound() const;
  std::string prettyName() const;
  std::string description() const;
  gfx::TileOverlay::Type type() const;
  gfx::TileOverlay::Group group() const;
  gfx::Picture picture( int size=0 ) const;
  Desirability desirability() const;

  Variant getOption( const std::string& name, Variant defaultVal=Variant() ) const;

  MetaData& operator=( const MetaData& a );
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

// contains some metaData for each building type
class MetaDataHolder
{
public:
  typedef std::vector<gfx::TileOverlay::Type> OverlayTypes;
  static MetaDataHolder& instance();

  void addData(const MetaData& data);
  static const MetaData& getData(const gfx::TileOverlay::Type buildingType);
  bool hasData(const gfx::TileOverlay::Type buildingType) const;
  OverlayTypes availableTypes() const;

  // return factory that consume good
  gfx::TileOverlay::Type getConsumerType(const good::Product inGoodType) const;

  static gfx::TileOverlay::Type findType( const std::string& name );
  static std::string findTypename( gfx::TileOverlay::Type type );
  static gfx::TileOverlay::Group findGroup( const std::string& name );

  static std::string findPrettyName( gfx::TileOverlay::Type type );
  static std::string findDescription( gfx::TileOverlay::Type type );
  static gfx::Picture randomPicture( gfx::TileOverlay::Type type, Size size );

  void initialize(vfs::Path filename );
  ~MetaDataHolder();
private:
  MetaDataHolder();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_OBJECTS_METADATA_H_INCLUDE_
