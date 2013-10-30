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

#include "advisor_entertainment_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "game/city.hpp"
#include "core/foreach.hpp"
#include "building/house.hpp"
#include "festival_planing_window.hpp"
#include "game/settings.hpp"

namespace gui
{

class EntertainmentInfoLabel : public Label
{
public:
  EntertainmentInfoLabel( Widget* parent, const Rect& rect, const TileOverlayType service, 
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
    case buildingAmphitheater: buildingStr = _("##amphitheatres##"); peoplesStr = _("##peoples##"); break;
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
  TileOverlayType _service;
  int _workingBuilding;
  int _numberBuilding;
  int _peoplesCount;
  int _showBuilding;
};

class AdvisorEntertainmentWindow::Impl
{
public:
  CityPtr city;

  EntertainmentInfoLabel* lbTheatresInfo;
  EntertainmentInfoLabel* lbAmphitheatresInfo;
  EntertainmentInfoLabel* lbColisseumInfo;
  EntertainmentInfoLabel* lbHippodromeInfo;
  Label* lbBlackframe;
  TexturedButton* btnHelp;

  struct InfrastructureInfo
  {
    int buildingCount;
    int buildingWork;
    int buildingShow;
    int peoplesStuding;
  };

  InfrastructureInfo getInfo( CityPtr city, const TileOverlayType service )
  {
    CityHelper helper( city );

    InfrastructureInfo ret;

    ret.buildingWork = 0;
    ret.peoplesStuding = 0;
    ret.buildingShow = 0;
    ret.buildingCount = 0;

    ServiceBuildingList servBuildings = helper.find<ServiceBuilding>( service );
    foreach( ServiceBuildingPtr building, servBuildings )
    {
      if( building->getWorkers() > 0 )
      {
        ret.buildingWork++;

        int maxStuding = 0;
        switch( service )
        {
        case B_THEATER: maxStuding = 500; break;
        case buildingAmphitheater: maxStuding = 800; break;
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

  setupUI( GameSettings::rcpath( "/gui/entertainmentadv.gui" ) );

  _d->lbBlackframe = findChild<Label*>( "lbBlackframe", true );

  _d->updateInfo();

  Point startPoint( 42, 64 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info;
  info = _d->getInfo( city, B_THEATER );
  _d->lbTheatresInfo = new EntertainmentInfoLabel( this, Rect( startPoint, labelSize ), B_THEATER, 
                                             info.buildingWork, info.buildingCount, info.peoplesStuding );

  info = _d->getInfo( city, buildingAmphitheater );
  _d->lbAmphitheatresInfo = new EntertainmentInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), buildingAmphitheater,
                                              info.buildingWork, info.buildingCount, info.peoplesStuding );
  info = _d->getInfo( city, B_COLLOSSEUM );
  _d->lbColisseumInfo = new EntertainmentInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), B_COLLOSSEUM,
                                              info.buildingWork, info.buildingCount, info.peoplesStuding );
  info = _d->getInfo( city, B_HIPPODROME );
  _d->lbHippodromeInfo = new EntertainmentInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), B_HIPPODROME,
                                              info.buildingWork, info.buildingCount, info.peoplesStuding );

  CityHelper helper( city );

  int scholars = 0;
  int students = 0;
  int matures = 0;
  HouseList houses = helper.find<House>( B_HOUSE );
  foreach( HousePtr house, houses )
  {
    scholars += house->getHabitants().count( CitizenGroup::scholar );
    students += house->getHabitants().count( CitizenGroup::student );
    matures  += house->getHabitants().count( CitizenGroup::mature );
  }

  if( PushButton* festivalBtn = findChild<PushButton*>( "btnNewFestival" ) )
  {
    CONNECT( festivalBtn, onClicked(), this, AdvisorEntertainmentWindow::_showFestivalWindow );
  }

  _d->btnHelp = new TexturedButton( this, Point( 12, getHeight() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void AdvisorEntertainmentWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  Widget::draw( painter );
}

void AdvisorEntertainmentWindow::_showFestivalWindow()
{
  FestivalPlaningWindow* wnd = FestivalPlaningWindow::create( this, _d->city, -1 );
  CONNECT( wnd, onFestivalAssign(), _d.data(), Impl::updateFestivalInfo );
}


void AdvisorEntertainmentWindow::Impl::updateInfo()
{
  lbBlackframe->setText( _( "##entrainment_not_need##") );
}

}//end namespace gui
