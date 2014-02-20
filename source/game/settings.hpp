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

#ifndef __CAESARIA_APPCONFIG_H_INCLUDED__
#define __CAESARIA_APPCONFIG_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "vfs/path.hpp"

class GameSettings
{
public:
  static const char* ctNamesModel;
  static const char* localePath;
  static const char* resourcePath;
  static const char* pantheonModel;
  static const char* houseModel;
  static const char* citiesModel;
  static const char* constructionModel;
  static const char* settingsPath;
  static const char* language;
  static const char* resolution;
  static const char* fullscreen;
  static const char* savedir;
  static const char* emigrantSalaryKoeff;
  static const char* minMonthWithFood;
  static const char* worldModel;
  static const char* worklessCitizenAway;
  static const char* langModel;
  static const char* fastsavePostfix;
  static const char* saveExt;
  static const char* workDir;
  static const char* adviserEnabled;
  static const char* lastTranslation;

  static GameSettings& getInstance();

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
