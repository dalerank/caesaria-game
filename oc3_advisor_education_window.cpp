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

#include "oc3_advisor_education_window.hpp"
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
#include "oc3_foreach.hpp"

class EducationInfoLabel : public Label
{
public:
  EducationInfoLabel( Widget* parent, const Rect& rect, const BuildingType service, 
                      int workBulding, int numberBuilding, int peoplesCount  )
    : Label( parent, rect )
  {
    _service = service;
    _workingBuilding = workBulding;
    _numberBuilding = numberBuilding;
    _peoplesCount = peoplesCount;

    setFont( Font::create( FONT_1_WHITE ) );
  }

  virtual void _updateTexture( GfxEngine& painter )
  {
    Label::_updateTexture( painter );

    std::string buildingStr, peoplesStr;
    switch( _service )
    {
    case B_SCHOOL: buildingStr = _("##schools##"); peoplesStr = _("##children##"); break;
    case B_COLLEGE: buildingStr = _("##colleges##"); peoplesStr = _("##students##"); break;
    case B_LIBRARY: buildingStr = _("##libraries##"); peoplesStr = _("##peoples##"); break;
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
  BuildingType _service;
  int _workingBuilding;
  int _numberBuilding;
  int _peoplesCount;
};

class AdvisorEducationWindow::Impl
{
public:
  PictureRef background;
  Label* cityInfo;

  EducationInfoLabel* lbSchoolInfo;
  EducationInfoLabel* lbCollegeInfo;
  EducationInfoLabel* lbLibraryInfo;

  struct InfrastructureInfo
  {
    int buildingCount;
    int buildingWork;
    int peoplesStuding;
  };

  InfrastructureInfo getInfo( CityPtr city, const BuildingType service )
  {
    CityHelper helper( city );

    InfrastructureInfo ret;

    ret.buildingWork = 0;
    ret.peoplesStuding = 0;
    ret.buildingCount = 0;

    ServiceBuildingList servBuildings = helper.getBuildings<ServiceBuilding>( service );
    foreach( ServiceBuildingPtr serv, servBuildings )
    {
      if( serv->getWorkers() > 0 )
      {
        ret.buildingWork++;

        int maxStuding = 0;
        switch( service )
        {
        case B_SCHOOL: maxStuding = 75; break;
        case B_COLLEGE: maxStuding = 100; break;
        case B_LIBRARY: maxStuding = 800; break;
        default: break;
        }

        ret.peoplesStuding += maxStuding * serv->getWorkers() / serv->getMaxWorkers();
      }
      ret.buildingCount++;
    }

    return ret;
  }
};


AdvisorEducationWindow::AdvisorEducationWindow( CityPtr city, Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 256 ) ) );

  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( _("##Education advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( 35, 100, getWidth() - 32, 100 + 68 ), PictureDecorator::blackFrame );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##work##"), 180, 82, false );
  font.draw( *_d->background, _("##max_available##"), 290, 82, false );
  font.draw( *_d->background, _("##coverage##"), 480, 82, false );

  Point startPoint( 42, 103 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info = _d->getInfo( city, B_SCHOOL );
  _d->lbSchoolInfo = new EducationInfoLabel( this, Rect( startPoint, labelSize ), B_SCHOOL, 
                                             info.buildingWork, info.buildingCount, info.peoplesStuding );

  info = _d->getInfo( city, B_COLLEGE );
  _d->lbCollegeInfo = new EducationInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), B_COLLEGE,
                                              info.buildingWork, info.buildingCount, info.peoplesStuding );

  info = _d->getInfo( city, B_LIBRARY );
  _d->lbLibraryInfo = new EducationInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), B_LIBRARY,
                                              info.buildingWork, info.buildingCount, info.peoplesStuding );

  CityHelper helper( city );

  int sumScholars = 0;
  int sumStudents = 0;
  HouseList houses = helper.getBuildings<House>( B_HOUSE );
  foreach( HousePtr house, houses )
  {
    sumScholars += house->getScholars();
    //sumStudents += (*it)->getStudents();
  }

  std::string cityInfoStr = StringHelper::format( 0xff, "%d %s, %d %s, %d %s", city->getPopulation(), _("##peoples##"),
                                                  sumScholars, _("##scholars##"), sumStudents, _("##students##") );
  _d->cityInfo = new Label( this, Rect( 65, 50, getWidth() - 65, 50 +30), cityInfoStr, false );
}

void AdvisorEducationWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}
