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

#ifndef __CAESARIA_DIVINITIES_H_INCLUDED__
#define __CAESARIA_DIVINITIES_H_INCLUDED__

#include "romedivinity.hpp"
#include "gfx/picture.hpp"
#include "game/service.hpp"

#define DIVINITY_MUST_INITIALIZE_FROM_PANTHEON friend class RomeDivinity;

namespace religion
{

namespace rome
{
  class Pantheon;
}

class RomeDivinity : public Divinity
{
  friend class rome::Pantheon;
public:
  typedef enum
  {
    Ceres = 0,
    Mars,
    Neptune,
    Venus,
    Mercury,

    Count=0xff
  } Type;

  static std::string findIntName( Type type );
  static StringArray getIntNames();
  static std::vector<RomeDivinity::Type> getIntTypes();

  virtual VariantMap save() const;
  virtual void load( const VariantMap& vm );

  virtual std::string name() const { return _name; }
  virtual std::string shortDescription() const;
  virtual Service::Type serviceType() const;
  virtual const gfx::Picture& picture() const;
  virtual float relation() const;
  virtual float monthDecrease() const;
  virtual void setEffectPoint( int value );
  virtual int wrathPoints() const;
  virtual object::Type templeType( TempleSize size ) const;
  virtual DateTime lastFestivalDate() const;
  virtual void setInternalName(const std::string &newName);
  virtual void updateRelation( float income, PlayerCityPtr city );
  virtual std::string internalName() const;
  virtual std::string moodDescription() const;
  virtual void checkAction(PlayerCityPtr city);

  RomeDivinity::Type dtype() const;
  void assignFestival( int type );

protected:
  template<typename ObjClass, typename... Args>
  static SmartPtr<ObjClass> create( const Args & ... args)
  {
    SmartPtr<ObjClass> instance( new ObjClass( args... ) );
    instance->setInternalName( findIntName( instance->dtype() ) );
    instance->drop();

    return instance;
  }

  RomeDivinity( Type type=RomeDivinity::Count );

  virtual void _doBlessing( PlayerCityPtr city ) {}
  virtual void _doWrath( PlayerCityPtr city ) {}
  virtual void _doSmallCurse( PlayerCityPtr city ) {}

  std::string _name;
  Service::Type _service;
  std::string _shortDesc;
  DateTime _lastFestival;
  bool _blessingDone;
  bool _smallCurseDone;
  int _wrathPoints;
  RomeDivinity::Type _dtype;
  int _effectPoints;
  gfx::Picture _pic;
  StringArray _moodDescr;

  struct
  {
    float current;
    float target;
  } _relation;
};

}//end namespace religion

#endif //__CAESARIA_DIVINITIES_H_INCLUDED__
