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

#ifndef __CAESARIA_WELL_H_INCLUDED__
#define __CAESARIA_WELL_H_INCLUDED__

#include "service.hpp"

class Well : public ServiceBuilding
{
public:
  Well();

  virtual void deliverService();
  virtual bool isNeedRoadAccess() const;
  virtual void burn();
  virtual bool build(const CityAreaInfo &info);
  virtual bool isDestructible() const;
  gfx::TilesArray coverageArea() const;
};

#endif //__CAESARIA_WELL_H_INCLUDED__
