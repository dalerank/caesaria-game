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

#ifndef __CAESARIA_CONSTRUCTION_EXTENSION_H_INCLUDED__
#define __CAESARIA_CONSTRUCTION_EXTENSION_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/variant_map.hpp"
#include "construction.hpp"

class ConstructionExtension : public ReferenceCounted
{
public:
  virtual void save( VariantMap& stream ) const;
  virtual void load( const VariantMap& stream );
  virtual void timeStep( ConstructionPtr parent, unsigned int time );
  virtual bool isDeleted() const { return _isDeleted; }
  virtual std::string type() const = 0;
  virtual void destroy( ConstructionPtr parent ) {}

protected:
  ConstructionExtension() : _isDeleted( false )
  {}

  bool _isDeleted;
  DateTime _finishDate;
  VariantMap _options;
};

class WarehouseBuff : public ConstructionExtension
{
public:
  static ConstructionExtensionPtr create();
  static ConstructionExtensionPtr assignTo(WarehousePtr warehouse, int type, float value, int week2finish );

  virtual void timeStep( ConstructionPtr parent, unsigned int time );
  virtual std::string type() const;

  float value() const;
  int group() const;
private:
  WarehouseBuff();
};

class FactoryProgressUpdater : public ConstructionExtension
{
public:
  static ConstructionExtensionPtr create();
  static ConstructionExtensionPtr assignTo(FactoryPtr factory, float value, int week2finish );

  virtual void timeStep( ConstructionPtr parent, unsigned int time );
  virtual std::string type() const;
private:
  FactoryProgressUpdater();
};

class ConstructionParamUpdater : public ConstructionExtension
{
public:
  static ConstructionExtensionPtr create();
  static ConstructionExtensionPtr assignTo(ConstructionPtr construction, int paramName,  bool relative, int value, int week2finish );

  virtual void timeStep(ConstructionPtr parent, unsigned int time);
  virtual std::string type() const;
  virtual void destroy(ConstructionPtr parent);

private:
  ConstructionParamUpdater();
};

class FortCurseByMars : public ConstructionExtension
{
public:
  static ConstructionExtensionPtr create();
  static ConstructionExtensionPtr assignTo( FortPtr fort, unsigned int monthsCurse );

  virtual void timeStep( ConstructionPtr parent, unsigned int time );
  virtual std::string type() const;
private:
  FortCurseByMars();
};

class ExtensionsFactory
{
public:
  virtual ~ExtensionsFactory();
  static ExtensionsFactory& instance();
  static ConstructionExtensionPtr create(std::string type);
  static ConstructionExtensionPtr create(const VariantMap& stream);

private:
  ExtensionsFactory();

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //__CAESARIA_CONSTRUCTION_EXTENSION_H_INCLUDED__
