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
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "cityservice_culture.hpp"
#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "objects/house.hpp"
#include "objects/entertainment.hpp"
#include "objects/house_level.hpp"
#include "gfx/tile.hpp"
#include "core/time.hpp"
#include "game/gamedate.hpp"
#include "objects/religion.hpp"
#include "objects/education.hpp"
#include "objects/constants.hpp"
#include "objects/theater.hpp"

using namespace constants;

namespace city
{

struct Coverage2Point{
  double coverage;
  int points;
} ;

typedef Coverage2Point CoveragePoints[6];
static const CoveragePoints religionPoints  = { {1.0, 30}, {0.86,22}, {0.71,14}, {0.51, 9}, {0.31, 3}, {0.0, 0} };
static const CoveragePoints theatresPoints  = { {1.0, 25}, {0.86,18}, {0.71,12}, {0.51, 8}, {0.31, 3}, {0.0, 0} };
static const CoveragePoints librariesPoints = { {1.0, 20}, {0.86,14}, {0.71,8 }, {0.51, 4}, {0.31, 2}, {0.0, 0} };
static const CoveragePoints schoolsPoints   = { {1.0, 15}, {0.86,10}, {0.71,6 }, {0.51, 4}, {0.31, 1}, {0.0, 0} };
static const CoveragePoints academiesPoints = { {1.0, 10}, {0.86,7 }, {0.71,4 }, {0.51, 2}, {0.31, 1}, {0.0, 0} };

class CultureRating::Impl
{
public:
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
  float hippodromeCoverage;
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

SrvcPtr CultureRating::create( PlayerCityPtr city )
{
  SrvcPtr ret( new CultureRating( city ) );
  ret->drop();

  return ret;
}

CultureRating::CultureRating( PlayerCityPtr city )
  : Srvc( city, defaultName() ), _d( new Impl )
{
  _d->lastDate = game::Date::current();
  _d->culture = 0;
}

void CultureRating::timeStep(const unsigned int time )
{
  if( !game::Date::isMonthChanged() )
    return;

  if( _d->lastDate.monthsTo( game::Date::current() ) > 0 )
  {
    _d->lastDate = game::Date::current();
    _d->parishionersCount = 0;
    _d->theaterVisitors = 0;
    _d->libraryVisitors = 0;
    _d->schoolVisitors = 0;
    _d->collegeVisitors = 0;
    int cityPopulation = _city()->population();

    Helper helper( _city() );

    TempleList temples = helper.find<Temple>( objects::religionGroup );
    foreach( temple, temples )
    {
      _d->parishionersCount += (*temple)->parishionerNumber();
    }

    _d->religionCoverage = _d->parishionersCount / (float)cityPopulation;
    _d->religionPoints = _d->convCoverage2Points( religionPoints, _d->religionCoverage );

    TheaterList theaters = helper.find<Theater>( objects::theater );
    foreach( theater, theaters )
    {
      _d->theaterVisitors += (*theater)->visitorsNumber();
    }
    _d->theatersCoverage = _d->theaterVisitors / (float)cityPopulation;
    _d->theatresPoints = _d->convCoverage2Points( theatresPoints, _d->theatersCoverage );

    LibraryList libraries = helper.find<Library>( objects::library );
    foreach( library, libraries )
    {
      _d->libraryVisitors += (*library)->getVisitorsNumber();
    }
    _d->libraryCoverage = _d->libraryVisitors / (float)cityPopulation;
    _d->libraryPoints = _d->convCoverage2Points( librariesPoints, _d->libraryCoverage );

    SchoolList schools = helper.find<School>( objects::school );
    foreach( school, schools )
    {
      _d->schoolVisitors += (*school)->getVisitorsNumber();
    }
    _d->schoolCoverage = _d->schoolVisitors / (float)cityPopulation;
    _d->schoolPoints = _d->convCoverage2Points( schoolsPoints, _d->schoolCoverage );

    AcademyList colleges = helper.find<Academy>( objects::academy );
    foreach( college, colleges )
    {
      _d->collegeVisitors += (*college)->getVisitorsNumber();
    }
    _d->collegeCoverage = _d->collegeVisitors / (float)cityPopulation;
    _d->collegePoints = _d->convCoverage2Points( academiesPoints, _d->collegeCoverage );

    _d->culture = ( _d->culture + _d->religionPoints + _d->theatresPoints + 
                    _d->libraryPoints + _d->schoolPoints + _d->collegePoints ) / 2;    
  }
}

int CultureRating::value() const {  return _d->culture; }

int CultureRating::coverage( Coverage type) const
{
  switch( type )
  {
  case covSchool: return _d->schoolCoverage * 100;
  case covLibrary: return _d->libraryCoverage * 100;
  case covAcademy: return _d->collegeCoverage * 100;
  case covReligion: return _d->religionCoverage * 100;
  case covTheatres: return _d->theatersCoverage * 100;
  default: return 0;
  }
}

std::string CultureRating::defaultName() { return CAESARIA_STR_EXT(CultureRating); }

}//end namespace city
