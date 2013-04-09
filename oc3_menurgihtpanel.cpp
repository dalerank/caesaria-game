#include "oc3_menurgihtpanel.h"
#include "sdl_facade.hpp"
#include "picture.hpp"
#include "gfx_engine.hpp"

class MenuRigthPanel::Impl
{
public:
    Picture* picture; 
};

MenuRigthPanel::MenuRigthPanel( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 100, 100 ) ), _d( new Impl )
{
}

void MenuRigthPanel::draw( GfxEngine& engine )
{
    engine.drawPicture( *_d->picture, getScreenLeft(), getScreenTop() );
}

MenuRigthPanelPtr MenuRigthPanel::create( Widget* parent, const Rect& rectangle, const Picture& tilePic )
{
    MenuRigthPanelPtr ret( new MenuRigthPanel( parent ) );

    ret->setGeometry( rectangle );
    SdlFacade &sdlFacade = SdlFacade::instance();
    
    ret->_d->picture = &sdlFacade.createPicture( rectangle.getWidth(), rectangle.getHeight() );
    SDL_SetAlpha( ret->_d->picture->get_surface(), 0, 0 );  // remove surface alpha

    int y = 0;
    while( y <  ret->_d->picture->get_height() )
    {
        sdlFacade.drawPicture( tilePic, *ret->_d->picture, 0, y);
        y += tilePic.get_height();
    }

    return ret;
}