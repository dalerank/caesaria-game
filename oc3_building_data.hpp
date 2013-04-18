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

#include <string>
#include <map>

#include "enums.hpp"

// contains some metaData for a building type
class BuildingData
{
public:
   BuildingData(const BuildingType buildingType, const std::string &name, const int cost);

   std::string getName() const;
   std::string getPrettyName() const;
   BuildingType getType() const;
   // returns the building price, -1 => cannot be built
   int getCost() const;

private:
   BuildingType _buildingType;
   std::string _name;  // debug name  (english, ex:"iron")
   std::string _prettyName;  // pretty-print name  (i18n, ex:"Mine de fer")
   int _cost;
};


// contains some metaData for each building type
class BuildingDataHolder
{
public:
   static BuildingDataHolder& instance();

   void addData(const BuildingData &data);
   BuildingData& getData(const BuildingType buildingType);
   bool hasData(const BuildingType buildingType);

   // return building that serve service
   BuildingType getBuildingTypeByService(const ServiceType serviceType);
   // return factory that consume goodType
   BuildingType getBuildingTypeByInGood(const GoodType inGoodType);
   // return factory that produce goodType
   BuildingType getBuildingTypeByOutGood(const GoodType outGoodType);

private:
   BuildingDataHolder();

   std::map<BuildingType, BuildingData> _mapDataByName;  // key=building_type, value=data
   std::map<ServiceType, BuildingType> _mapBuildingByService;
   std::map<GoodType, BuildingType> _mapBuildingByInGood;
   std::map<GoodType, BuildingType> _mapBuildingByOutGood;
   static BuildingDataHolder *_instance;
};


#endif
