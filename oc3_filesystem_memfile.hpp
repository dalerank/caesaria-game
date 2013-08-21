#ifndef __OC3_MEMORY_FILE_H_INCLUDED__
#define __OC3_MEMORY_FILE_H_INCLUDED__

#include "oc3_filesystem_file.hpp"
#include "oc3_filepath.hpp"
#include "oc3_bytearray.hpp"

namespace io
{

/*!
        Class for reading and writing from memory.
*/
class MemoryFile : public FSEntity
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
  virtual const FilePath& getFileName() const;

  static NFile create( void* memory, long len, const FilePath& fileName, bool deleteMemoryWhenDropped );
  static NFile create( ByteArray data, const FilePath& fileName );

private:
  MemoryFile();

  void *Buffer;
  long Len;
  long Pos;
  FilePath Filename;
  bool deleteMemoryWhenDropped;
};

} //end namespace io


#endif

