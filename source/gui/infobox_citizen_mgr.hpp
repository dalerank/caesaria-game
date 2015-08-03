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


#ifndef _CAESARIA_INFOBOX_CITIZEN_MGR_H_INCLUDE_
#define _CAESARIA_INFOBOX_CITIZEN_MGR_H_INCLUDE_

#include "info_box.hpp"
#include "walker/predefinitions.hpp"
#include "walker/constants.hpp"

namespace gui
{

namespace infobox
{

class Manager;

namespace citizen
{

class CzInfoboxCreator : public ReferenceCounted
{
public:
  virtual Infobox* create( gui::Widget* parent, PlayerCityPtr city, const TilePos& pos ) = 0;
};

template< class T >
class CzBaseCreator : public CzInfoboxCreator
{
public:
  Infobox* create( gui::Widget* parent, PlayerCityPtr city, const TilePos& pos )
  {
    return new T( parent, city, pos );
  }
};

typedef SmartPtr<CzInfoboxCreator> CzInfoboxCreatorPtr;

class PManager
{
public:
  static PManager& instance();

  virtual ~PManager();

  void addCreator( walker::Type type, CzInfoboxCreatorPtr c );
  void loadInfoboxes();

  Infobox* show( gui::Widget* parent, PlayerCityPtr city , const TilePos& pos);
private:
  PManager();

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace citizen

}//end namespace infobox

}//end namespace gui

#define REGISTER_CITIZEN_INFOBOX(name,a) \
namespace { \
struct Registrator_##name { Registrator_##name() { PManager::instance().addCreator( walker::name, new CzBaseCreator<a>() ); }}; \
static Registrator_##name rtor_##name; \
}
#endif //_CAESARIA_WINDOW_GAMESPEED_OPTIONS_H_INCLUDE_
