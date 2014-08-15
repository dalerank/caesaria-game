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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com


#ifndef _CAESARIA_ENUMS_INCLUDE_H_
#define _CAESARIA_ENUMS_INCLUDE_H_

enum ClimateType { climateCentral=0, climateNorthen, climateDesert, climateCount };

enum FestivalType { smallFest=1, middleFest, greatFest };

enum ScreenType { SCREEN_NONE, SCREEN_MENU, SCREEN_GAME, SCREEN_BRIEFING, SCREEN_QUIT, SCREEN_MAX };

enum BuildMenuType 
{ 
  BM_NONE, 
  BM_WATER, 
  BM_HEALTH, 
  BM_SECURITY, 
  BM_EDUCATION, 
  BM_ENGINEERING, 
  BM_ADMINISTRATION, 
  BM_ENTERTAINMENT, 
  BM_COMMERCE, 
  BM_FARM, 
  BM_RAW_MATERIAL, 
  BM_FACTORY, 
  BM_RELIGION, 
  BM_TEMPLE, 
  BM_BIGTEMPLE, 
  BM_MAX 
};

#endif  //_CAESARIA_ENUMS_INCLUDE_H_
