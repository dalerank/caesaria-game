#include "oc3_infoboxmanager.h"
#include "gui_info_box.hpp"
#include "oc3_guienv.h"

class InfoBoxManager::Impl
{
public:
    GuiEnv* gui;
    GuiInfoBox* infoBox;
};

InfoBoxManagerPtr InfoBoxManager::create( GuiEnv* gui )
{
    InfoBoxManagerPtr ret( new InfoBoxManager() );
    ret->_d->gui = gui;

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
    }
    
    if( infoBox  )
    {
        Point pos( 156, ( _d->gui->getCursorPos().getY() < _d->gui->getRootWidget()->getHeight() / 2 ) ? 407 : 30);

        infoBox->setPosition( pos );
        //_screenGame->setInfoBox(infoBox);
    }
}