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

#ifndef __CAESARIA_NATIVEBUILDINGS_H_INCLUDED__
#define __CAESARIA_NATIVEBUILDINGS_H_INCLUDED__

#include "building.hpp"

class NativeBuilding : public Building
{
public:
  NativeBuilding( const TileOverlay::Type type, const Size& size );
  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

  void build(PlayerCityPtr city, const TilePos& pos );
};

class NativeHut : public NativeBuilding
{
public:
  NativeHut();
  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);
  //virtual GuiInfoBox* makeInfoBox();  
};

class NativeField  : public NativeBuilding
{
public:
  NativeField();
  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);
  //virtual GuiInfoBox* makeInfoBox();
};

class NativeCenter : public NativeBuilding
{
public:
  NativeCenter();
  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);
  //virtual GuiInfoBox* makeInfoBox();
};


#endif //__CAESARIA_NATIVEBUILDINGS_H_INCLUDED__
