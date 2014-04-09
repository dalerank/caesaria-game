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


#ifndef _CAESARIA_BUILDING_METADATA_H_INCLUDE_
#define _CAESARIA_BUILDING_METADATA_H_INCLUDE_

#include "good/good.hpp"
#include "core/scopedptr.hpp"
#include "vfs/path.hpp"
#include "gfx/picture.hpp"
#include "core/variant.hpp"
#include "gfx/tileoverlay.hpp"
#include "desirability.hpp"

// contains some metaData for a building type
class MetaData
{
  friend class MetaDataHolder;

  static MetaData invalid;
public:
  MetaData( const gfx::TileOverlay::Type type, const std::string& name );
  MetaData( const MetaData& a );

  ~MetaData();

  std::string getName() const;
  std::string getSound() const;
  std::string getPrettyName() const;
  std::string getDescription() const;
  gfx::TileOverlay::Type getType() const;
  gfx::TileOverlay::Group getGroup() const;
  gfx::Picture getBasePicture() const;
  Desirability getDesirbility() const;

  Variant getOption( const std::string& name, Variant defaultVal=Variant() ) const;

  MetaData& operator=( const MetaData& a );

private:
  std::string _prettyName;  // pretty-print name  (i18n, ex:"Iron mine")

  class Impl;
  ScopedPtr< Impl > _d;
};

// contains some metaData for each building type
class MetaDataHolder
{
public:
   static MetaDataHolder& instance();

   void addData(const MetaData& data);
   const MetaData& getData(const gfx::TileOverlay::Type buildingType) const;
   bool hasData(const gfx::TileOverlay::Type buildingType) const;

   // return factory that consume goodType
   gfx::TileOverlay::Type getConsumerType(const Good::Type inGoodType) const;

   static gfx::TileOverlay::Type getType( const std::string& name );
   static std::string getTypename( gfx::TileOverlay::Type type );
   static gfx::TileOverlay::Group getClass( const std::string& name );

   static std::string getPrettyName( gfx::TileOverlay::Type type );
   static std::string getDescription( gfx::TileOverlay::Type type );

   void initialize( const vfs::Path& filename );
private:
   MetaDataHolder();

   class Impl;
   ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_BUILDING_METADATA_H_INCLUDE_
