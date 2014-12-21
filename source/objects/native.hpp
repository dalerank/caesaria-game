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
  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual bool build( const CityAreaInfo& info );
  virtual bool canDestroy() const;
};

class NativeHut : public NativeBuilding
{
public:
  NativeHut();
  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void timeStep(const unsigned long time);
  virtual void applyService( ServiceWalkerPtr walker);
  virtual float evaluateService(ServiceWalkerPtr walker);

  float discontent() const;

protected:
  float _discontent;
  int _day2look;
};

class NativeField  : public NativeBuilding
{
public:
  NativeField();
  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void timeStep(const unsigned long time);
  unsigned int progress() const;

  unsigned int catchCrops();

private:
  void _updatePicture();

  unsigned int _progress;

};

class NativeCenter : public NativeBuilding
{
public:
  NativeCenter();
  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  void store( unsigned int qty );
};


#endif //__CAESARIA_NATIVEBUILDINGS_H_INCLUDED__
