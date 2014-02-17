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

using namespace constants;

namespace events
{

GameEventPtr BuildEvent::create( const TilePos& pos, const TileOverlay::Type type )
{
  return create( pos, TileOverlayFactory::getInstance().create( type ) );
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

void BuildEvent::exec( Game& game )
{
  const MetaData& buildingData = MetaDataHolder::instance().getData( _overlay->getType() );
  if( _overlay.isValid() )
  {
    _overlay->build( game.getCity(), _pos );

    if( !_overlay->isDeleted() )
    {
      ConstructionPtr construction = ptr_cast<Construction>( _overlay );
      if( construction.isValid() )
      {
        CityHelper helper( game.getCity() );
        helper.updateDesirability( construction, true );

        game.getCity()->addOverlay( _overlay );
        game.getCity()->getFunds().resolveIssue( FundIssue( CityFunds::buildConstruction,
                                                            -(int)buildingData.getOption( "cost" ) ) );

        GameEventPtr e = PlaySound::create( "buildok", 1, 256 );
        e->dispatch();

        if( construction->isNeedRoadAccess() && construction->getAccessRoads().empty() )
        {
          GameEventPtr e = WarningMessageEvent::create( "##building_need_road_access##" );
          e->dispatch();
        }

        std::string error = construction->getError();
        if( !error.empty() )
        {
          GameEventPtr e = WarningMessageEvent::create( error );
          e->dispatch();
        }

        WorkingBuildingPtr wb = ptr_cast<WorkingBuilding>( construction );
        if( wb.isValid() && wb->getMaxWorkers() > 0 )
        {
          int worklessCount = CityStatistic::getWorklessNumber( game.getCity() );
          if( worklessCount < wb->getMaxWorkers() )
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
        GameEventPtr e = WarningMessageEvent::create( construction->getError() );
        e->dispatch();
      }
    }
  }
}

} //end namespace events
