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

#include "victoryconditions.hpp"
#include "objects/house_level.hpp"
#include "core/variant_map.hpp"

namespace city
{

class VictoryConditions::Impl
{
public:
  int maxHouseLevel;
  int population;
  int culture;
  int prosperity;
  int favour;
  int peace;
  bool may_continue;
  bool success;
  int reignYears;
  DateTime finishDate;
  StringArray overview;

  std::string shortDesc,
              caption,
              next,
              title,
              winText;
};

VictoryConditions::VictoryConditions() : _d( new Impl )
{
  _d->success = false;
  _d->maxHouseLevel = 30;
  _d->population = 0;
  _d->culture = 0;
  _d->prosperity = 0;
  _d->favour = 0;
  _d->may_continue = false;
  _d->peace = 0;
  _d->reignYears = 0;
  _d->finishDate = DateTime( 500, 1, 1 );
}

VictoryConditions::~VictoryConditions(){}

bool VictoryConditions::isSuccess( int culture, int prosperity,
                                   int favour, int peace,
                                   int population ) const
{
  if( (_d->population + _d->culture + _d->prosperity + _d->favour + _d->peace) == 0 )
    return false;

  if( _d->reignYears > 0 )
    return false;

  _d->success = (_d->population <= population &&
                 _d->culture <= culture && _d->prosperity <= prosperity &&
                 _d->favour <= favour && _d->peace <= peace);

  return _d->success;
}

bool VictoryConditions::isSuccess() const {  return _d->success; }
bool VictoryConditions::mayContinue() const { return _d->may_continue; }

void VictoryConditions::load( const VariantMap& stream )
{
  _d->maxHouseLevel = HouseSpecHelper::instance().getLevel( stream.get( "maxHouseLevel" ).toString() );
  VARIANT_LOAD_ANY_D( _d, success, stream )
  VARIANT_LOAD_ANY_D( _d, population, stream )
  VARIANT_LOAD_ANY_D( _d, culture, stream )
  VARIANT_LOAD_ANY_D( _d, prosperity, stream )
  VARIANT_LOAD_ANY_D( _d, favour, stream )
  VARIANT_LOAD_ANY_D( _d, peace, stream )
  VARIANT_LOAD_TIME_D( _d, finishDate, stream )
  VARIANT_LOAD_ANY_D( _d, reignYears, stream )
  VARIANT_LOAD_ANY_D( _d, may_continue, stream )
  if( _d->finishDate.year() < -9000 )
  {
    _d->finishDate = DateTime( 500, 1, 1 );
  }

  _d->overview = stream.get( "overview" ).toStringArray();
  _d->shortDesc = stream.get( "short" ).toString();
  _d->winText = stream.get( "win.text" ).toString();
  VARIANT_LOAD_STR_D( _d, caption, stream)
  VARIANT_LOAD_STR_D( _d, next, stream )
  VARIANT_LOAD_STR_D( _d, title, stream )
}

VariantMap VictoryConditions::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, success )
  VARIANT_SAVE_ANY_D( ret, _d, culture )
  VARIANT_SAVE_ANY_D( ret, _d, population )
  VARIANT_SAVE_ANY_D( ret, _d, prosperity )
  VARIANT_SAVE_ANY_D( ret, _d, favour )
  VARIANT_SAVE_ANY_D( ret, _d, peace )
  VARIANT_SAVE_STR_D( ret, _d, overview )
  VARIANT_SAVE_ANY_D( ret, _d, finishDate )
  VARIANT_SAVE_ANY_D( ret, _d, reignYears )
  ret[ "short"      ] = Variant( _d->shortDesc );
  ret[ "win.text"   ] = Variant( _d->winText );
  VARIANT_SAVE_STR_D( ret, _d, caption )
  VARIANT_SAVE_STR_D( ret, _d, may_continue )
  VARIANT_SAVE_STR_D( ret, _d, next )
  VARIANT_SAVE_STR_D( ret, _d, title )
  return ret;
}

VictoryConditions& VictoryConditions::operator=(const VictoryConditions& a)
{
  _d->maxHouseLevel = a._d->maxHouseLevel;
  _d->success = a._d->success;
  _d->population = a._d->population;
  _d->culture = a._d->culture;
  _d->prosperity = a._d->prosperity;
  _d->favour = a._d->favour;
  _d->peace = a._d->peace;
  _d->overview = a._d->overview;
  _d->shortDesc = a._d->shortDesc;
  _d->caption = a._d->caption;
  _d->next = a._d->next;
  _d->title = a._d->title;
  _d->finishDate = a._d->finishDate;
  _d->winText = a._d->winText;
  _d->reignYears = a._d->reignYears;

  return *this;
}

int VictoryConditions::needCulture() const{  return _d->culture;}
int VictoryConditions::needProsperity() const{  return _d->prosperity;}
int VictoryConditions::needFavour() const{  return _d->favour;}
int VictoryConditions::needPeace() const{  return _d->peace;}
const DateTime &VictoryConditions::finishDate() const { return _d->finishDate; }
std::string VictoryConditions::shortDesc() const {  return _d->shortDesc;}
std::string VictoryConditions::nextMission() const { return _d->next; }
std::string VictoryConditions::newTitle() const { return _d->title; }
std::string VictoryConditions::winText() const{ return _d->winText; }
int VictoryConditions::needPopulation() const{  return _d->population;}
const StringArray& VictoryConditions::overview() const{  return _d->overview;}
void VictoryConditions::addReignYears(int value){  _d->reignYears += value;}
void VictoryConditions::decreaseReignYear(){ _d->reignYears = std::max<int>( _d->reignYears-1, 0); }

}//end namespace city
