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

#ifndef __CAESARIA_REFERENCE_COUNTED_H_INCLUDED__
#define __CAESARIA_REFERENCE_COUNTED_H_INCLUDED__

#include <string>
#include "core/requirements.hpp"

class ReferenceCounted
{
public:
  ReferenceCounted() : _debugName( "" ), _referenceCounter(1)  {}
  virtual ~ReferenceCounted() {}

  //! Grabs the object. Increments the reference counter by one.
  void grab() const { ++_referenceCounter; }

  //! Drops the object. Decrements the reference counter by one.
  bool drop() const
  {
    // someone is doing bad reference counting.
    _CAESARIA_DEBUG_BREAK_IF( _referenceCounter <= 0 )

    --_referenceCounter;
    if (!_referenceCounter)
    {
      delete this;
      return true;
    }
    return false;
  }

  //! Get the reference count.
  int rcount() const{ return _referenceCounter; }

  //! Returns the debug name of the object.
  std::string debugName() const{ return _debugName; }
protected:

  //! Sets the debug name of the object.
  /** The Debugname may only be set and changed by the object
    itself. This method should only be used in Debug mode.
    \param newName: New debug name to set.
  */
  void setDebugName(const std::string& newName) { _debugName = newName; }

private:

  //! The debug name.
  std::string _debugName;

  //! The reference counter. Mutable to do reference counting on const objects.
  mutable int _referenceCounter;
};

#endif //__CAESARIA_REFERENCE_COUNTED_H_INCLUDED__

