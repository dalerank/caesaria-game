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

    if( !overlay  )
    {
        const TerrainTile& terrain = tile->get_terrain();
        infoBox = new InfoBoxLand( _d->gui->getRootWidget(), tile );
    }
    
    if( infoBox  )
    {
        infoBox->setPosition( Point( (_d->gui->getRootWidget()->getWidth() - infoBox->getWidth()) / 2, 30) );
        //_screenGame->setInfoBox(infoBox);
    }
}