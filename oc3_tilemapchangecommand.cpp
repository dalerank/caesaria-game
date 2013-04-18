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

class TilemapChangeCommand::Impl
{
public:
    bool isValid;
    bool isRemoveTool;  // true when using "clear land" tool
    bool isBorderBuilding;
    bool isMultiBuilding;
    Construction* construction;
};

TilemapChangeCommand::~TilemapChangeCommand()
{

}

TilemapChangeCommand::TilemapChangeCommand( BuildingType type )
: _d( new Impl )
{
    Construction* construction = dynamic_cast<Construction*>( ConstructionManager::getInstance().create( type ) );
    _d->construction = construction;
    _d->isMultiBuilding = false;
    _d->isRemoveTool = false;
    _d->isValid = true;
    _d->isBorderBuilding = false;

    switch( type )
    {
    case B_ROAD:
    case B_AQUEDUCT:
        _d->isBorderBuilding = true;
        _d->isMultiBuilding = true;
    break;

    case B_HOUSE:
        _d->isMultiBuilding = true;
        //break not needed that catch multibuild flag
    break;

    default:
    break;    
    }   
}

TilemapChangeCommand::TilemapChangeCommand() : _d( new Impl )
{
    _d->isValid = false;
    _d->isRemoveTool = false;  // true when using "clear land" tool
    _d->isBorderBuilding = false;
    _d->isMultiBuilding = false;
    _d->construction = 0;
}

TilemapChangeCommand& TilemapChangeCommand::operator=( const TilemapChangeCommand& other )
{
    _d->isValid = other._d->isValid;
    _d->isRemoveTool = other._d->isRemoveTool;  // true when using "clear land" tool
    _d->isBorderBuilding = other._d->isBorderBuilding;
    _d->isMultiBuilding = other._d->isMultiBuilding;
    _d->construction = other._d->construction;

    return *this;
}

bool TilemapChangeCommand::isRemoveTool() const
{
    return _d->isRemoveTool;
}

bool TilemapChangeCommand::isValid() const
{
    return _d->isValid;
}

bool TilemapChangeCommand::isBorderBuilding() const
{
    return _d->isBorderBuilding;
}

bool TilemapChangeCommand::isMultiBuilding() const
{
    return _d->isMultiBuilding;
}

Construction* TilemapChangeCommand::getContruction() const
{
    return _d->construction;
}

TilemapRemoveCommand::TilemapRemoveCommand()
    : TilemapChangeCommand()
{
    _d->isValid = true;
    _d->isRemoveTool = true;
    _d->isMultiBuilding = true;
}