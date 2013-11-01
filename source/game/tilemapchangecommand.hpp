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

#include "core/scopedptr.hpp"
#include "enums.hpp"
#include "core/predefinitions.hpp"
#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "construction.hpp"

class TilemapChangeCommand : public ReferenceCounted
{
public:
   virtual ~TilemapChangeCommand();
};

typedef SmartPtr< TilemapChangeCommand > TilemapChangeCommandPtr;

class TilemapBuildCommand : public TilemapChangeCommand
{
public:
  static TilemapChangeCommandPtr create( TileOverlay::Type type );

  ConstructionPtr getContruction() const;
  bool isBorderBuilding() const;
  bool isMultiBuilding() const;
  bool isCanBuild() const;
  void setCanBuild( bool cb );
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
  static TilemapChangeCommandPtr create( const DrawingOverlayType type );

  DrawingOverlayType getType() const;

private:
  TilemapOverlayCommand();

  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr< TilemapOverlayCommand > TilemapOverlayCommandPtr;

#endif
