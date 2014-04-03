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

#include "renderermode.hpp"
#include "objects/objects_factory.hpp"
#include "objects/building.hpp"
#include "objects/constants.hpp"
#include "layerconstants.hpp"

using namespace constants;

namespace gfx
{

class LayerMode::Impl
{
public:
  int type;
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

int LayerMode::getType() const {  return _d->type;}
void LayerMode::_setType(int type){  _d->type = type;}

class BuildMode::Impl
{
public:
    bool isBorderBuilding;
    bool isMultiBuilding;
    bool isRoadAssignment;
    ConstructionPtr construction;
};

bool BuildMode::isBorderBuilding() const {    return _d->isBorderBuilding;}
bool BuildMode::isMultiBuilding() const{  return _d->isMultiBuilding;}
bool BuildMode::isRoadAssignment() const{  return _d->isRoadAssignment;}
ConstructionPtr BuildMode::getContruction() const{    return _d->construction;}

DestroyMode::DestroyMode() : LayerMode( citylayer::destroy )
{
}

Renderer::ModePtr DestroyMode::create()
{
  Renderer::ModePtr ret( new DestroyMode() );
  ret->drop();

  return ret;
}

Renderer::ModePtr BuildMode::create(TileOverlay::Type type )
{
  BuildMode* newCommand = new BuildMode();
  TileOverlayPtr overlay = TileOverlayFactory::getInstance().create( type );
  newCommand->_d->construction = ptr_cast<Construction>( overlay );
  newCommand->_d->isMultiBuilding = false;
  newCommand->_d->isBorderBuilding = false;
  newCommand->_d->isRoadAssignment = false;

  switch( type )
  {
  case construction::road:
    newCommand->_d->isBorderBuilding = true;
    newCommand->_d->isMultiBuilding = true;
    newCommand->_d->isRoadAssignment = false;
  break;

  case building::aqueduct:
  case building::wall:
  case building::fortification:
    newCommand->_d->isBorderBuilding = true;
    newCommand->_d->isMultiBuilding = true;
  break;

  case building::house:
  case construction::garden:
    newCommand->_d->isMultiBuilding = true;
  break;

  default:
  break;
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
