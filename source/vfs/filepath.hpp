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

#ifndef __CAESARIA_FILEPATH_H_INCLUDED__
#define __CAESARIA_FILEPATH_H_INCLUDED__

#include <string>
#include "core/scopedptr.hpp"

namespace vfs
{

class Path
{
public:
  static const char* anyFile;
  static const char* firstEntry;
  static const char* secondEntry;

  Path( const char* nPath );
  Path();
  Path( const std::string& nPath );
  Path( const Path& );
  virtual ~Path();

  bool isExist() const;
  bool isFolder() const;

  std::string getExtension() const;
  
  Path addEndSlash() const;
  Path removeBeginSlash() const;
  Path removeEndSlash() const;
 
  const std::string& toString() const;
  std::string removeExtension() const;

  void splitToDirPathExt( Path* path, Path* filename=0, Path* extension=0 );

  Path& operator=(const Path& other );
  Path& operator+=(char c);

  //! flatten a path and file name for example: "/you/me/../." becomes "/you"
  Path flattenFilename( const Path& root = "/" ) const;

  //! Converts a relative path to an absolute (unique) path, resolving symbolic links
  Path getAbsolutePath() const;

  bool operator==(const Path& other) const;
  bool operator==(const std::string& other) const;

  char& operator[](const unsigned int index);

  bool isExtension( const std::string& ext, bool checkCase=true ) const;

  //! Returns the base part of a filename, i.e. the name without the directory
  //! part. If no directory is prefixed, the full name is returned.
  /** \param filename: The file to get the basename from */
  Path getBasename( bool keepExtension=true ) const;

  Path getRelativePathTo( const Path& directory ) const;

  //! Returns the directory a file is located in.
  /** \param filename: The file to get the directory from */
  virtual std::string getDir() const;

private:  
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace io

#endif //__CAESARIA_FILEPATH_H_INCLUDED__
