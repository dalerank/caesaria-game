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

#ifndef __CAESARIA_FILEPATH_H_INCLUDED__
#define __CAESARIA_FILEPATH_H_INCLUDED__

#include <string>
#include "core/scopedptr.hpp"

namespace vfs
{

class Directory;
class Info;

class Path
{
public:
  typedef enum { nativeCase=0, ignoreCase=1, equaleCase=2 } SensType;

  static const char* anyFile;
  static const char* firstEntry;
  static const char* secondEntry;

  Path( const char* nPath );
  Path( );
  Path( const std::string& nPath );
  Path( const Path& );
  virtual ~Path();

  bool exist( SensType sens=nativeCase ) const;
  bool empty() const;
  bool isFolder() const;
  bool isDirectoryEntry() const;

  //Returns the suffix of the file.
  //The suffix consists of all characters in the file after (but not including) the last '.'.
  std::string suffix() const;
  
  Path addEndSlash() const;
  Path removeEndSlash() const;

  Path removeBeginSlash() const;

  char lastChar() const;
  char firstChar() const;
 
  const std::string& toString() const;
  const char* toCString() const;

  std::string extension() const;
  bool haveExtension() const;
  unsigned int hash() const;
  std::string removeExtension() const;
  Path changeExtension( const std::string& newExtension ) const;
  bool isMyExtension( const std::string& ext, bool checkCase=true ) const;

  void splitToDirPathExt( Path* path, Path* filename=0, Path* extension=0 );

  Path& operator=(const Path& other );
  Path& operator+=(char c);
  Path operator+(const Path& other );
  Path canonical() const;

  Info info() const;

  //! flatten a path and file name for example: "/you/me/../." becomes "/you"
  Path flattenFilename( const Path& root = "/" ) const;

  //! Converts a relative path to an absolute (unique) path, resolving symbolic links
  Path absolutePath() const;

  bool operator==(const Path& other) const;
  bool operator==(const std::string& other) const;

  char& operator[](const unsigned int index);

  //! Returns the base part of a filename, i.e. the name without the directory
  //! part. If no directory is prefixed, the full name is returned.
  /** \param filename: The file to get the basename from */
  Path baseName( bool keepExtension=true ) const;

  Path getRelativePathTo( const Directory& directory ) const;

  //! Returns the directory a file is located in.
  /** \param filename: The file to get the directory from */
  virtual std::string directory() const;

private:  
  class Impl;
  ScopedPtr< Impl > _d;
};

inline std::string operator+(const std::string& str, const Path& path)
{
  return str + path.toString();
}

}//end namespace io

#endif //__CAESARIA_FILEPATH_H_INCLUDED__
