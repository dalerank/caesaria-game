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

#ifndef __CAESARIA_CITYSERVICE_MILITARY_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_MILITARY_H_INCLUDED__

#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "core/signals.hpp"

namespace city
{

class Military : public city::Srvc
{
public:  
  struct Notification
  {
    typedef enum { unknown, barbarian=0x1, chastener, attack } Type;

    DateTime date;
    Type type;
    std::string message;
    std::string objectName;
    Point location;

    VariantList save() const;
    void load(const VariantList &stream);
  };

  typedef std::vector< Notification > NotificationArray;

  static city::SrvcPtr create( PlayerCityPtr city );

  virtual void timeStep( const unsigned int time );

  void addNotification( const std::string& text, const std::string& name, Notification::Type type );

  Notification priorityNotification() const;
  const NotificationArray& notifications() const;

  bool haveNotification(Notification::Type type) const;
  bool isUnderAttack() const;

  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream);

  const DateTime& lastAttack() const;
  int monthFromLastAttack() const;

  world::PlayerArmyList expeditions() const;
  world::ObjectList enemies() const;

  void updateThreat( int value );
  unsigned int threatValue() const;

  static std::string defaultName();

private:
  Military(PlayerCityPtr city);

  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr<Military> MilitaryPtr;

}//end namespace city

#endif //__CAESARIA_CITYSERVICE_MILITARY_H_INCLUDED__
