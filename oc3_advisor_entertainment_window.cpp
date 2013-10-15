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

#include "oc3_advisor_entertainment_window.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_gettext.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_gettext.hpp"
#include "oc3_enums.hpp"
#include "oc3_city.hpp"
#include "oc3_foreach.hpp"
#include "oc3_building_house.hpp"
#include "oc3_window_festival_planing.hpp"

class EntertainmentInfoLabel : public Label
{
public:
  EntertainmentInfoLabel( Widget* parent, const Rect& rect, const BuildingType service, 
                      int workBulding, int numberBuilding, int peoplesCount  )
    : Label( parent, rect )
  {
    _service = service;
    _workingBuilding = workBulding;
    _numberBuilding = numberBuilding;
    _peoplesCount = peoplesCount;
    _showBuilding = 0;

    setFont( Font::create( FONT_1_WHITE ) );
  }

  virtual void _updateTexture( GfxEngine& painter )
  {
    Label::_updateTexture( painter );

    std::string buildingStr, peoplesStr;
    switch( _service )
    {
    case B_THEATER: buildingStr = _("##theaters##"); peoplesStr = _("##peoples##"); break;
    case B_AMPHITHEATER: buildingStr = _("##amphitheatres##"); peoplesStr = _("##peoples##"); break;
    case B_COLLOSSEUM: buildingStr = _("##colloseum##"); peoplesStr = _("##peoples##"); break;
    case B_HIPPODROME: buildingStr = _("##hippodromes##"); peoplesStr = "-"; break;
    default:
    break;
    }

    PictureRef& texture = getTextPicture();
    Font font = getFont();
    font.draw( *texture, StringHelper::format( 0xff, "%d %s", _numberBuilding, buildingStr.c_str() ), 0, 0 );
    font.draw( *texture, StringHelper::format( 0xff, "%d", _workingBuilding ), 165, 0 );
    font.draw( *texture, StringHelper::format( 0xff, "%d", _showBuilding ), 245, 0 );
    font.draw( *texture, StringHelper::format( 0xff, "%d %s", _peoplesCount, peoplesStr.c_str() ), 305, 0 );
  }

private:
  BuildingType _service;
  int _workingBuilding;
  int _numberBuilding;
  int _peoplesCount;
  int _showBuilding;
};

class AdvisorEntertainmentWindow::Impl
{
public:
  CityPtr city;
  PictureRef background;

  EntertainmentInfoLabel* lbTheatresInfo;
  EntertainmentInfoLabel* lbAmphitheatresInfo;
  EntertainmentInfoLabel* lbColisseumInfo;
  EntertainmentInfoLabel* lbHippodromeInfo;
  Label* lbInfo;
  TexturedButton* btnHelp;

  struct InfrastructureInfo
  {
    int buildingCount;
    int buildingWork;
    int buildingShow;
    int peoplesStuding;
  };

  InfrastructureInfo getInfo( CityPtr city, const BuildingType service )
  {
    CityHelper helper( city );

    InfrastructureInfo ret;

    ret.buildingWork = 0;
    ret.peoplesStuding = 0;
    ret.buildingShow = 0;
    ret.buildingCount = 0;

    ServiceBuildingList servBuildings = helper.getBuildings<ServiceBuilding>( service );
    foreach( ServiceBuildingPtr building, servBuildings )
    {
      if( building->getWorkers() > 0 )
      {
        ret.buildingWork++;

        int maxStuding = 0;
        switch( service )
        {
        case B_THEATER: maxStuding = 500; break;
        case B_AMPHITHEATER: maxStuding = 800; break;
        case B_COLLOSSEUM: maxStuding = 1500; break;
        default:
        break;
        }

        ret.peoplesStuding += maxStuding * building->getWorkers() / building->getMaxWorkers();
      }
      ret.buildingCount++;
    }

    return ret;
  }

  void updateFestivalInfo()
  {

  }

  void updateInfo();
};


AdvisorEntertainmentWindow::AdvisorEntertainmentWindow( CityPtr city, Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;

  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 384 ) ) );

  Label* title = new Label( this, Rect( 60, 10, getWidth() - 10, 10 + 40) );
  title->setText( _("##entertainment_advisor_title##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( 32, 60, getWidth() - 32, 60 + 86 ), PictureDecorator::blackFrame );
  _d->lbInfo = new Label( this, Rect( 50, 145, getWidth() - 50, 145 + 75) );

  _d->updateInfo();

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 263 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##work##"), 180, 45, false );
  font.draw( *_d->background, _("##show##"), 260, 45, false );
  font.draw( *_d->background, _("##max_available##"), 350, 45, false);
  font.draw( *_d->background, _("##coverage##"), 480, 45, false );

  Point startPoint( 42, 64 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info = _d->getInfo( city, B_THEATER );
  _d->lbTheatresInfo = new EntertainmentInfoLabel( this, Rect( startPoint, labelSize ), B_THEATER, 
                                             info.buildingWork, info.buildingCount, info.peoplesStuding );

  info = _d->getInfo( city, B_AMPHITHEATER );
  _d->lbAmphitheatresInfo = new EntertainmentInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), B_AMPHITHEATER,
                                              info.buildingWork, info.buildingCount, info.peoplesStuding );

  info = _d->getInfo( city, B_COLLOSSEUM );
  _d->lbColisseumInfo = new EntertainmentInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), B_COLLOSSEUM,
                                              info.buildingWork, info.buildingCount, info.peoplesStuding );

  info = _d->getInfo( city, B_HIPPODROME );
  _d->lbHippodromeInfo = new EntertainmentInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), B_HIPPODROME,
                                                  info.buildingWork, info.buildingCount, info.peoplesStuding );

  CityHelper helper( city );

  int sumScholars = 0;
  //int sumStudents = 0;
  HouseList houses = helper.getBuildings<House>( B_HOUSE );
  foreach( HousePtr house, houses )
  {
    sumScholars += house->getHabitants().count( CitizenGroup::young );
    //sumStudents += (*it)->getStudents();
  }

  //festival
  PictureDecorator::draw( *_d->background, Rect( 50, 247, getWidth() - 50, 247 + 110 ), PictureDecorator::blackFrame );

  Label* festivalTitle = new Label( this, Rect( 50, 218, getWidth() - 50, 218 + 35) );
  festivalTitle->setText( _("##Festivals##") );
  festivalTitle->setFont( Font::create( FONT_3 ) );
  festivalTitle->setTextAlignment( alignUpperLeft, alignCenter );

  PushButton* festivalBtn = new PushButton( this, Rect( Point( 104, 278 ), Size( 300, 20) ), 
                                            _("##new_festival##"), -1, false, PushButton::blackBorderUp );

  CONNECT( festivalBtn, onClicked(), this, AdvisorEntertainmentWindow::_showFestivalWindow );

  Picture& pic = Picture::load( ResourceGroup::menuMiddleIcons, 16 );
  _d->background->draw( pic, 460, 260 );

  _d->btnHelp = new TexturedButton( this, Point( 12, getHeight() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void AdvisorEntertainmentWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}

void AdvisorEntertainmentWindow::_showFestivalWindow()
{
  FestivalPlaningWindow* wnd = FestivalPlaningWindow::create( this, _d->city, -1 );
  CONNECT( wnd, onFestivalAssign(), _d.data(), Impl::updateFestivalInfo );
}


void AdvisorEntertainmentWindow::Impl::updateInfo()
{
  lbInfo->setText( _( "##entrainment_not_need##") );
}
