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


#include "oc3_building_entertainment.hpp"
#include "oc3_positioni.hpp"
#include "oc3_resourcegroup.hpp"

EntertainmentBuilding::EntertainmentBuilding(const Service::Type service,
                                             const BuildingType type,
                                             const Size& size )
  : ServiceBuilding(service, type, size)
{
   switch (service)
   {
   case Service::S_THEATER:
      _traineeMap[WT_ACTOR] = 0;
      break;
   case Service::S_AMPHITHEATER:
      _traineeMap[WT_ACTOR] = 0;
      _traineeMap[WT_GLADIATOR] = 0;
      break;
   case Service::S_COLLOSSEUM:
      _traineeMap[WT_GLADIATOR] = 0;
      _traineeMap[WT_TAMER] = 0;
      break;
   default:
      break;
   }
}

void EntertainmentBuilding::deliverService()
{
   // we need all trainees types for the show
   int minLevel = 100;
   for (std::map<WalkerType, int>::iterator itLevel = _traineeMap.begin(); itLevel != _traineeMap.end(); ++itLevel)
   {
      minLevel = std::min(minLevel, itLevel->second);
   }

   if (minLevel > 10)
   {
      // all trainees are there for the show!
      for (std::map<WalkerType, int>::iterator itLevel = _traineeMap.begin(); itLevel != _traineeMap.end(); ++itLevel)
      {
         itLevel->second = itLevel->second - 10;
      }
      ServiceBuilding::deliverService();
   }
}

int EntertainmentBuilding::getVisitorsNumber() const
{
  return 0;
}

Theater::Theater() : EntertainmentBuilding(Service::S_THEATER, B_THEATER, Size(2))
{
  _getAnimation().load( ResourceGroup::entertaiment, 14, 21);
  _getAnimation().setOffset( Point( 60, 36 ) );

  _fgPictures.resize(2);
  _fgPictures[0] = Picture::load( ResourceGroup::entertaiment, 35);
}

int Theater::getVisitorsNumber() const
{
  return 500;
}

BuildingAmphiTheater::BuildingAmphiTheater() : EntertainmentBuilding(Service::S_AMPHITHEATER, B_AMPHITHEATER, Size(3))
{
  setPicture( Picture::load( ResourceGroup::entertaiment, 1));

  _getAnimation().load( ResourceGroup::entertaiment, 2, 10);
  _getAnimation().setOffset( Point( 100, 49 ) );
  _fgPictures.resize(2);
  _fgPictures[0] = Picture::load( ResourceGroup::entertaiment, 12);
}

Collosseum::Collosseum() : EntertainmentBuilding(Service::S_COLLOSSEUM, B_COLLOSSEUM, Size(5) )
{
  setPicture( Picture::load( ResourceGroup::entertaiment, 36));

  _getAnimation().load( ResourceGroup::entertaiment, 37, 13);
  _getAnimation().setOffset( Point( 122, 81 ) );
  _fgPictures.resize(2);
  _fgPictures[0] = Picture::load( ResourceGroup::entertaiment, 50);
}

//------------

BuildingHippodrome::BuildingHippodrome() : EntertainmentBuilding(Service::S_HIPPODROME, B_HIPPODROME, Size(5) )
{
  setPicture( Picture::load("circus", 5));
  Picture logo = Picture::load("circus", 3);
  Picture logo1 = Picture::load("circus", 1);
  logo.setOffset(150,181);
  logo1.setOffset(300,310);
  _fgPictures.resize(5);
  _fgPictures[ 0 ] = logo;
  _fgPictures[ 1 ] = logo1;
}

//-----------
