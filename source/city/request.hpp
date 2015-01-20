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

#ifndef __CAESARIA_CITY_REQUEST_H_INCLUDED__
#define __CAESARIA_CITY_REQUEST_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/time.hpp"
#include "predefinitions.hpp"
#include "core/variant.hpp"
#include "good/good.hpp"

namespace city
{

namespace request
{

class Request : public ReferenceCounted
{
public:
  virtual ~Request() {}

  virtual bool isReady( PlayerCityPtr ) const { return false; }
  virtual void exec( PlayerCityPtr ) {}
  virtual void success( PlayerCityPtr ) { _isDeleted = true; }
  virtual void fail( PlayerCityPtr ) { _isDeleted = true; }
  virtual bool isDeleted() const { return _isDeleted; }
  virtual DateTime startDate() const { return _startDate; }
  virtual DateTime finishedDate() const { return _finishDate; }
  virtual bool isAnnounced() const { return _isAnnounced; }
  virtual void update() {}
  virtual void setAnnounced( bool value ) { _isAnnounced = value; }

  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );
  virtual std::string description() const{  return ""; }

protected:
  bool _isDeleted, _isAnnounced;
  DateTime _finishDate, _startDate;

  Request( DateTime finish );
};

class RqGood : public Request
{
public:
  static RequestPtr create( const VariantMap& stream );

  virtual ~RqGood();
  virtual void exec( PlayerCityPtr city );
  virtual bool isReady( PlayerCityPtr city ) const;

  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream );

  virtual void success( PlayerCityPtr city );
  virtual void fail( PlayerCityPtr city );

  virtual void update();

  int qty() const;
  good::Product goodType() const;
  virtual std::string description() const;

  static std::string typeName();
private:
  RqGood();

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace request

}//end namespace city

#endif //__CAESARIA_CITY_REQUEST_H_INCLUDED__
