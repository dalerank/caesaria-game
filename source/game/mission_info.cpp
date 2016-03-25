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

#include "mission_info.hpp"
#include "vfs/path.hpp"
#include "core/logger.hpp"
#include "core/saveadapter.hpp"

MissionInfo::MissionInfo()
{

}

bool MissionInfo::load(const std::string& path)
{
  vfs::Path p(path);
  _info = config::load(p);

  if (!p.exist() || _info.empty())
  {
    Logger::error("Cant load mission info from " + p.toString());
    return false;
  }

  _info["original.path"] = p.toString();
  _info["localization.name"] = p.baseName().removeExtension();
  return true;
}

Variant MissionInfo::get(const std::string& name)
{
  return _info.get(name);
}
