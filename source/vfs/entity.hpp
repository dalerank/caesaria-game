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


#ifndef __CAESARIA_FSENTITY_H_INCLUDED__
#define __CAESARIA_FSENTITY_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/bytearray.hpp"
#include "core/smartptr.hpp"

namespace vfs
{

class Path;

class Entity : public ReferenceCounted
{
public:
  typedef enum { fmRead=0x0, fmWrite=0x1, fmAppend=0x2 } Mode;
	
	virtual ~Entity() {}

  //! returns how much was read
  virtual int read(void* buffer, unsigned int sizeToRead) = 0;

  virtual ByteArray readLine() = 0;

  virtual ByteArray read( unsigned int sizeToRead ) = 0;

  virtual int write( const void* buffer, unsigned int sizeToWrite) = 0;

  virtual int write( const ByteArray& bArray ) = 0;

  //! changes position in file, returns true if successful
  virtual bool seek(long finalPos, bool relativeMovement = false) = 0;

  //! returns size of file
  virtual long size() const = 0;

  //! returns if file is open
  virtual bool isOpen() const = 0;

  //! returns where in the file we are.
  virtual long getPos() const = 0;

  //! returns name of file
  virtual const Path& path() const = 0;

  virtual bool isEof() const = 0;

  virtual void flush() = 0;

  virtual size_t lastModify() const = 0;
};

typedef SmartPtr< Entity > FSEntityPtr;

} //end namespace io

#endif //__CAESARIA_FSENTITY_H_INCLUDED__

