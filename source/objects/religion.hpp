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

#ifndef __CAESARIA_RELIGION_BUILDING_H_INCLUDED__
#define __CAESARIA_RELIGION_BUILDING_H_INCLUDED__

#include "service.hpp"
#include "religion/romedivinity.hpp"

class Temple : public ServiceBuilding
{
public:
  virtual ~Temple();

  religion::DivinityPtr divinity() const;

  virtual unsigned int parishionerNumber() const = 0;
  virtual void deliverService();

protected:
  Temple( religion::DivinityPtr divinity, TileOverlay::Type type, int imgId, const Size& size );

  virtual unsigned int walkerDistance() const;

private:
  class Impl;
  ScopedPtr< Impl > _td;
};

class SmallTemple : public Temple
{
protected:
  SmallTemple( religion::DivinityPtr divinity, TileOverlay::Type type, int imgId );
  virtual unsigned int parishionerNumber() const;
};

class BigTemple : public Temple
{
protected:
  BigTemple( religion::DivinityPtr divinity, TileOverlay::Type type, int imgId );
  virtual unsigned int parishionerNumber() const;

  virtual bool build(const CityAreaInfo &info);
};

class TempleCeres : public SmallTemple
{
public:
  TempleCeres();
};

class TempleNeptune : public SmallTemple
{
public:
  TempleNeptune();
};

class TempleMars : public SmallTemple
{
public:
  TempleMars();
};

class TempleVenus : public SmallTemple
{
public:
  TempleVenus();
};

class TempleMercury : public SmallTemple
{
public:
  TempleMercury();
};

class BigTempleCeres : public BigTemple
{
public:
  BigTempleCeres();
};

class BigTempleNeptune : public BigTemple
{
public:
  BigTempleNeptune();
};

class BigTempleMars : public BigTemple
{
public:
  BigTempleMars();
};

class BigTempleVenus : public BigTemple
{
public:
  BigTempleVenus();
};

class BigTempleMercury : public BigTemple
{
public:
  BigTempleMercury();
};

class TempleOracle : public BigTemple
{
public:
  TempleOracle();
  virtual unsigned int parishionerNumber() const;

  virtual bool build(const CityAreaInfo &info);
};


#endif
