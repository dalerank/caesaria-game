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


#include "rightpanel.hpp"
#include "gfx/picture.hpp"
#include "gfx/engine.hpp"

namespace gui
{

class MenuRigthPanel::Impl
{
public:
    PictureRef picture; 
};

MenuRigthPanel::MenuRigthPanel( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 100, 100 ) ), _d( new Impl )
{
}

void MenuRigthPanel::draw( GfxEngine& engine )
{
    engine.drawPicture( *_d->picture, screenLeft(), screenTop() );
}

MenuRigthPanel* MenuRigthPanel::create( Widget* parent, const Rect& rectangle, const Picture& tilePic )
{
    MenuRigthPanel* ret = new MenuRigthPanel( parent );

    ret->setGeometry( rectangle );
    
    ret->_d->picture.reset( Picture::create( rectangle.getSize() ) );
    //SDL_SetAlpha( ret->_d->picture->getSurface(), 0, 0 );  // remove surface alpha

    int y = 0;
    while( y <  ret->_d->picture->getHeight() )
    {
        ret->_d->picture->draw( tilePic, Point( 0, y ) );
        y += tilePic.getHeight();
    }

    return ret;
}

}//end namespace gui
