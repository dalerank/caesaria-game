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


#ifndef __CAESARIA_MEMORY_FILE_H_INCLUDED__
#define __CAESARIA_MEMORY_FILE_H_INCLUDED__

#include "file.hpp"
#include "filepath.hpp"
#include "core/bytearray.hpp"

namespace vfs
{

/*!
        Class for reading and writing from memory.
*/
class MemoryFile : public Entity
{
public:
  //! Destructor
  virtual ~MemoryFile();

  //! returns how much was read
  virtual int read(void* buffer, unsigned int sizeToRead);

  virtual ByteArray readLine();

  //! returns how much was written
  virtual int write(const void* buffer, unsigned int sizeToWrite);

  //! changes position in file, returns true if successful
  virtual bool seek(long finalPos, bool relativeMovement = false);

  //! returns size of file
  virtual long getSize() const;

  virtual bool isOpen() const;

  virtual int write(const ByteArray& array);

  virtual ByteArray read(unsigned int sizeToRead);

  //! returns where in the file we are.
  virtual long getPos() const;

  virtual void flush();

  virtual bool isEof() const;

  //! returns name of file
  virtual const Path& getFileName() const;

  static NFile create( void* memory, long len, const Path& fileName, bool deleteMemoryWhenDropped );
  static NFile create( ByteArray data, const Path& fileName );

private:
  MemoryFile();

  void *Buffer;
  long Len;
  long Pos;
  Path Filename;
  bool deleteMemoryWhenDropped;
};

} //end namespace io


#endif

