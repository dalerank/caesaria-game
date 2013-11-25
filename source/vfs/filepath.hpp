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

#ifndef __OPENCAESAR3_FILEPATH_H_INCLUDED__
#define __OPENCAESAR3_FILEPATH_H_INCLUDED__

#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"

#include <string>

namespace io
{

class FileList;

class FilePath
{
public:
  static const char* anyFile;
  static const char* firstEntry;
  static const char* secondEntry;

  FilePath( const char* nPath );
  FilePath();
  FilePath( const std::string& nPath );
  FilePath( const FilePath& );
  virtual ~FilePath();

  bool isExist() const;
  bool isFolder() const;

  bool rename( const FilePath& pathNew );
  std::string getExtension() const;
  
  FilePath addEndSlash() const;
  FilePath removeBeginSlash() const;
  FilePath removeEndSlash() const;
 
  const std::string& toString() const;
  std::string removeExtension() const;
 
  FilePath getUpDir() const;

  void splitToDirPathExt( FilePath* path, FilePath* filename=0, FilePath* extension=0 );

  FilePath& operator=(const FilePath& other );
  FilePath& operator+=(char c);

  //! flatten a path and file name for example: "/you/me/../." becomes "/you"
  FilePath flattenFilename( const FilePath& root = "/" ) const;

  //! Converts a relative path to an absolute (unique) path, resolving symbolic links
  FilePath getAbsolutePath() const;

  bool operator==(const FilePath& other) const;
  bool operator==(const std::string& other) const;

  char& operator[](const unsigned int index);

  bool isExtension( const std::string& ext, bool checkCase=true ) const;

  //! Returns the base part of a filename, i.e. the name without the directory
  //! part. If no directory is prefixed, the full name is returned.
  /** \param filename: The file to get the basename from */
  FilePath getBasename( bool keepExtension=true ) const;

  FilePath getRelativePathTo( const FilePath& directory ) const;

  //! Returns the directory a file is located in.
  /** \param filename: The file to get the directory from */
  virtual FilePath getFileDir() const;

  virtual void remove();

private:
  void _OsRename(const FilePath &newName);
  
  class Impl;
  ScopedPtr< Impl > _d;
};

class FileDir : public FilePath
{
public:
  FileDir( const FilePath& pathTo );
  FileDir( const std::string& nPath );
  FileDir( const FileDir& nPath );

  virtual ~FileDir() {}

  bool create();
  FilePath getFilePath( const FilePath& fileName );

  FileList getEntries() const;
  FilePath find( const FilePath& fileName ) const;
  //void Remove();

  static FileDir getCurrentDir();
  static bool changeCurrentDir( const FilePath& dirName );
  static FileDir getApplicationDir();

private:
  void _OsCreate( const FileDir &dirName );
};

}//end namespace io

#endif
