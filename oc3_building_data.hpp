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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef BUILDING_DATA_HPP
#define BUILDING_DATA_HPP

#include "oc3_good.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_filepath.hpp"
#include "oc3_picture.hpp"
#include "oc3_variant.hpp"

// contains some metaData for a building type
class BuildingData
{
  friend class BuildingDataHolder;

  static BuildingData invalid;
public:
  typedef struct
  {
    int base;
    int range;
    int step;
  } Desirability;

  BuildingData( const TileOverlayType buildingType, const std::string &name, const int cost );
  BuildingData( const BuildingData& a );

  ~BuildingData();

  std::string getName() const;
  std::string getPrettyName() const;
  TileOverlayType getType() const;
  TileOverlayGroup getClass() const;
  const Picture& getBasePicture() const;
  // returns the building price, -1 => cannot be built
  int getCost() const;
  const Desirability& getDesirbilityInfo() const;

  Variant getOption( const std::string& name, Variant defaultVal=Variant() ) const;

  BuildingData& operator=( const BuildingData& a );

private:
  TileOverlayGroup _buildingClass;
  std::string _name;  // debug name  (english, ex:"iron")
  std::string _prettyName;  // pretty-print name  (i18n, ex:"Iron mine")
  Picture _basePicture;
  int _cost;

  class Impl;
  ScopedPtr< Impl > _d;
};

// contains some metaData for each building type
class BuildingDataHolder
{
public:
   static BuildingDataHolder& instance();

   void addData(const BuildingData &data);
   const BuildingData& getData(const TileOverlayType buildingType) const;
   bool hasData(const TileOverlayType buildingType) const;

   // return factory that consume goodType
   TileOverlayType getConsumerType(const Good::Type inGoodType) const;

   static TileOverlayType getType( const std::string& name );
   static TileOverlayGroup getClass( const std::string& name );

   static std::string getPrettyName( TileOverlayType bType );

   void initialize( const io::FilePath& filename );
private:
   BuildingDataHolder();

   class Impl;
   ScopedPtr< Impl > _d;
};


#endif
