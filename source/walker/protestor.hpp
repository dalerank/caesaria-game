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

#ifndef __CAESARIA_PROTESTOR_H_INCLUDE_
#define __CAESARIA_PROTESTOR_H_INCLUDE_

#include "walker.hpp"

class Protestor : public Walker
{
public:
  static ProtestorPtr create( PlayerCityPtr city );
  virtual ~Protestor();

  virtual void timeStep(const unsigned long time);
  void send2City( HousePtr house );

  virtual void die();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual int agressive() const;

protected:
  virtual void _reachedPathway();
  virtual void _updateThinks();

private:
  Protestor( PlayerCityPtr city );

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif//__CAESARIA_PROTESTOR_H_INCLUDE_
