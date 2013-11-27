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

#ifndef __CAESARIA_DIRECTORY_H_INCLUDED__
#define __CAESARIA_DIRECTORY_H_INCLUDED__

#include "path.hpp"
#include "entries.hpp"

#include <string>

namespace vfs
{

class Directory : public Path
{
public:
  Directory();
  Directory( const Path& pathTo );
  Directory( const std::string& nPath );
  Directory( const Directory& nPath );

  virtual ~Directory() {}

  //void CopyTo( const NrpDir& pathNew );
  Path getFilePath( const Path& fileName );

  Directory operator/(const Directory& dir );
  Path operator/(const Path& filename );

  Entries getEntries() const;
  Path find( const Path& fileName ) const;
  Directory up() const;
  //void Remove();

  static Directory getCurrentDir();
  static bool changeCurrentDir( const Path& dirName );
  static Directory getApplicationDir();

  static bool create( std::string dir );
};

}//end namespace io

#endif //__CAESARIA_FILEPATH_H_INCLUDED__
