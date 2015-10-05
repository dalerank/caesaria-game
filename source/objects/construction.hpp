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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_CONSTRUCTION_H_INCLUDE_
#define _CAESARIA_CONSTRUCTION_H_INCLUDE_

#include "objects/overlay.hpp"
#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "predefinitions.hpp"
#include "game/service.hpp"
#include "objects/metadata.hpp"
#include "gfx/tilesarray.hpp"

class Construction : public Overlay
{
public:
  virtual ~Construction();

  virtual bool canBuild( const city::AreaInfo& areaInfo ) const;  // returns true if it can be built there
  virtual std::string troubleDesc() const;
  virtual bool build( const city::AreaInfo& info );
  virtual void burn();
  virtual void collapse();
  virtual const gfx::Picture& picture() const;
  virtual bool isNeedRoad() const;
  virtual const gfx::TilesArray& roadside() const;  // return all road tiles adjacent to the construction
  virtual void computeRoadside();
  virtual int  roadsideDistance() const; // virtual because HOUSE has different behavior
  virtual gfx::TilesArray enterArea() const;

  virtual void destroy();

  virtual void updateState( Param name, double value );
  virtual void setState( Param name, double value );
  virtual double state( Param name ) const;

  virtual void timeStep(const unsigned long time);
  virtual const gfx::Picture& picture( const city::AreaInfo& areaInfo ) const;

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

  virtual void addExtension( ConstructionExtensionPtr ext );
  ConstructionExtensionPtr getExtension( const std::string& name );
  virtual const ConstructionExtensionList& extensions() const;  

  virtual void initialize(const MetaData &mdata);
protected:
  Construction( const object::Type type, const Size& size );
  gfx::TilesArray& _roadside();
  void _checkDestroyState();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif//_CAESARIA_CONSTRUCTION_H_INCLUDE_
