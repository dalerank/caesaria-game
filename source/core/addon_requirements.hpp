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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_ADDON_REQUIREMENTS_INCLUDE_HPP__
#define __CAESARIA_ADDON_REQUIREMENTS_INCLUDE_HPP__

#ifdef CAESARIA_PLATFORM_WIN
  #ifdef CAESARIA_ADDON_DEFINED
    #define  ADDON_EXPORT __declspec(dllexport)
  #else
    #define  ADDON_EXPORT __declspec(dllimport)
  #endif 
#else 
 #define ADDON_EXPORT
#endif

namespace addon
{

enum Type
{
  mainMenu=0,
  level,
  briefing
};

const unsigned int API_VERSION = 0x1001;

struct GameInfo
{
  //virtual void createPicture( const char* name, unsigned int width, unsigned int height, char* data ) = 0;
};

}

#endif // __CAESARIA_ADDON_REQUIREMENTS_INCLUDE_HPP__
