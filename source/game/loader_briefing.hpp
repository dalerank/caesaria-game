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

#ifndef __CAESARIA_SCENARIO_BRIEFING_LOADER_H_INCLUDED__
#define __CAESARIA_SCENARIO_BRIEFING_LOADER_H_INCLUDED__

#include "abstractloader.hpp"
#include "core/scopedptr.hpp"

class GameLoaderBriefing : public GameAbstractLoader
{
public:
  GameLoaderBriefing();

  bool load(const std::string& filename, Game& oScenario);
  bool isLoadableFileExtension( const std::string& filename );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_SCENARIO_BRIEFING_LOADER_H_INCLUDED__
