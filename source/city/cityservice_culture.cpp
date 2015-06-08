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
#include "city/statistic.hpp"
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
#include "cityservice_factory.hpp"
#include "city/states.hpp"

namespace city
{

REGISTER_SERVICE_IN_FACTORY(CultureRating,culture)

namespace coverage
{
enum { levelNumber = 6 };
struct Point {
  double coverage;
  int points;
};
typedef Point Points[ levelNumber ];
static const Points religion =  { {1.0,30}, {0.86,22}, {0.71,14}, {0.51,9}, {0.31,3}, {0.0,0} };
static const Points theatres =  { {1.0,25}, {0.86,18}, {0.71,12}, {0.51,8}, {0.31,3}, {0.0,0} };
static const Points libraries = { {1.0,20}, {0.86,14}, {0.71,8},  {0.51,4}, {0.31,2}, {0.0,0} };
static const Points schools =   { {1.0,15}, {0.86,10}, {0.71,6},  {0.51,4}, {0.31,1}, {0.0,0} };
static const Points academies = { {1.0,10}, {0.86,7},  {0.71,4},  {0.51,2}, {0.31,1}, {0.0,0} };

struct SubRating
{
  const Points& intervals;
  int coverage;
  int value;
  int visitors;

  SubRating( const Points& v) : intervals(v), coverage(0), value(0) {}

  void update( double coverageValue )
  {
    value = 0;
    coverageValue = std::min( coverageValue, 1.0 );
    coverage = coverageValue * 100;
    for( int i=0; i < coverage::levelNumber; i++ )
    {
      if( coverageValue >= intervals[ i ].coverage )
      {
        value = intervals[ i ].points * coverageValue;
        break;
      }
    }
  }
};

}//end namespace coverage

class CultureRating::Impl
{
public:
  DateTime lastDate;
  int culture;
  coverage::SubRating religion;
  coverage::SubRating theaters;
  coverage::SubRating libraries;
  coverage::SubRating schools;
  coverage::SubRating academies;
  float hippodromeCoverage;

  int collegePoints;

  Impl() : religion(coverage::religion),
           theaters(coverage::theatres),
           libraries(coverage::libraries),
           schools(coverage::schools),
           academies(coverage::academies)
           {}
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
    _d->religion.visitors = 0;
    _d->theaters.visitors = 0;
    _d->libraries.visitors = 0;
    _d->schools.visitors = 0;
    _d->academies.visitors = 0;
    int cityPopulation = _city()->states().population;

    TempleList temples = city::statistic::getObjects<Temple>( _city(), object::group::religion );
    foreach( temple, temples ) { _d->religion.visitors += (*temple)->parishionerNumber(); }
    _d->religion.update( _d->religion.visitors / (float)cityPopulation );

    TheaterList theaters = city::statistic::getObjects<Theater>( _city(), object::theater );
    foreach( theater, theaters ) { _d->theaters.visitors += (*theater)->currentVisitors(); }
    _d->theaters.update( _d->theaters.visitors / (float)cityPopulation );

    LibraryList libraries = city::statistic::getObjects<Library>( _city(), object::library );
    foreach( library, libraries ) { _d->libraries.visitors += (*library)->currentVisitors(); }
    _d->libraries.update( _d->libraries.visitors / (float)cityPopulation );

    SchoolList schools = city::statistic::getObjects<School>( _city(), object::school );
    foreach( school, schools ) { _d->schools.visitors += (*school)->currentVisitors(); }

    _d->schools.update( _d->schools.visitors / (float)cityPopulation );

    AcademyList colleges = city::statistic::getObjects<Academy>( _city(), object::academy );
    foreach( college, colleges ) { _d->academies.visitors += (*college)->currentVisitors(); }
    _d->academies.update( _d->academies.visitors / (float)cityPopulation );

    _d->culture = ( _d->culture + _d->religion.value + _d->theaters.value +
                    _d->libraries.value + _d->schools.value + _d->academies.value ) / 2;
  }
}

VariantMap CultureRating::save() const
{
  VariantMap ret = Srvc::save();
  VARIANT_SAVE_ANY_D( ret, _d, lastDate )
  VARIANT_SAVE_ANY_D( ret, _d, culture  )

  return ret;
}

void CultureRating::load(const VariantMap &stream)
{
  Srvc::load( stream );

  VARIANT_LOAD_TIME_D( _d, lastDate, stream )
  VARIANT_LOAD_ANY_D ( _d, culture,  stream )
}

int CultureRating::value() const {  return _d->culture; }

int CultureRating::coverage( Coverage type) const
{
  switch( type )
  {
  case covSchool: return _d->schools.coverage * 100;
  case covLibrary: return _d->libraries.coverage * 100;
  case covAcademy: return _d->academies.coverage * 100;
  case covReligion: return _d->religion.coverage * 100;
  case covTheatres: return _d->theaters.coverage * 100;
  default: return 0;
  }
}

std::string CultureRating::defaultName() { return CAESARIA_STR_EXT(CultureRating); }

}//end namespace city
