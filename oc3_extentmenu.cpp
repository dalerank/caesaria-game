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


#include "oc3_extentmenu.h"

#include "sdl_facade.hpp"
#include "pic_loader.hpp"
#include "oc3_resourcegroup.h"
#include "oc3_positioni.h"
#include "oc3_pushbutton.h"
#include <iostream>

static const Uint32 minimizeBtnPicId = 97;
static const Uint32 bgPicId = 17;
static const Uint32 bottomPicId = 20;

class ExtentMenu::Impl
{
public:
    Picture* bgPicture;
    PushButton* btnMinimize;
};

static void configureButton( PushButton& oButton, const int pic_index, const Point& pos )
{
    PicLoader& loader = PicLoader::instance();

    oButton.setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index), stNormal );
    oButton.setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index+1), stHovered );
    oButton.setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index+2), stPressed );
    oButton.setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index+3), stDisabled );

    oButton.setPosition( pos );
}


ExtentMenu::ExtentMenu( Widget* parent, const Rect& rect, int id ) : Widget( parent, id, rect ), _d( new Impl )
{
    SdlFacade &sdlFacade = SdlFacade::instance();

    const Picture& bground = PicLoader::instance().get_picture( ResourceGroup::panelBackground, bgPicId );
    const Picture& bottom = PicLoader::instance().get_picture( ResourceGroup::panelBackground, bottomPicId );

    _d->bgPicture = &sdlFacade.createPicture( bground.get_width(), bground.get_height() + bottom.get_height() );
    sdlFacade.drawPicture( bground, *_d->bgPicture, 0, 0);
    sdlFacade.drawPicture( bottom, *_d->bgPicture, 0, bground.get_height() );

    setWidth( _d->bgPicture->get_width() );
    setHeight( _d->bgPicture->get_height() );

    Point offset( 1, 32 );
    int dy = 35;
    configureButton( *_d->btnMinimize, minimizeBtnPicId, Point( 127, 5 ) );
    _d->btnMinimize->onClicked().connect( this, &ExtentMenu::minimize );

    // //
    // _midIcon.setPicture(PicLoader::instance().get_picture("panelwindows", 1));
    // _midIcon.setPosition(8, 217);
    // add_widget(_midIcon);

    // // header
    // set3Button(_senateButton, WidgetEvent(), 79);
    // set3Button(_empireButton, WidgetEvent(), 82);
    // set3Button(_missionButton, WidgetEvent(), 85);
    // set3Button(_northButton, WidgetEvent(), 88);
    // set3Button(_rotateLeftButton, WidgetEvent(), 91);
    // set3Button(_rotateRightButton, WidgetEvent(), 94);

    /*set4Button(_houseButton, WidgetEvent::BuildingEvent(B_HOUSE), 123);
    _houseButton.setPosition( offset + Point( 0, dy * 0 ) );

    set4Button(_clearButton, WidgetEvent::ClearLandEvent(), 131);
    _clearButton.setPosition( offset + Point( 0, dy * 1 ) );

    set4Button(_roadButton, WidgetEvent::BuildingEvent(B_ROAD), 135);
    _roadButton.setPosition( offset + Point( 0, dy * 2 ) );
    // second row
    set4Button(_waterButton, WidgetEvent::BuildMenuEvent(BM_WATER), 127);
    _waterButton.setPosition( offset + Point( 0, dy * 3 ));

    set4Button(_healthButton, WidgetEvent::BuildMenuEvent(BM_HEALTH), 163);
    _healthButton.setPosition( offset + Point( 0, dy * 4 ) );

    set4Button(_templeButton, WidgetEvent::BuildMenuEvent(BM_RELIGION), 151);
    _templeButton.setPosition( offset + Point( 0, dy * 5 ) );
    // third row
    set4Button(_educationButton, WidgetEvent::BuildMenuEvent(BM_EDUCATION), 147);
    _educationButton.setPosition( offset + Point( 0, dy * 6 ) );

    set4Button(_entertainmentButton, WidgetEvent::BuildMenuEvent(BM_ENTERTAINMENT), 143);
    _entertainmentButton.setPosition( offset + Point( 0, dy * 7 ) );

    set4Button(_administrationButton, WidgetEvent::BuildMenuEvent(BM_ADMINISTRATION), 139);
    _administrationButton.setPosition( offset + Point( 0, dy * 8 ) );
    // 4th row
    set4Button(_engineerButton, WidgetEvent::BuildMenuEvent(BM_ENGINEERING), 167);
    _engineerButton.setPosition( offset + Point( 0, dy * 9 ) );

    set4Button(_securityButton, WidgetEvent::BuildMenuEvent(BM_SECURITY), 159);
    _securityButton.setPosition( offset + Point( 0, dy * 10 ) );

    set4Button(_commerceButton, WidgetEvent::BuildMenuEvent(BM_COMMERCE), 155);
    _commerceButton.setPosition( offset + Point( 0, dy * 11 ) );
    */

    //   // 5th row
    //   set4Button(_cancelButton, WidgetEvent(), 171);
    //   set4Button(_messageButton, WidgetEvent(), 115);
    //   set4Button(_disasterButton, WidgetEvent(), 119);

    // set button position
    // _menuButton.setPosition(4, 3);
   
    // header
    // _senateButton.setPosition(7, 155);
    // _empireButton.setPosition(84, 155);
    // _missionButton.setPosition(7, 184);
    // _northButton.setPosition(46, 184);
    // _rotateLeftButton.setPosition(84, 184);
    // _rotateRightButton.setPosition(123, 184);



    //   // 5th row
    //   _cancelButton.setPosition(x0, y0+dy*12);
    //   _messageButton.setPosition(x0, y0+dy*13);
    //   _disasterButton.setPosition(x0, y0+dy*14);

    //   _bgPicture = &PicLoader::instance().get_picture("paneling", 17);
    //   setSize(_bgPicture->get_surface()->w, _bgPicture->get_surface()->h);
    //
    //   // top of menu
    //   _menuButton.setText("Menu");
    //   _menuButton.setEvent(WidgetEvent::InGameMenuEvent());
    //   _menuButton.setNormalPicture(PicLoader::instance().get_picture("paneling", 234));
    //   _menuButton.setHoverPicture(PicLoader::instance().get_picture("paneling", 234+1));
    //   _menuButton.setSelectedPicture(PicLoader::instance().get_picture("paneling", 234+2));
    //   _menuButton.setUnavailablePicture(PicLoader::instance().get_picture("paneling", 234+3));
    //   _menuButton.init_pictures();
    //   add_widget(_menuButton);
    //
    //   set4Button(_minimizeButton, WidgetEvent(), 97);
    //
    //   //
    //   _midIcon.setPicture(PicLoader::instance().get_picture("panelwindows", 1));
    //   _midIcon.setPosition(8, 217);
    //   add_widget(_midIcon);
    //
    //   //_bottomIcon.setPicture(PicLoader::instance().get_picture("paneling", 20));
    //   //_bottomIcon.setPosition(0, _bgPicture->get_surface()->h);
    //   //add_widget(_bottomIcon);
    //
    //   // header
    //   set3Button(_senateButton, WidgetEvent(), 79);
    //   set3Button(_empireButton, WidgetEvent(), 82);
    //   set3Button(_missionButton, WidgetEvent(), 85);
    //   set3Button(_northButton, WidgetEvent(), 88);
    //   set3Button(_rotateLeftButton, WidgetEvent(), 91);
    //   set3Button(_rotateRightButton, WidgetEvent(), 94);
    //
    //   set4Button(_houseButton, WidgetEvent::BuildingEvent(B_HOUSE), 123);
    //   set4Button(_clearButton, WidgetEvent::ClearLandEvent(), 131);
    //   set4Button(_roadButton, WidgetEvent::BuildingEvent(B_ROAD), 135);
    //   // second row
    //   set4Button(_waterButton, WidgetEvent::BuildMenuEvent(BM_WATER), 127);
    //   set4Button(_healthButton, WidgetEvent::BuildMenuEvent(BM_HEALTH), 163);
    //   set4Button(_templeButton, WidgetEvent::BuildMenuEvent(BM_TEMPLE), 151);
    //   // third row
    //   set4Button(_educationButton, WidgetEvent::BuildMenuEvent(BM_EDUCATION), 147);
    //   set4Button(_entertainmentButton, WidgetEvent::BuildMenuEvent(BM_ENTERTAINMENT), 143);
    //   set4Button(_administrationButton, WidgetEvent::BuildMenuEvent(BM_ADMINISTRATION), 139);
    //   // 4th row
    //   set4Button(_engineerButton, WidgetEvent::BuildMenuEvent(BM_ENGINEERING), 167);
    //   set4Button(_securityButton, WidgetEvent::BuildMenuEvent(BM_SECURITY), 159);
    //   set4Button(_commerceButton, WidgetEvent::BuildMenuEvent(BM_COMMERCE), 155);
    //   // 5th row
    //   set4Button(_cancelButton, WidgetEvent(), 171);
    //   set4Button(_messageButton, WidgetEvent(), 115);
    //   set4Button(_disasterButton, WidgetEvent(), 119);
    //
    //   // set button position
    //   _menuButton.setPosition(4, 3);
    //   _minimizeButton.setPosition(127, 5);
    //   // header
    //   _senateButton.setPosition(7, 155);
    //   _empireButton.setPosition(84, 155);
    //   _missionButton.setPosition(7, 184);
    //   _northButton.setPosition(46, 184);
    //   _rotateLeftButton.setPosition(84, 184);
    //   _rotateRightButton.setPosition(123, 184);
    //   // first row
    //   _houseButton.setPosition(13, 277);
    //   _clearButton.setPosition(63, 277);
    //   _roadButton.setPosition(113, 277);
    //   // second row
    //   _waterButton.setPosition(13, 313);
    //   _healthButton.setPosition(63, 313);
    //   _templeButton.setPosition(113, 313);
    //   // third row
    //   _educationButton.setPosition(13, 349);
    //   _entertainmentButton.setPosition(63, 349);
    //   _administrationButton.setPosition(113, 349);
    //   // 4th row
    //   _engineerButton.setPosition(13, 385);
    //   _securityButton.setPosition(63, 385);
    //   _commerceButton.setPosition(113, 385);
    //   // 5th row
    //   _cancelButton.setPosition(13, 421);
    //   _messageButton.setPosition(63, 421);
    //   _disasterButton.setPosition(113, 421);
}

void ExtentMenu::draw( GfxEngine& engine )
{
    if( !isVisible() )
        return;

    engine.drawPicture( getBgPicture(), getScreenLeft(), getScreenTop() );
    Widget::draw( engine );
}

ExtentMenuPtr ExtentMenu::create( Widget* parent, const Rect& rect, int id )
{
    ExtentMenuPtr ret( new ExtentMenu( parent, rect, id ) );
    return ret;
}

void ExtentMenu::minimize()
{
    std::cout << "ExtentMenu::minimize called" << std::endl;
}

const Picture& ExtentMenu::getBgPicture() const
{
    return *_d->bgPicture;
}