// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.


#ifndef __OPENCAESAR3_FILE_H_INCLUDED__
#define __OPENCAESAR3_FILE_H_INCLUDED__

#include "oc3_filesystem_entity.hpp"

namespace io
{

class NFile : public FSEntity
{
public:	
  NFile( FSEntityPtr file );

  NFile();

  ~NFile();

  //! returns how much was read
  int read(void* buffer, unsigned int sizeToRead);

  ByteArray readLine();

  ByteArray read( unsigned int sizeToRead );

  int write(const void* buffer, unsigned int sizeToWrite);

  int write( const ByteArray& bArray );

  //! changes position in file, returns true if successful
  bool seek(long finalPos, bool relativeMovement = false);

  //! returns size of file
  long getSize() const;

  //! returns if file is open
  bool isOpen() const;

  //! returns where in the file we are.
  long getPos() const;

  //! returns name of file
  const FilePath& getFileName() const;

  bool isEof() const;

  void flush();

  NFile& operator=(const NFile& other );

  static NFile open( const FilePath& fileName, FSEntity::Mode mode=FSEntity::fmRead );

private:
  FSEntityPtr _entity;
};

} //end namesapce io
#endif

