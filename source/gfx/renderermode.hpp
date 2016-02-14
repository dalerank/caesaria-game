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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_LAYERMODE_H_INCLUDE_
#define __CAESARIA_LAYERMODE_H_INCLUDE_

#include "gfx/renderer.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "objects/construction.hpp"

namespace gfx
{

class LayerMode : public Renderer::Mode
{
public:
  typedef enum { border=0, multibuild, checkWalkers, assign2road } Flag;
  static Renderer::ModePtr create( const int type );

  int type() const;
  bool flag( Flag name );
protected:
  void _setType( int type );
  void _setFlag( Flag name, bool value );
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
  static Renderer::ModePtr create( object::Type type );

  ConstructionPtr contruction() const;
  OverlayPtr overlay() const;

protected:
  BuildMode( int layer );

  class Impl;
  ScopedPtr< Impl > _d;
};

class EditorMode : public BuildMode
{
public:
  static Renderer::ModePtr create( object::Type type );

public:
  EditorMode();
};

typedef SmartPtr< LayerMode > LayerModePtr;

}//end namespace gfx

#endif  //__CAESARIA_LAYERMODE_H_INCLUDE_
