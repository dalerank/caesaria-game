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

#ifndef __OPENCAESAR3_APPCONFIG_H_INCLUDED__
#define __OPENCAESAR3_APPCONFIG_H_INCLUDED__

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
  static const char* workDir;
  static const char* settingsPath;
  static const char* language;
  static const char* resolution;
  static const char* fullscreen;
  static const char* savedir;
  static const char* emigrantSalaryKoeff;

  static GameSettings& getInstance();

  static void set( const std::string& option, const Variant& value );
  static Variant get( const std::string& option );

  static vfs::Path rcpath( const std::string& option="" );

private:
  GameSettings();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_APPCONFIG_H_INCLUDED__
