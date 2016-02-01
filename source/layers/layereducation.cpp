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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "layereducation.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "constants.hpp"
#include "gfx/tilemap_camera.hpp"
#include "city/statistic.hpp"
#include "core/color_list.hpp"
#include "core/event.hpp"
#include "game/gamedate.hpp"
#include "gfx/textured_path.hpp"
#include "objects/education.hpp"
#include "core/priorities.hpp"
#include "core/gettext.hpp"

using namespace gfx;

namespace citylayer
{

class Education::Impl
{
public:
  struct
  {
    OverlayPtr selected;
    OverlayPtr underMouse;
  } overlay;

  DateTime lastUpdate;
  std::vector<ColoredWay> ways;
  std::set<object::Type> flags;
  int type;
};

int Education::type() const {  return _d->type; }

int Education::_getLevelValue( HousePtr house ) const
{
  switch(_d->type)
  {
  case citylayer::education:
  {
    float acc = 0;
    int level = house->spec().minEducationLevel();
    switch(level)
    {
    case 3: acc += house->getServiceValue( Service::academy );
    case 2: acc += house->getServiceValue( Service::library );
    case 1: acc += house->getServiceValue( Service::school );
      return static_cast<int>(acc / level);
    default: return 0;
    }
  }

  case citylayer::school: return (int) house->getServiceValue( Service::school );
  case citylayer::library: return (int) house->getServiceValue( Service::library );
  case citylayer::academy: return (int) house->getServiceValue( Service::academy );
  }

  return 0;
}

void Education::drawTile(const RenderInfo& rinfo, Tile& tile)
{
  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();

    int educationLevel = -1;
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;
    }
    else if( _d->flags.count( overlay->type() ) > 0 )
    {
      needDrawAnimations = true;
      //city::Helper helper( _city() );
      //drawArea( engine, helper.getArea( overlay ), offset, ResourceGroup::foodOverlay, OverlayPic::base );
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();

      educationLevel = _getLevelValue( house );

      needDrawAnimations = (house->spec().level() <= HouseLevel::hovel) && (house->habitants().empty());

      drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.house );
    }
    else
    {
      //other buildings
      drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.constr );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( educationLevel > 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, educationLevel );
    }
  }

  tile.setRendered();
}

std::string Education::_getAccessLevel( int lvlValue ) const
{
  static const std::vector<std::string> accesDesc = { "##no_", "##warning_",
                                                      "##bad_", "##simple_",
                                                      "##good_", "##awesome_" };
  float limiter = 100.f / accesDesc.size();
  return accesDesc[ math::clamp<int>( ceil( lvlValue / limiter ), 0, accesDesc.size()-1 ) ];
}

void Education::afterRender(Engine& engine)
{
  Info::afterRender(engine);

  if( game::Date::isDayChanged() )
    _updatePaths();
}

void Education::_updatePaths()
{
  auto eduBuilding = _d->overlay.selected.as<EducationBuilding>();
  if( _d->flags.count( object::typeOrDefault( eduBuilding ) ) )
  {
    _d->ways.clear();
    const WalkerList& walkers = eduBuilding->walkers();
    for( auto walker : walkers )
      _d->ways.push_back( ColoredWay{ walker->pathway().allTiles(), ColorList::red, Point( -2, 2 ) } );
  }
}

void Education::render(Engine& engine)
{
  Info::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  for( auto& item : _d->ways )
    TexturedPath::draw( item.tiles, rinfo, item.color, item.offset );
}

void Education::onEvent( const NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case NEvent::Mouse::moved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      std::string levelName = "";      
      if( tile != 0 )
      {
        auto house = tile->overlay<House>();
        if( house != 0 )
        {
          std::string typeName;
          int lvlValue = _getLevelValue( house );
          switch( _d->type )
          {
          case citylayer::education:
          {
            bool schoolAccess = house->hasServiceAccess( Service::school );
            bool libraryAccess = house->hasServiceAccess( Service::library );
            bool academyAccess = house->hasServiceAccess( Service::academy );

            if( schoolAccess && libraryAccess && academyAccess )
            {
              text = "##education_full_access##";
            }
            else
            {
              if( schoolAccess && libraryAccess ) { text = "##education_have_school_library_access##"; }
              else if( schoolAccess || libraryAccess ) { text = "##education_have_school_or_library_access##"; }
              else if( academyAccess ) { text = "##education_have_academy_access##"; }
              else { text = "##education_have_no_access##"; }
            }
          }
          break;
          case citylayer::school: typeName = "school";  break;
          case citylayer::library: typeName = "library"; break;
          case citylayer::academy: typeName = "academy"; break;
          }       

          if( text.empty() )
          {
            levelName = _getAccessLevel( lvlValue );
            text = levelName + typeName + "_access##";
          }
        }

        _d->overlay.underMouse = tile->overlay();
      }

      _setTooltipText( _(text) );
    }
    break;

    case NEvent::Mouse::btnLeftPressed:
    {
      if( _d->overlay.underMouse.is<EducationBuilding>() )
      {
        _d->overlay.selected = _d->overlay.underMouse;
        _updatePaths();
      }
    }
    break;

    default: break;
    }
  }

  Layer::onEvent( event );
}

Education::Education( Camera& camera, PlayerCityPtr city, int type)
  : Info( camera, city, 9 ), _d( new Impl )
{
  _d->type = type;

  switch( type )
  {
  case citylayer::education:
    _d->flags << object::school << object::library << object::academy;
    _visibleWalkers() << walker::scholar << walker::librarian << walker::teacher;
  break;

  case citylayer::school: _d->flags << object::school; _visibleWalkers() << walker::scholar; break;
  case citylayer::library: _d->flags << object::library; _visibleWalkers() << walker::librarian; break;
  case citylayer::academy: _d->flags << object::academy; _visibleWalkers() << walker::teacher; break;
  }

  _initialize();
}

}//end namespace citylayer
