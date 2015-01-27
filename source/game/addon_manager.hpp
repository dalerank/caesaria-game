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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_ADDON_MANAGER_INCLUDE_HPP__
#define __CAESARIA_ADDON_MANAGER_INCLUDE_HPP__

#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"
#include "vfs/directory.hpp"
#include "core/addon_requirements.hpp"
#include "core/singleton.hpp"

namespace addon
{

class Addon : public ReferenceCounted
{
public:
  Addon();
  ~Addon();

  bool open( vfs::Path path );
  void initialize();
  unsigned int level() const;

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

class Manager : public StaticSingleton<Manager>
{
  friend class StaticSingleton;
public:
  ~Manager();

  void load( vfs::Directory folder );
  void load( vfs::Path path, bool ls );
  void initAddons4level( addon::Type type );

private:
  Manager();
  class Impl;
  ScopedPtr<Impl> _d;
};

}

#endif // __CAESARIA_ADDON_MANAGER_INCLUDE_HPP__
