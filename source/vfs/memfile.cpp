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


#include "memfile.hpp"
#include "core/logger.hpp"

namespace vfs
{

MemoryFile::MemoryFile()
{
    Buffer = 0;
    Len = 0;
    _pos = 0;
    deleteMemoryWhenDropped = false;

#ifdef _DEBUG
        setDebugName("MemoryFile");
#endif

}

NFile MemoryFile::create(void* memory, long len, const Path& fileName, bool d)
{
    MemoryFile* mf = new MemoryFile();
    mf->Buffer = memory;
    mf->Len  = len;
    mf->_pos = 0;
    mf->Filename = fileName;
    mf->deleteMemoryWhenDropped = d;

    FSEntityPtr ret( mf );
    ret->drop();

    return NFile( ret );
}

NFile MemoryFile::create( ByteArray data, const Path& fileName )
{
    MemoryFile* mf = new MemoryFile();
    mf->Buffer = new char[ data.size() ];
    memcpy( mf->Buffer, data.data(), data.size() );
    mf->Len  = data.size();
    mf->_pos = 0;
    mf->Filename = fileName;
    mf->deleteMemoryWhenDropped = true;

    FSEntityPtr ret( mf );
    ret->drop();

    return NFile( ret );
}

MemoryFile::~MemoryFile()
{
	if (deleteMemoryWhenDropped)
        delete [] (char*)Buffer;
}


//! returns how much was read
int MemoryFile::read(void* buffer, unsigned int sizeToRead)
{
  int amount = static_cast<int>(sizeToRead);
  if( _pos + amount > Len )
  {
    amount -= _pos + amount - Len;
  }

  if( amount <= 0 )
  {
    return 0;
  }

  char* p = (char*)Buffer;
  memcpy(buffer, p + _pos, amount);

  _pos += amount;
  return amount;
}

ByteArray MemoryFile::readLine()
{
    if (!isOpen())
    {
      return ByteArray();
    }

    ByteArray ret;

    bool endLineReaded = false;
    unsigned int readOneLineCounter = 0;
    while( !endLineReaded )
    {
        readOneLineCounter++;

        int idx = _pos;
        while( idx < 100 )
        {
          if( ((char*)Buffer)[ idx ] == '\n'  )
          {
            ret.push_back( '\0' );
            return ret;
          }

          ret.push_back( ((char*)Buffer)[ idx ] );
          idx++;
        }

        int reallyReadingBytes = idx - _pos;

        if( reallyReadingBytes <= 0 )
            return ByteArray();

        if( readOneLineCounter > 1000 )
        {
            Logger::warning( "Too many iteration for read one line" );
            return ByteArray();
        }
    }

    return ret;
}

//! returns how much was written
int MemoryFile::write(const void* buffer, unsigned int sizeToWrite)
{
	int amount = static_cast<int>(sizeToWrite);
	if (_pos + amount > Len)
		amount -= _pos + amount - Len;

	if (amount <= 0)
		return 0;

	char* p = (char*)Buffer;
	memcpy(p + _pos, buffer, amount);

	_pos += amount;

	return amount;
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool MemoryFile::seek(long finalPos, bool relativeMovement)
{
	if (relativeMovement)
	{
		if (_pos + finalPos > Len)
			return false;

		_pos += finalPos;
	}
	else
	{
		if (finalPos > Len)
			return false;

		_pos = finalPos;
	}

	return true;
}


//! returns size of file
long MemoryFile::size() const
{
    return Len;
}

bool MemoryFile::isOpen() const
{
    return size() != 0;
}

int MemoryFile::write(const ByteArray &array )
{
    int savePos = _pos;
    write( array.data(), array.size() );
    return _pos - savePos;
}

ByteArray MemoryFile::read(unsigned int sizeToRead)
{
    ByteArray ret;
    ret.resize( sizeToRead );
    int bytesRead = read( ret.data(), sizeToRead );

    ret.resize( bytesRead );

    return ret;
}

//! returns where in the file we are.
long MemoryFile::getPos() const
{
    return _pos;
}

void MemoryFile::flush()
{
}

bool MemoryFile::isEof() const
{
		return _pos >= Len;
}

//! returns name of file
const Path& MemoryFile::path() const
{
	return Filename;
}

} //end namespace io
