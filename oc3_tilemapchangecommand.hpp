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


#ifndef __OPENCAESAR3_TILEMAPCHANGECOMMAND_H_INCLUDE_
#define __OPENCAESAR3_TILEMAPCHANGECOMMAND_H_INCLUDE_

#include "oc3_scopedptr.hpp"
#include "oc3_enums.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_smartptr.hpp"

class Construction;

class TilemapChangeCommand : public ReferenceCounted
{
public:
   virtual ~TilemapChangeCommand();
};

typedef SmartPtr< TilemapChangeCommand > TilemapChangeCommandPtr;

class TilemapBuildCommand : public TilemapChangeCommand
{
public:
  static TilemapChangeCommandPtr create( BuildingType type );

  ConstructionPtr getContruction() const;
  bool isBorderBuilding() const;
  bool isMultiBuilding() const;
public:
  TilemapBuildCommand();
  
  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr< TilemapBuildCommand > TilemapBuildCommandPtr;

class TilemapRemoveCommand : public TilemapChangeCommand
{
public:
  static TilemapChangeCommandPtr create();
private:
  TilemapRemoveCommand();
};

class TilemapOverlayCommand : public TilemapChangeCommand
{
public:
  static TilemapChangeCommandPtr create( const OverlayType type );

  OverlayType getType() const;

private:
  TilemapOverlayCommand();

  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr< TilemapOverlayCommand > TilemapOverlayCommandPtr;

#endif