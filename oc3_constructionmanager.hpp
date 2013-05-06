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

#ifndef __OPENCAESAR3_CONSTRUCTIONMANAGER_H_INCLUDE_
#define __OPENCAESAR3_CONSTRUCTIONMANAGER_H_INCLUDE_

#include "oc3_enums.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_predefinitions.hpp"

class Construction;

class AbstractConstructor
{
public:
  virtual Construction* create() = 0;
};

template< class T > class BaseConstructor : public AbstractConstructor
{
public:
  BaseConstructor() 
  {
  }

  Construction* create() 
  {
    return new T(); 
  }
};

class ConstructionManager
{
public:
    static ConstructionManager& getInstance();
    ConstructionPtr create(const BuildingType buildingType) const;
    ConstructionPtr create(const std::string& typeName ) const;

    bool canCreate( const BuildingType type ) const;

    void addConstructor( const BuildingType type, const std::string& typeName, AbstractConstructor* ctor );
private:
    ConstructionManager();

    class Impl;
    ScopedPtr< Impl > _d;
};

#endif  //__OPENCAESAR3_CONSTRUCTIONMANAGER_H_INCLUDE_
