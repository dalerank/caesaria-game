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

#ifndef _CAESARIA_RESOURCELOADER_INCLUDE_H_
#define _CAESARIA_RESOURCELOADER_INCLUDE_H_

#include "core/signals.hpp"
#include "core/scopedptr.hpp"
#include "vfs/path.hpp"
#include "vfs/directory.hpp"
#include "vfs/file.hpp"
#include "vfs/archive.hpp"


class ResourceLoader
{
public:
  ResourceLoader();
  virtual ~ResourceLoader();

  void loadFromModel( vfs::Path path2model, const vfs::Directory altDir=vfs::Directory() );
  void loadAtlases( vfs::NFile file, bool lazy );
  void loadFiles( vfs::ArchivePtr archive );
  void loadFiles( vfs::Path path );

public signals:
  Signal1<std::string> &onStartLoading();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_RESOURCELOADER_INCLUDE_H_
