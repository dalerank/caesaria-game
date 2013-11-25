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

#include "cityservice_culture.hpp"
#include "city.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "building/house.hpp"
#include "building/entertainment.hpp"
#include "house_level.hpp"
#include "gfx/tile.hpp"
#include "core/time.hpp"
#include "gamedate.hpp"
#include "building/religion.hpp"
#include "building/education.hpp"
#include "building/constants.hpp"

using namespace constants;

struct Coverage2Point{
  double coverage;
  int points;
} ;

typedef Coverage2Point CoveragePoints[6];
static const CoveragePoints religionPoints = { {1.0, 30}, {0.86,22}, {0.71,14}, {0.51, 9}, {0.31, 3}, {0.0, 0} };
static const CoveragePoints theatresPoints = { {1.0, 25}, {0.86,18}, {0.71,12}, {0.51, 8}, {0.31, 3}, {0.0, 0} };
static const CoveragePoints librariesPoints = { {1.0, 20}, {0.86,14}, {0.71,8}, {0.51, 4}, {0.31, 2}, {0.0, 0} };
static const CoveragePoints schoolsPoints = { {1.0, 15}, {0.86,10}, {0.71,6}, {0.51, 4}, {0.31, 1}, {0.0, 0} };
static const CoveragePoints academiesPoints = { {1.0, 10}, {0.86,7}, {0.71,4}, {0.51, 2}, {0.31, 1}, {0.0, 0} };

class CityServiceCulture::Impl
{
public:
  PlayerCityPtr city;
  DateTime lastDate;
  int culture;
  int parishionersCount;
  int theaterVisitors;
  int libraryVisitors;
  int schoolVisitors;
  int collegeVisitors;
  float religionCoverage;
  float theatersCoverage;
  float libraryCoverage;
  float schoolCoverage;
  float collegeCoverage;
  int religionPoints;
  int theatresPoints;
  int libraryPoints;
  int schoolPoints;
  int collegePoints;

  int convCoverage2Points( const CoveragePoints& covp, double value )
  {
    value = std::min( value, 1.0 );
    for( int i=0; i < 6; i++ )
    {
      if( value >= covp[ i ].coverage )
      {
        return (int)(covp[ i ].points * value);
      }
    }

    return 0;
  }
};

CityServicePtr CityServiceCulture::create(PlayerCityPtr city )
{
  CityServicePtr ret( new CityServiceCulture( city ) );
  ret->drop();

  return ret;
}

CityServiceCulture::CityServiceCulture(PlayerCityPtr city )
  : CityService( getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->lastDate = GameDate::current();
  _d->culture = 0;
}

void CityServiceCulture::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;

  if( _d->lastDate.getMonthToDate( GameDate::current() ) > 0 )
  {
    _d->lastDate = GameDate::current();
    _d->parishionersCount = 0;
    _d->theaterVisitors = 0;
    int cityPopulation = _d->city->getPopulation();

    CityHelper helper( _d->city );

    TempleList temples = helper.find<Temple>( building::religionGroup );
    foreach( TemplePtr temple, temples )
    {
      _d->parishionersCount += temple->getParishionerNumber();
    }

    _d->religionCoverage = _d->parishionersCount / (float)cityPopulation;
    _d->religionPoints = _d->convCoverage2Points( religionPoints, _d->religionCoverage );

    TheaterList theaters = helper.find<Theater>( building::theater );
    foreach( TheaterPtr theater, theaters )
    {
      _d->theaterVisitors += theater->getVisitorsNumber();
    }
    _d->theatersCoverage = _d->theaterVisitors / (float)cityPopulation;
    _d->theatresPoints = _d->convCoverage2Points( theatresPoints, _d->theatersCoverage );

    LibraryList libraries = helper.find<Library>( building::library );
    foreach( LibraryPtr library, libraries )
    {
      _d->libraryVisitors += library->getVisitorsNumber();
    }
    _d->libraryCoverage = _d->libraryVisitors / (float)cityPopulation;
    _d->libraryPoints = _d->convCoverage2Points( librariesPoints, _d->libraryCoverage );

    SchoolList schools = helper.find<School>( building::school );
    foreach( SchoolPtr school, schools )
    {
      _d->schoolVisitors += school->getVisitorsNumber();
    }
    _d->schoolCoverage = _d->schoolVisitors / (float)cityPopulation;
    _d->schoolPoints = _d->convCoverage2Points( schoolsPoints, _d->schoolCoverage );

    AcademyList colleges = helper.find<Academy>( building::academy );
    foreach( AcademyPtr college, colleges )
    {
      _d->collegeVisitors += college->getVisitorsNumber();
    }
    _d->collegeCoverage = _d->collegeVisitors / (float)cityPopulation;
    _d->collegePoints = _d->convCoverage2Points( academiesPoints, _d->collegeCoverage );

    _d->culture = ( _d->culture + _d->religionPoints + _d->theatresPoints + 
                    _d->libraryPoints + _d->schoolPoints + _d->collegePoints ) / 2;
  }
}

int CityServiceCulture::getValue() const
{
  return _d->culture;
}

int CityServiceCulture::getCoverage( Coverage type) const
{
  switch( type )
  {
  case ccSchool: return _d->schoolCoverage * 100;
  case ccLibrary: return _d->libraryCoverage * 100;
  case ccAcademy: return _d->collegeCoverage * 100;
  case ccReligion: return _d->religionCoverage * 100;
  case ccTheatres: return _d->theatersCoverage * 100;
  }

  return 0;
}

std::string CityServiceCulture::getDefaultName()
{
  return CAESARIA_STR_EXT(CityServiceCulture);
}
