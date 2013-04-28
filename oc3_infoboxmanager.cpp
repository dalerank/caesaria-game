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
#include <iostream>

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
    LandOverlay* overlay = tile->get_terrain().getOverlay();
    GuiInfoBox* infoBox = 0;

    if( _d->showDebugInfo )
    {
      std::cout << "Tile debug info: dsrbl=" << tile->get_terrain().getDesirability() << std::endl; 
    }

    if( !overlay )
    {
        const TerrainTile& terrain = tile->get_terrain();
        infoBox = new InfoBoxLand( _d->gui->getRootWidget(), tile );
    }
    else
    {
        if( Road* road = safety_cast< Road* >( overlay ) )
        {
            infoBox = new InfoBoxLand( _d->gui->getRootWidget(), tile );
        }
        else if( House* house = safety_cast< House* >( overlay ) )
        {
            if( house->getNbHabitants() > 0 )
                infoBox = new InfoBoxHouse( _d->gui->getRootWidget(), *house );
            else
            {
                infoBox = new InfoBoxFreeHouse( _d->gui->getRootWidget(), tile );
            }
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