#ifndef __OPENCAESAR3_FSENTITY_H_INCLUDED__
#define __OPENCAESAR3_FSENTITY_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "oc3_bytearray.hpp"
#include "core/smartptr.hpp"

namespace io
{

class FilePath;

class FSEntity : public ReferenceCounted
{
public:
  typedef enum { fmRead=0x0, fmWrite=0x1, fmAppend=0x2 } Mode;
	
  virtual ~FSEntity() {}

  //! returns how much was read
  virtual int read(void* buffer, unsigned int sizeToRead) = 0;

  virtual ByteArray readLine() = 0;

  virtual ByteArray read( unsigned int sizeToRead ) = 0;

  virtual int write( const void* buffer, unsigned int sizeToWrite) = 0;

  virtual int write( const ByteArray& bArray ) = 0;

  //! changes position in file, returns true if successful
  virtual bool seek(long finalPos, bool relativeMovement = false) = 0;

  //! returns size of file
  virtual long getSize() const = 0;

  //! returns if file is open
  virtual bool isOpen() const = 0;

  //! returns where in the file we are.
  virtual long getPos() const = 0;

  //! returns name of file
  virtual const FilePath& getFileName() const = 0;

  virtual bool isEof() const = 0;

  virtual void flush() = 0;
};

typedef SmartPtr< FSEntity > FSEntityPtr;

} //end namespace io

#endif

