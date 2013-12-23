// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "advisor_health_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"
#include "texturedbutton.hpp"
#include "objects/constants.hpp"
#include "objects/service.hpp"

using namespace constants;

namespace gui
{

class HealthInfoLabel : public Label
{
public:
  HealthInfoLabel( Widget* parent, const Rect& rect, const TileOverlay::Type service,
                   int workBulding, int numberBuilding, int peoplesCount  )
    : Label( parent, rect )
  {
    _service = service;
    _workingBuilding = workBulding;
    _numberBuilding = numberBuilding;
    _peoplesCount = peoplesCount;

    setFont( Font::create( FONT_1 ) );
  }

  virtual void _updateTexture( GfxEngine& painter )
  {
    Label::_updateTexture( painter );

    std::string buildingStr, peoplesStr;
    switch( _service )
    {
    case building::baths: buildingStr = _("##bath##"); peoplesStr = _("##peoples##"); break;
    case building::barber: buildingStr = _("##barber##"); peoplesStr = _("##peoples##"); break;
    case building::hospital: buildingStr = _("##hospital##"); peoplesStr = _("##patients##"); break;
    case building::doctor: buildingStr = _("##doctor##"); peoplesStr = _("##peoples##"); break;
    default: break;
    }

    PictureRef& texture = getTextPicture();
    Font font = getFont();
    std::string buildingStrT = StringHelper::format( 0xff, "%d %s", _numberBuilding, buildingStr.c_str() );
    font.draw( *texture, buildingStrT, 0, 0 );

    std::string buildingWorkT = StringHelper::format( 0xff, "%d", _workingBuilding );
    font.draw( *texture, buildingWorkT, 165, 0 );

    std::string peoplesStrT = StringHelper::format( 0xff, "%d %s", _peoplesCount, peoplesStr.c_str() );
    font.draw( *texture, peoplesStrT, 255, 0 );
  }

private:
  TileOverlay::Type _service;
  int _workingBuilding;
  int _numberBuilding;
  int _peoplesCount;
};

class AdvisorHealthWindow::Impl
{
public:
  PictureRef background;

  HealthInfoLabel* lbBathsInfo;
  HealthInfoLabel* lbBarbersInfo;
  HealthInfoLabel* lbDoctorInfo;
  HealthInfoLabel* lbHospitalInfo;
  TexturedButton* btnHelp;

  struct InfrastructureInfo
  {
    int buildingCount;
    int buildingWork;
    int peoplesServed;
  };

  InfrastructureInfo getInfo( PlayerCityPtr city, const TileOverlay::Type service )
  {
    CityHelper helper( city );

    InfrastructureInfo ret;

    ret.buildingWork = 0;
    ret.peoplesServed = 0;
    ret.buildingCount = 0;

    ServiceBuildingList srvBuildings = helper.find<ServiceBuilding>( service );
    foreach( ServiceBuildingPtr building, srvBuildings )
    {
      ret.buildingWork += building->getWorkersCount() > 0 ? 1 : 0;
      ret.buildingCount++;
    }

    return ret;
  }
};


AdvisorHealthWindow::AdvisorHealthWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 290 ) ) );

  Label* title = new Label( this, Rect( 60, 10, getWidth() - 10, 10 + 40) );
  title->setText( _("##Health advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );
  Picture& icon = Picture::load( ResourceGroup::panelBackground, 261 );
  _d->background->draw( icon, Point( 11, 11 ) );

  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( 35, 110, getWidth() - 35, 110 + 85 ), PictureDecorator::blackFrame );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##work##"), 180, 92, false );
  font.draw( *_d->background, _("##max_available##"), 290, 92, false );
  font.draw( *_d->background, _("##coverage##"), 480, 92, false );

  Point startPoint( 42, 112 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info = _d->getInfo( city, building::baths );
  _d->lbBathsInfo = new HealthInfoLabel( this, Rect( startPoint, labelSize ), building::baths,
                                             info.buildingWork, info.buildingCount, info.peoplesServed );

  info = _d->getInfo( city, building::barber );
  _d->lbBarbersInfo = new HealthInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), building::barber,
                                              info.buildingWork, info.buildingCount, info.peoplesServed );

  info = _d->getInfo( city, building::doctor );
  _d->lbDoctorInfo = new HealthInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), building::doctor,
                                              info.buildingWork, info.buildingCount, info.peoplesServed );

  info = _d->getInfo( city, building::hospital );
  _d->lbDoctorInfo = new HealthInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), building::hospital,
                                          info.buildingWork, info.buildingCount, info.peoplesServed );

  _d->btnHelp = new TexturedButton( this, Point( 12, getHeight() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void AdvisorHealthWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}

}//end namespace gui
