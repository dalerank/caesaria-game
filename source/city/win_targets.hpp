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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_WIN_TARGETS_H_INCLUDED__
#define __CAESARIA_WIN_TARGETS_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "core/stringarray.hpp"

class VariantMap;

class CityWinTargets : public ReferenceCounted
{
public:
  CityWinTargets();
  ~CityWinTargets();

  int getCulture() const;
  int getProsperity() const;
  int getFavour() const;
  int getPeace() const;
  std::string getShortDesc() const;
  int getPopulation() const;
  const StringArray& getOverview() const;

  bool isSuccess( int culture, int prosperity,
                  int favour, int peace,
                  int population ) const;

  void load( const VariantMap& stream );

  CityWinTargets& operator=(const CityWinTargets& a);

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_WIN_TARGETS_H_INCLUDED__
