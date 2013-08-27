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

#include "oc3_advisor_religion_window.hpp"
#include "oc3_picture.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_gettext.hpp"
#include "oc3_enums.hpp"
#include "oc3_city.hpp"
#include "oc3_building_house.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_divinity.hpp"

class ReligionInfoLabel : public Label
{
public:
  ReligionInfoLabel( Widget* parent, const Rect& rect, RomeDivinityPtr divinity, 
                     int smallTempleCount, int bigTempleCount  )
    : Label( parent, rect )
  {
    _divinity = divinity;
    _smallTempleCount = smallTempleCount;
    _bigTempleCount = bigTempleCount;
    _lastFestival = 99;
    _mood = 0;

    setFont( Font::create( FONT_1_WHITE ) );
  }

  virtual void _updateTexture( GfxEngine& painter )
  {
    Label::_updateTexture( painter );

    PictureRef& texture = getTextPicture();
    Font font = getFont();

    if( _divinity.isValid() )
    {
      font.draw( *texture, _divinity->getName(), 0, 0 );
      Font fontBlack = Font::create( FONT_1 );
      fontBlack.draw( *texture, StringHelper::format( 0xff, "(%s)", _divinity->getShortDescription().c_str() ), 80, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _bigTempleCount ), 280, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _lastFestival ), 350, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _mood ), 420, 0 );
    }
    else
    {
      font.draw( *texture, _("##Oracles_in_city##"), 0, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
    }
  }

private:
  RomeDivinityPtr _divinity;
  int _smallTempleCount;
  int _bigTempleCount;
  int _lastFestival;
  int _mood;
};

class AdvisorReligionWindow::Impl
{
public:
  PictureRef background;

  ReligionInfoLabel* lbCeresInfo;
  ReligionInfoLabel* lbNeptuneInfo;
  ReligionInfoLabel* lbMercuryInfo;
  ReligionInfoLabel* lbMarsInfo;
  ReligionInfoLabel* lbVenusInfo;
  ReligionInfoLabel* lbOracleInfo;
  TexturedButton* btnHelp;

  struct InfrastructureInfo
  {
    int smallTemplCount;
    int bigTempleCount;
  };

  InfrastructureInfo getInfo( CityPtr city, const BuildingType small, const BuildingType big )
  {
    CityHelper helper( city );

    InfrastructureInfo ret;

    ret.smallTemplCount = 0;
    ret.bigTempleCount = 0;

    std::list< ServiceBuildingPtr > smallTemples = helper.getBuildings<ServiceBuilding>( small );
   /*
     for( std::list< ServiceBuildingPtr >::iterator it=buildings.begin(); it != buildings.end(); it++ )
        {
          if( (*it)->getWorkers() > 0 )
          {
            ret.buildingWork++;
          }
        }*/
    

    ret.smallTemplCount = smallTemples.size();

    std::list< ServiceBuildingPtr > bigTemples = helper.getBuildings<ServiceBuilding>( big );
    ret.bigTempleCount = bigTemples.size();

    return ret;
  }
};


AdvisorReligionWindow::AdvisorReligionWindow( CityPtr city, Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 280 ) ) );

  Label* title = new Label( this, Rect( 60, 10, 60 + 210, 10 + 40) );
  title->setText( _("##Religion advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );
  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( 35, 62, getWidth() - 35, 62 + 130 ), PictureDecorator::blackFrame );

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 264 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##Temples##"), 268, 32, false );
  font.draw( *_d->background, _("##small##"), 240, 47, false );
  font.draw( *_d->background, _("##large##"), 297, 47, false );
  font.draw( *_d->background, _("##Fest.##"), 370, 47, false );
  font.draw( *_d->background, _("##Divinity##"), 450, 47, false );

  Point startPoint( 42, 65 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info = _d->getInfo( city, B_TEMPLE_CERES, B_BIG_TEMPLE_CERES );
  _d->lbCeresInfo = new ReligionInfoLabel( this, Rect( startPoint, labelSize ), DivinePantheon::ceres(), 
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, B_TEMPLE_NEPTUNE, B_BIG_TEMPLE_NEPTUNE );
  _d->lbNeptuneInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), DivinePantheon::neptune(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, B_TEMPLE_MERCURE, B_BIG_TEMPLE_MERCURE );
  _d->lbMercuryInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), DivinePantheon::mercury(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, B_TEMPLE_MARS, B_BIG_TEMPLE_MARS );
  _d->lbMarsInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), DivinePantheon::mars(),
                                          info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, B_TEMPLE_VENUS, B_BIG_TEMPLE_VENUS );
  _d->lbVenusInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 80), labelSize), DivinePantheon::venus(),
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, B_TEMPLE_ORACLE, B_TEMPLE_ORACLE );
  _d->lbOracleInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 100), labelSize), RomeDivinityPtr(),
                                            info.smallTemplCount, 0 );

  _d->btnHelp = new TexturedButton( this, Point( 12, getHeight() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void AdvisorReligionWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}
