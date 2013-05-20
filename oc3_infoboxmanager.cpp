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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#include "oc3_infoboxmanager.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_guienv.hpp"
#include "oc3_road.hpp"
#include "oc3_buildingprefect.hpp"
#include "oc3_tile.hpp"
#include "oc3_service_building.hpp"
#include "oc3_stringhelper.hpp"

class InfoBoxManager::Impl
{
public:
    GuiEnv* gui;
    GuiInfoBox* infoBox;
    bool showDebugInfo;
};

InfoBoxManagerPtr InfoBoxManager::create( GuiEnv* gui )
{
    InfoBoxManagerPtr ret( new InfoBoxManager() );
    ret->_d->gui = gui;
    ret->_d->showDebugInfo = true;

    return ret;
}

InfoBoxManager::InfoBoxManager() : _d( new Impl )
{

}

InfoBoxManager::~InfoBoxManager()
{

}

void InfoBoxManager::showHelp( Tile* tile )
{
    LandOverlayPtr overlay = tile->get_terrain().getOverlay();
    GuiInfoBox* infoBox = 0;

    if( _d->showDebugInfo )
    {
      StringHelper::debug( 0xff, "Tile debug info: dsrbl=%d", tile->get_terrain().getDesirability() ); 
    }

    if( overlay.isNull() )
    {
        const TerrainTile& terrain = tile->get_terrain();
        infoBox = new InfoBoxLand( _d->gui->getRootWidget(), tile );
    }
    else
    {
      RoadPtr road  = overlay.as<Road>();
      if( road.isValid() )
      {
        infoBox = new InfoBoxLand( _d->gui->getRootWidget(), tile );    
      }
      
      HousePtr house = overlay.as<House>();
      if( house.isValid() )
      {
        if( house->getNbHabitants() > 0 )
        {
          infoBox = new InfoBoxHouse( _d->gui->getRootWidget(), house );
        }
        else
        {
          infoBox = new InfoBoxFreeHouse( _d->gui->getRootWidget(), tile );
        }
      }

      BuildingPrefectPtr prefecture = overlay.as<BuildingPrefect>();
      if( prefecture.isValid() )
      {
        Size  size = _d->gui->getRootWidget()->getSize();
        infoBox = new GuiInfoService( _d->gui->getRootWidget(), prefecture.as<ServiceBuilding>() );
        infoBox->setPosition( Point( (size.getWidth() - infoBox->getWidth()) / 2, 
                                      size.getHeight() - infoBox->getHeight()) );

        infoBox->setTitle( "##engineering_post_title##");
        infoBox->setText( "##engineering_post_text##");
        return;
      }
    }
    
    if( infoBox  )
    {
        Point pos( 156, ( _d->gui->getCursorPos().getY() < _d->gui->getRootWidget()->getHeight() / 2 ) ? 407 : 30);

        infoBox->setPosition( pos );
        //_screenGame->setInfoBox(infoBox);
    }
}

void InfoBoxManager::setShowDebugInfo( const bool showInfo )
{
  _d->showDebugInfo = showInfo;
} 