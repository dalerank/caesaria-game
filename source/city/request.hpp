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

class CityRequest : public ReferenceCounted
{
public:
  virtual ~CityRequest();

  virtual bool mayExec( PlayerCityPtr city ) { return false; }
  virtual void exec( PlayerCityPtr city ) {}
  virtual void success( PlayerCityPtr ) { _isDeleted = true; }
  virtual void fail( PlayerCityPtr ) { _isDeleted = true; }
  virtual bool isDeleted() const { return _isDeleted; }
  virtual const DateTime& getFinishedDate() const { return _finishedDate; }

  virtual VariantMap save() const { return VariantMap(); }
  virtual void load( const VariantMap& stream ) {}

protected:
  bool _isDeleted;
  DateTime _finishedDate;

  CityRequest( DateTime finish ) :
    _isDeleted( false ), _finishedDate( finish )
  {

  }
};

class GoodRequest : public CityRequest
{
public:
  static CityRequestPtr create( const VariantMap& stream );

  virtual ~GoodRequest();
  virtual void exec( PlayerCityPtr city );
  virtual bool mayExec( PlayerCityPtr city ) const;

  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream );

  virtual void success( PlayerCityPtr city );
  virtual void fail( PlayerCityPtr city );

  int getQty() const;
  Good::Type getGoodType() const;
  int getMonths2Comply() const;

  static std::string typeName();
private:
  GoodRequest();

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //__CAESARIA_CITY_REQUEST_H_INCLUDED__
