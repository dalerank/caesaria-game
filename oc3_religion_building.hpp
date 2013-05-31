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


#ifndef __OPENCAESAR3_RELIGION_BIULDING_H_INCLUDED__
#define __OPENCAESAR3_RELIGION_BIULDING_H_INCLUDED__

#include "oc3_service_building.hpp"

class Temple : public ServiceBuilding
{
public:
  RomeDivinityPtr getDivinity() const;

protected:
  Temple( RomeDivinityPtr divinity, BuildingType type, int imgId, const Size& size );

private:
  class Impl;
  ScopedPtr< Impl > _td;
};

class SmallTemple : public Temple
{
protected:
  SmallTemple( RomeDivinityPtr divinity, BuildingType type, int imgId );
};

class BigTemple : public Temple
{
protected:
  BigTemple( RomeDivinityPtr divinity );
};

class TempleCeres : public SmallTemple
{
public:
  TempleCeres();
};

class TempleNeptune : public ServiceBuilding
{
public:
  TempleNeptune();
};

class TempleMars : public ServiceBuilding
{
public:
  TempleMars();
};

class TempleVenus : public ServiceBuilding
{
public:
  TempleVenus();
};

class TempleMercure : public ServiceBuilding
{
public:
  TempleMercure();
};

class BigTempleCeres : public ServiceBuilding
{
public:
  BigTempleCeres();
};

class BigTempleNeptune : public ServiceBuilding
{
public:
  BigTempleNeptune();
};

class BigTempleMars : public ServiceBuilding
{
public:
  BigTempleMars();
};

class BigTempleVenus : public ServiceBuilding
{
public:
  BigTempleVenus();
};

class BigTempleMercure : public ServiceBuilding
{
public:
  BigTempleMercure();
};

class TempleOracle : public ServiceBuilding
{
public:
  TempleOracle();
};


#endif