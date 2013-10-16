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


#include "oc3_filesystem_memfile.hpp"
#include "oc3_stringhelper.hpp"

namespace io
{

MemoryFile::MemoryFile()
{
    Buffer = 0;
    Len = 0;
    Pos = 0;
    deleteMemoryWhenDropped = false;

#ifdef _DEBUG
        setDebugName(L"MemoryFile");
#endif

}

NFile MemoryFile::create(void* memory, long len, const FilePath& fileName, bool d)
{
    MemoryFile* mf = new MemoryFile();
    mf->Buffer = memory;
    mf->Len  = len;
    mf->Pos = 0;
    mf->Filename = fileName;
    mf->deleteMemoryWhenDropped = d;

    FSEntityPtr ret( mf );
    ret->drop();

    return NFile( ret );
}

NFile MemoryFile::create( ByteArray data, const FilePath& fileName )
{
    MemoryFile* mf = new MemoryFile();
    mf->Buffer = new char[ data.size() ];
    memcpy( mf->Buffer, data.data(), data.size() );
    mf->Len  = data.size();
    mf->Pos = 0;
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
  if( Pos + amount > Len )
  {
    amount -= Pos + amount - Len;
  }

  if( amount <= 0 )
  {
    return 0;
  }

  char* p = (char*)Buffer;
  memcpy(buffer, p + Pos, amount);

  Pos += amount;
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

        int idx = Pos;
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

        int reallyReadingBytes = idx - Pos;

        if( reallyReadingBytes <= 0 )
            return ByteArray();

        if( readOneLineCounter > 1000 )
        {
            StringHelper::debug( 0xff, "Too many iteration for read one line" );
            return ByteArray();
        }
    }

    return ret;
}

//! returns how much was written
int MemoryFile::write(const void* buffer, unsigned int sizeToWrite)
{
    int amount = static_cast<int>(sizeToWrite);
	if (Pos + amount > Len)
		amount -= Pos + amount - Len;

	if (amount <= 0)
		return 0;

    char* p = (char*)Buffer;
	memcpy(p + Pos, buffer, amount);

	Pos += amount;

	return amount;
}



//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool MemoryFile::seek(long finalPos, bool relativeMovement)
{
	if (relativeMovement)
	{
		if (Pos + finalPos > Len)
			return false;

		Pos += finalPos;
	}
	else
	{
		if (finalPos > Len)
			return false;

		Pos = finalPos;
	}

	return true;
}


//! returns size of file
long MemoryFile::getSize() const
{
    return Len;
}

bool MemoryFile::isOpen() const
{
    return getSize() != 0;
}

int MemoryFile::write(const ByteArray &array )
{
    int savePos = Pos;
    write( array.data(), array.size() );
    return Pos - savePos;
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
    return Pos;
}

void MemoryFile::flush()
{
}

bool MemoryFile::isEof() const
{
    return Pos >= Len;
}

//! returns name of file
const FilePath& MemoryFile::getFileName() const
{
	return Filename;
}

} //end namespace io
