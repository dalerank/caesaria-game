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

#include "oc3_tilemapchangecommand.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_building.hpp"

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

TilemapChangeCommandPtr TilemapBuildCommand::create( BuildingType type )
{
  TilemapBuildCommand* newCommand = new TilemapBuildCommand();
  ConstructionPtr construction = ConstructionManager::getInstance().create( type );
  newCommand->_d->construction = construction;
  newCommand->_d->isMultiBuilding = false;
  newCommand->_d->isBorderBuilding = false;
  newCommand->_d->canBuild = false;

  switch( type )
  {
  case B_ROAD:
  case B_AQUEDUCT:
    newCommand->_d->isBorderBuilding = true;
    newCommand->_d->isMultiBuilding = true;
    break;

  case B_HOUSE:
    newCommand->_d->isMultiBuilding = true;
    //break not needed that catch multibuild flag
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
  OverlayType type;
};

TilemapChangeCommandPtr TilemapOverlayCommand::create( const OverlayType type )
{
  TilemapOverlayCommand* newCommand = new TilemapOverlayCommand();
  newCommand->_d->type = type;

  TilemapChangeCommandPtr ret( newCommand );
  ret->drop();

  return ret;
}

TilemapOverlayCommand::TilemapOverlayCommand() : _d( new Impl )
{
  _d->type = OV_NOTHING;
}

OverlayType TilemapOverlayCommand::getType() const
{
  return _d->type;
}