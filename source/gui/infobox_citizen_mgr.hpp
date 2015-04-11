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

class Creator : public ReferenceCounted
{
public:
  virtual gui::infobox::Infobox* create( gui::Widget* parent, PlayerCityPtr city, const TilePos& pos ) = 0;
};

typedef SmartPtr<Creator> CreatorPtr;

class PManager
{
public:
  static PManager& instance();

  virtual ~PManager();

  void addCreator( walker::Type type, CreatorPtr c );
  void loadInfoboxes();

  gui::infobox::Infobox* show( gui::Widget* parent, PlayerCityPtr city , const TilePos& pos);
private:
  PManager();

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace citizen

}//end namespace infobox

}//end namespace gui
#endif //_CAESARIA_WINDOW_GAMESPEED_OPTIONS_H_INCLUDE_
