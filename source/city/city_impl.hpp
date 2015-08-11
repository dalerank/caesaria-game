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

#ifndef __CAESARIA_CITYIMPL_H_INCLUDED__
#define __CAESARIA_CITYIMPL_H_INCLUDED__

#include "predefinitions.hpp"
#include "objects/predefinitions.hpp"
#include "core/flowlist.hpp"
#include "walkergrid.hpp"

namespace city
{

/** Helper class for player city */
class Services : public city::SrvcList
{
public:
  /** Call every frame */
  void timeStep( PlayerCityPtr city, unsigned int time);
  void initialize( PlayerCityPtr city, const std::string& model );
};

/**  */
class Overlays : public FlowList<Overlay>
{
public:
  void update( PlayerCityPtr city, unsigned int time );

  void onDestroyOverlay( PlayerCityPtr city, OverlayPtr overlay );
};

class Walkers : public FlowList<Walker>
{
public:
  //walkers fast access map !!!
  WalkersGrid grid;
  //*********************** !!!

  unsigned int idCount;

  void postpone( WalkerPtr w );

  void clear();

  inline const WalkerList& at( const TilePos& pos ) { return grid.at( pos ); }

  VariantMap save() const;

  void update( PlayerCityPtr, unsigned int time );
};

class Options : public std::map<int, int>
{
public:
  VariantList save() const;
  void load(const VariantList &stream );
  void resetIfNot( int name, int value );
};

}//end namespace city

#endif //__CAESARIA_CITYIMPL_H_INCLUDED__
