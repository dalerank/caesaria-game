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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_APPCONFIG_H_INCLUDED__
#define __CAESARIA_APPCONFIG_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "vfs/path.hpp"

class GameSettings
{
public:
#define __GS_PROPERTY(a) static const char* a;
  __GS_PROPERTY(ctNamesModel)
  __GS_PROPERTY(localePath)
  __GS_PROPERTY(resourcePath)
  __GS_PROPERTY(pantheonModel)
  __GS_PROPERTY(houseModel)
  __GS_PROPERTY(citiesModel)
  __GS_PROPERTY(constructionModel)
  __GS_PROPERTY(settingsPath)
  __GS_PROPERTY(language)
  __GS_PROPERTY(resolution)
  __GS_PROPERTY(fullscreen)
  __GS_PROPERTY(savedir)
  __GS_PROPERTY(emigrantSalaryKoeff)
  __GS_PROPERTY(minMonthWithFood)
  __GS_PROPERTY(worldModel)
  __GS_PROPERTY(worklessCitizenAway)
  __GS_PROPERTY(langModel)
  __GS_PROPERTY(fastsavePostfix)
  __GS_PROPERTY(saveExt)
  __GS_PROPERTY(workDir)
  __GS_PROPERTY(lastTranslation)
  __GS_PROPERTY(c3gfx)
  __GS_PROPERTY(archivesModel)
  __GS_PROPERTY(soundThemesModel)
  __GS_PROPERTY(soundVolume)
  __GS_PROPERTY(ambientVolume)
  __GS_PROPERTY(musicVolume)
  __GS_PROPERTY(animationsModel)
  __GS_PROPERTY(walkerModel)
  __GS_PROPERTY(giftsModel)
  __GS_PROPERTY(emblemsModel)
  __GS_PROPERTY(testArchive)
  __GS_PROPERTY(screenFitted)
  __GS_PROPERTY(needAcceptBuild)
  __GS_PROPERTY(sg2model)
  __GS_PROPERTY(ranksModel)
  __GS_PROPERTY(autosaveInterval)
  __GS_PROPERTY(talksArchive)
  __GS_PROPERTY(rectuterDistance)
#undef __GS_PROPERTY

  static GameSettings& instance();

  static void set( const std::string& option, const Variant& value );
  static Variant get( const std::string& option );

  static vfs::Path rcpath( const std::string& option="" );
  static vfs::Path rpath( const std::string& option="" );

  static void load();
  static void save();

  void setwdir( const std::string& wdir );
private:
  GameSettings();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_APPCONFIG_H_INCLUDED__
