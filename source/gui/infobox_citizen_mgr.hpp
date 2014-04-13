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

class InfoboxManager;

class InfoboxCitizenCreator : public ReferenceCounted
{
public:
  virtual gui::InfoboxSimple* create( gui::Widget* parent, WalkerList walkers ) = 0;
};

typedef SmartPtr<InfoboxCitizenCreator> InfoboxCitizenCreatorPtr;

class InfoboxCitizenManager
{
public:
  static InfoboxCitizenManager& instance();

  void loadInfoboxes( InfoboxManager& manager );
  virtual ~InfoboxCitizenManager();

  void addCreator( constants::walker::Type type, InfoboxCitizenCreatorPtr c );

  gui::InfoboxSimple* show( gui::Widget* parent, WalkerList walkers );
private:
  InfoboxCitizenManager();

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_WINDOW_GAMESPEED_OPTIONS_H_INCLUDE_
