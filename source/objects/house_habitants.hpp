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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_HOUSE_HABITANTS_H_INCLUDED__
#define __CAESARIA_HOUSE_HABITANTS_H_INCLUDED__

#include "game/citizen_group.hpp"

class House;

class Habitants : public CitizenGroup
{
public:
  unsigned int capacity;

  struct {
    unsigned int current;
    unsigned int max;
  } workers;

  int homeless() const;
  int freeRoom() const;

  Habitants();

  void makeGeneration( House& h );
  void update(House& h, const CitizenGroup& group);
  void updateCapacity( House& h );

  VariantMap save() const;
  void load( const VariantMap& stream );
};

class RecruterService
{
public:
  RecruterService( Habitants& habitants ) : _habitants( habitants ) {}
  RecruterService( const RecruterService& other ) : _habitants( other._habitants ) {}

  void set( float i );
  float value() const;

  int max() const;
  void setMax( int value );

private:
  Habitants& _habitants;
};

#endif //__CAESARIA_HOUSE_HABITANTS_H_INCLUDED__
