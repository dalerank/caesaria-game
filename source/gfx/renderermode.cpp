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

#include "renderermode.hpp"
#include "objects/objects_factory.hpp"
#include "objects/building.hpp"
#include "objects/constants.hpp"
#include "layers/constants.hpp"

namespace gfx
{

typedef std::set<LayerMode::Flag> Flags;

class LayerMode::Impl
{
public:
  int type;
  Flags flags;
};

Renderer::ModePtr LayerMode::create( const int type )
{
  Renderer::ModePtr ret( new LayerMode( type ) );
  ret->drop();

  return ret;
}

LayerMode::LayerMode(int type) : _d( new Impl )
{
  _d->type = type;
  //_d->type = citylayer::simple;
}

int LayerMode::type() const {  return _d->type;}

bool LayerMode::flag(LayerMode::Flag name) { return _d->flags.count( name ) > 0; }
void LayerMode::_setType(int type){  _d->type = type;}
void LayerMode::_setFlag(LayerMode::Flag name, bool value)
{
  if( value ) _d->flags.insert( name );
  else _d->flags.erase( name );
}

class BuildMode::Impl
{
public:
  ConstructionPtr construction;
};

ConstructionPtr BuildMode::contruction() const{    return _d->construction;}

DestroyMode::DestroyMode() : LayerMode( citylayer::destroyd )
{
}

Renderer::ModePtr DestroyMode::create()
{
  Renderer::ModePtr ret( new DestroyMode() );
  ret->drop();

  return ret;
}

Renderer::ModePtr BuildMode::create(object::Type type)
{
  BuildMode* newCommand = new BuildMode();
  OverlayPtr overlay = TileOverlayFactory::instance().create( type );

  auto md = object::Info::find( type );

  newCommand->_d->construction = overlay.as<Construction>();
  newCommand->_setFlag( multibuild, false );
  newCommand->_setFlag( border, false );
  newCommand->_setFlag( assign2road, false );
  newCommand->_setFlag( checkWalkers, md.checkWalkersOnBuild() );

  if( type == object::road )
  {
    newCommand->_setFlag( border, true );
    newCommand->_setFlag( multibuild, true );
    newCommand->_setFlag( assign2road, false );
  }
  else if( type == object::aqueduct ||
           type == object::wall ||
           type == object::fortification )
  {
    newCommand->_setFlag( border, true );
    newCommand->_setFlag( multibuild, true );
  }
  else if( type == object::garden )
  {
    newCommand->_setFlag( multibuild, true );
  }
  else if( type == object::house || type == object::plaza )
  {
    newCommand->_setFlag( multibuild, true );
  }

  Renderer::ModePtr ret( newCommand );
  ret->drop();

  return ret;
}

BuildMode::BuildMode()
  : LayerMode( citylayer::build ), _d( new Impl )
{
}

}//end namespace gfx
