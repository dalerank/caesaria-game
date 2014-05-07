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

#include "build.hpp"
#include "objects/objects_factory.hpp"
#include "game/game.hpp"
#include "city/helper.hpp"
#include "city/funds.hpp"
#include "playsound.hpp"
#include "city/statistic.hpp"
#include "warningmessage.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

GameEventPtr BuildEvent::create( const TilePos& pos, const TileOverlay::Type type )
{
  return create( pos, TileOverlayFactory::instance().create( type ) );
}

GameEventPtr BuildEvent::create(const TilePos& pos, TileOverlayPtr overlay)
{
  BuildEvent* ev = new BuildEvent();
  ev->_pos = pos;
  ev->_overlay = overlay;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

bool BuildEvent::_mayExec(Game&, unsigned int) const {  return true;}

void BuildEvent::_exec( Game& game, unsigned int )
{  
  if( _overlay.isNull() )
    return;

  TileOverlayPtr ctOv = game.city()->getOverlay( _pos );

  bool mayBuild = true;
  if( ctOv.isValid() )
  {
    mayBuild = ctOv->isDestructible();
  }

  if( !_overlay->isDeleted() && mayBuild )
  {
      _overlay->build( game.city(), _pos );
      city::Helper helper( game.city() );
      helper.updateDesirability( _overlay, true );
      game.city()->addOverlay( _overlay );

      ConstructionPtr construction = ptr_cast<Construction>( _overlay );
      if( construction.isValid() )
      {
        const MetaData& buildingData = MetaDataHolder::getData( _overlay->type() );
        game.city()->funds().resolveIssue( FundIssue( city::Funds::buildConstruction,
                                                      -(int)buildingData.getOption( "cost" ) ) );

        GameEventPtr e = PlaySound::create( "buildok", 1, 100 );
        e->dispatch();

        if( construction->isNeedRoadAccess() && construction->getAccessRoads().empty() )
        {
          GameEventPtr e = WarningMessageEvent::create( "##building_need_road_access##" );
          e->dispatch();
        }

        std::string error = construction->errorDesc();
        if( !error.empty() )
        {
          GameEventPtr e = WarningMessageEvent::create( error );
          e->dispatch();
        }

        WorkingBuildingPtr wb = ptr_cast<WorkingBuilding>( construction );
        if( wb.isValid() && wb->maxWorkers() > 0 )
        {
          int worklessCount = city::Statistic::getWorklessNumber( game.city() );
          if( worklessCount < wb->maxWorkers() )
          {
            GameEventPtr e = WarningMessageEvent::create( "##city_need_more_workers##" );
            e->dispatch();
          }
        }
      }      
    }
    else
    {
      ConstructionPtr construction = ptr_cast<Construction>( _overlay );
      if( construction.isValid() )
      {
        GameEventPtr e = WarningMessageEvent::create( construction->errorDesc() );
        e->dispatch();
      }
    }
}

} //end namespace events
