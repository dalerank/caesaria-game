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

#include "oc3_app_config.hpp"

const std::string AppConfig::resourcePath = "resourcePath";
const std::string AppConfig::pantheonModel = "pantheonConfig";
const std::string AppConfig::houseModel = "houseModel";
const std::string AppConfig::constructionModel = "constructionModel";

class AppConfig::Impl
{
public:
  VariantMap options;
};

AppConfig& AppConfig::getInstance()
{
  static AppConfig inst;
  return inst;
}

AppConfig::AppConfig() : _d( new Impl )
{
  _d->options[ resourcePath ] = Variant( std::string( "./resources" ) );
  _d->options[ pantheonModel ] = Variant( std::string( "/pantheon.model" ) );
  _d->options[ houseModel ] = Variant( std::string( "/house.model" ) );
  _d->options[ constructionModel ] = Variant( std::string( "/construction.model" ) );
}

void AppConfig::set( const std::string& option, const Variant& value )
{
  getInstance()._d->options[ option ] = value;
}

Variant AppConfig::get( const std::string& option )
{
  return getInstance()._d->options[ option ];
}