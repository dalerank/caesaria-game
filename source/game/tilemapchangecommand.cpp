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

#include "tilemapchangecommand.hpp"
#include "tileoverlay_factory.hpp"
#include "building/building.hpp"
#include "building/constants.hpp"

using namespace constants;

class TilemapBuildCommand::Impl
{
public:
    bool isBorderBuilding;
    bool isMultiBuilding;
    ConstructionPtr construction;
    bool canBuild;
};

TilemapChangeCommand::~TilemapChangeCommand()
{

}

bool TilemapBuildCommand::isBorderBuilding() const
{
    return _d->isBorderBuilding;
}

bool TilemapBuildCommand::isMultiBuilding() const
{
    return _d->isMultiBuilding;
}

ConstructionPtr TilemapBuildCommand::getContruction() const
{
    return _d->construction;
}

TilemapRemoveCommand::TilemapRemoveCommand()
{
}

TilemapChangeCommandPtr TilemapRemoveCommand::create()
{
  TilemapChangeCommandPtr ret( new TilemapRemoveCommand() );
  ret->drop();

  return ret;
}

TilemapChangeCommandPtr TilemapBuildCommand::create(TileOverlay::Type type )
{
  TilemapBuildCommand* newCommand = new TilemapBuildCommand();
  TileOverlayPtr overlay = TileOverlayFactory::getInstance().create( type );
  newCommand->_d->construction = overlay.as<Construction>();
  newCommand->_d->isMultiBuilding = false;
  newCommand->_d->isBorderBuilding = false;
  newCommand->_d->canBuild = false;

  switch( type )
  {
  case construction::B_ROAD:
  case building::B_AQUEDUCT:
    newCommand->_d->isBorderBuilding = true;
    newCommand->_d->isMultiBuilding = true;
    break;

  case building::house:
  case construction::B_GARDEN:
    newCommand->_d->isMultiBuilding = true;
  break;

  default:
    break;    
  }   

  TilemapChangeCommandPtr ret( newCommand );
  ret->drop();

  return ret;
}

TilemapBuildCommand::TilemapBuildCommand() : _d( new Impl )
{

}

void TilemapBuildCommand::setCanBuild( bool cb )
{
  _d->canBuild = cb;
}

bool TilemapBuildCommand::isCanBuild() const
{
  return _d->canBuild;
}

class TilemapOverlayCommand::Impl
{
public:
  DrawingOverlayType type;
};

TilemapChangeCommandPtr TilemapOverlayCommand::create( const DrawingOverlayType type )
{
  TilemapOverlayCommand* newCommand = new TilemapOverlayCommand();
  newCommand->_d->type = type;

  TilemapChangeCommandPtr ret( newCommand );
  ret->drop();

  return ret;
}

TilemapOverlayCommand::TilemapOverlayCommand() : _d( new Impl )
{
  _d->type = drwSimple;
}

DrawingOverlayType TilemapOverlayCommand::getType() const
{
  return _d->type;
}
