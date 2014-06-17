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
#include "core/variant.hpp"

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
  bool success;
  StringArray overview;
  std::string shortDesc,
              caption,
              nextMission,
              newTitle;
};

VictoryConditions::VictoryConditions() : _d( new Impl )
{
  _d->success = false;
  _d->maxHouseLevel = 30;
  _d->population = 0;
  _d->culture = 0;
  _d->prosperity = 0;
  _d->favour = 0;
  _d->peace = 0;
}

VictoryConditions::~VictoryConditions()
{

}

bool VictoryConditions::isSuccess( int culture, int prosperity,
                                int favour, int peace,
                                int population ) const
{
  if( (_d->population + _d->culture + _d->prosperity + _d->favour + _d->peace) == 0 )
    return false;

  _d->success = (_d->population <= population &&
                 _d->culture <= culture && _d->prosperity <= prosperity &&
                 _d->favour <= favour && _d->peace <= peace);

  return _d->success;
}

void VictoryConditions::load( const VariantMap& stream )
{
  _d->maxHouseLevel = HouseSpecHelper::instance().geLevel( stream.get( "maxHouseLevel" ).toString() );
  _d->success = stream.get( "success" ).toBool();
  _d->population = (int)stream.get( "population" );
  _d->culture = (int)stream.get( "culture" );
  _d->prosperity = (int)stream.get( "prosperity" );
  _d->favour = (int)stream.get( "favour" );
  _d->peace = (int)stream.get( "peace" );
  _d->overview = stream.get( "overview" ).toStringArray();
  _d->shortDesc = stream.get( "short" ).toString();
  _d->caption = stream.get( "caption" ).toString();
  _d->nextMission = stream.get( "next" ).toString();
  _d->newTitle = stream.get( "title" ).toString();
}

VariantMap VictoryConditions::save() const
{
  VariantMap ret;
  ret[ "success" ] = _d->success;
  ret[ "culture" ] = _d->culture;
  ret[ "population" ] = _d->population;
  ret[ "prosperity" ] = _d->prosperity;
  ret[ "favour" ]  = _d->favour;
  ret[ "peace"  ]  = _d->peace;
  ret[ "overview" ] = Variant( _d->overview );
  ret[ "short" ] = Variant( _d->shortDesc );
  ret[ "caption" ] = Variant( _d->caption );
  ret[ "next" ] = Variant( _d->nextMission );
  ret[ "title" ] = Variant( _d->newTitle );

  return ret;
}

VictoryConditions&VictoryConditions::operator=(const VictoryConditions& a)
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
  _d->nextMission = a._d->nextMission;
  _d->newTitle = a._d->newTitle;

  return *this;
}

int VictoryConditions::needCulture() const{  return _d->culture;}
int VictoryConditions::needProsperity() const{  return _d->prosperity;}
int VictoryConditions::needFavour() const{  return _d->favour;}
int VictoryConditions::needPeace() const{  return _d->peace;}
std::string VictoryConditions::getShortDesc() const {  return _d->shortDesc;}
std::string VictoryConditions::getNextMission() const { return _d->nextMission; }
std::string VictoryConditions::getNewTitle() const { return _d->newTitle; }
int VictoryConditions::needPopulation() const{  return _d->population;}
const StringArray& VictoryConditions::getOverview() const{  return _d->overview;}

}//end namespace city
