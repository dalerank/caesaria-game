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

#ifndef __CAESARIA_BUILD_OPTIONS_H_INCLUDED__
#define __CAESARIA_BUILD_OPTIONS_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "objects/overlay.hpp"

namespace city
{  

namespace development
{

typedef enum
{
  unknown,
  water,
  health,
  security,
  education,
  engineering,
  administration,
  entertainment,
  commerce,
  farm,
  raw_material,
  factory,
  religion,
  temple,
  big_temple,
  all
} Branch;

Branch toBranch( const std::string& name );
std::string toString( Branch branch );
void loadBranchOptions( const std::string& filename );

class Range : public object::TypeSet
{
public:
  static Range fromBranch( const Branch branch);
  static Range fromSequence( const object::Type start, const object::Type stop );
  Range& operator<<( const object::Type type );
};

class Options : public ReferenceCounted
{
public:
  Options();
  virtual ~Options();

  void setBuildingAvailble( const object::Type type, bool mayBuild );
  void setGroupAvailable(const Branch type, Variant mayBuild );
  bool isGroupAvailable(const Branch type ) const;
  unsigned int getBuildingsQuote( const object::Type type ) const;

  bool isBuildingAvailble( const object::Type type ) const;

  void clear();
  void load( const VariantMap& options );
  VariantMap save() const;

  Options& operator=(const Options& a);

  void setBuildingAvailble(const Range& range, bool mayBuild);
  bool isBuildingsAvailble(const Range& range) const;
  bool isCheckDesirability() const;
  unsigned int maximumForts() const;

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace development

}//end namespace city
#endif //__CAESARIA_BUILD_OPTIONS_H_INCLUDED__
