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

namespace city
{

class VictoryConditions : public ReferenceCounted
{
public:
  VictoryConditions();
  ~VictoryConditions();

  int needCulture() const;
  int needProsperity() const;
  int needFavour() const;
  int needPeace() const;
  const DateTime& finishDate() const;
  std::string shortDesc() const;
  std::string nextMission() const;
  std::string newTitle() const;
  std::string winText() const;
  int needPopulation() const;
  const StringArray& overview() const;

  void addReignYears( int value );
  void decreaseReignYear();

  bool isSuccess( int culture, int prosperity,
                  int favour, int peace,
                  int population ) const;

  bool isSuccess() const;
  bool mayContinue() const;

  void load( const VariantMap& stream );
  VariantMap save() const;

  VictoryConditions& operator=(const VictoryConditions& a);

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace city

#endif //__CAESARIA_WIN_TARGETS_H_INCLUDED__
