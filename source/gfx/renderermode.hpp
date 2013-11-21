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


#ifndef __OPENCAESAR3_TILEMAPCHANGECOMMAND_H_INCLUDE_
#define __OPENCAESAR3_TILEMAPCHANGECOMMAND_H_INCLUDE_

#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "game/construction.hpp"
#include "gfx/renderer.hpp"

class LayerMode : public Renderer::Mode
{
public:
  static Renderer::ModePtr create( const int type );

  int getType() const;

protected:
  void _setType( int type );
  LayerMode( int type );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

class DestroyMode : public LayerMode
{
public:
  static Renderer::ModePtr create();
private:
  DestroyMode();
};

class BuildMode : public LayerMode
{
public:
  static Renderer::ModePtr create( TileOverlay::Type type );

  ConstructionPtr getContruction() const;
  bool isBorderBuilding() const;
  bool isMultiBuilding() const;
public:
  BuildMode();

  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr< BuildMode > BuildModePtr;
typedef SmartPtr< LayerMode > LayerModePtr;

#endif
