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

#ifndef __CAESARIA_HIPPODROME_H_INCLUDED__
#define __CAESARIA_HIPPODROME_H_INCLUDED__

#include "entertainment.hpp"
#include "core/direction.hpp"

class HippodromeSection : public Building
{
public:
  typedef enum { middle, ended } Type;
  HippodromeSection( Hippodrome& base, constants::Direction direction, Type type );

  virtual ~HippodromeSection();
  virtual void destroy();

  void setAnimationVisible( bool visible );
private:
  TilePos _basepos;
  constants::Direction _direction;
  Type _type;
};
typedef SmartPtr<HippodromeSection> HippodromeSectionPtr;

class Hippodrome : public EntertainmentBuilding
{
public:
  Hippodrome();
  virtual std::string troubleDesc() const;
  virtual bool canBuild( const CityAreaInfo& areaInfo ) const;
  virtual void deliverService();
  virtual bool build(const CityAreaInfo &info);
  constants::Direction direction() const;
  virtual void timeStep(const unsigned long time);
  virtual const gfx::Pictures& pictures( gfx::Renderer::Pass pass ) const;
  virtual void destroy();

  bool isRacesCarry() const;
  ~Hippodrome();
protected:
  virtual WalkerList _specificWorkers() const;

private:
  void _init(bool onBuild=false);
  HippodromeSectionPtr _addSection( HippodromeSection::Type type, TilePos offset );
  void _checkDirection( const CityAreaInfo& areaInfo );

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //__CAESARIA_BUILDING_ENTERTAINMENT_H_INCLUDED__
