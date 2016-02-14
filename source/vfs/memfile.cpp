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
  _buffer = 0;
  _length = 0;
  _pos = 0;
  _deleteMemoryWhenDropped = false;

#ifdef _DEBUG
  setDebugName("MemoryFile");
#endif
}

NFile MemoryFile::create(void* memory, long len, const Path& fileName, bool d)
{
  MemoryFile* mf = new MemoryFile();
  mf->_buffer = memory;
  mf->_length  = len;
  mf->_pos = 0;
  mf->_filename = fileName;
  mf->_deleteMemoryWhenDropped = d;

  FSEntityPtr ret( mf );
  ret->drop();

  return NFile( ret );
}

NFile MemoryFile::create( ByteArray data, const Path& fileName )
{
  MemoryFile* mf = new MemoryFile();
  mf->_buffer = new char[ data.size() ];
  memcpy( mf->_buffer, data.data(), data.size() );
  mf->_length  = data.size();
  mf->_pos = 0;
  mf->_filename = fileName;
  mf->_deleteMemoryWhenDropped = true;

  FSEntityPtr ret( mf );
  ret->drop();

  return NFile( ret );
}

MemoryFile::~MemoryFile()
{
  if (_deleteMemoryWhenDropped)
  delete [] (char*)_buffer;
}


//! returns how much was read
int MemoryFile::read(void* buffer, unsigned int sizeToRead)
{
  int amount = static_cast<int>(sizeToRead);
  if( _pos + amount > _length )
  {
    amount -= _pos + amount - _length;
  }

  if( amount <= 0 )
  {
    return 0;
  }

  char* p = (char*)_buffer;
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
      if( ((char*)_buffer)[ idx ] == '\n'  )
      {
        ret.push_back( '\0' );
        return ret;
      }

      ret.push_back( ((char*)_buffer)[ idx ] );
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
  if (_pos + amount > _length)
    amount -= _pos + amount - _length;

  if (amount <= 0)
    return 0;

  char* p = (char*)_buffer;
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
    if (_pos + finalPos > _length)
      return false;

    _pos += finalPos;
  }
  else
  {
    if (finalPos > _length)
      return false;

    _pos = finalPos;
  }

  return true;
}


//! returns size of file
long MemoryFile::size() const { return _length; }

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

size_t MemoryFile::lastModify() const
{
  return 0;
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
long MemoryFile::getPos() const {    return _pos; }
void MemoryFile::flush() {}
bool MemoryFile::isEof() const{		return _pos >= _length;}

//! returns name of file
const Path& MemoryFile::path() const{	return _filename; }

} //end namespace io
