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

#ifndef __OPENCAESAR3_SCENARIOABSTRACTLOADER_H_INCLUDED__
#define __OPENCAESAR3_SCENARIOABSTRACTLOADER_H_INCLUDED__

#include "oc3_referencecounted.hpp"

class Scenario;

class ScenarioAbstractLoader : public ReferenceCounted
{
public:
  virtual ~ScenarioAbstractLoader() {}
  virtual bool load( const std::string& filename, Scenario& oScenario ) = 0;
  virtual bool isLoadableFileExtension( const std::string& filename ) = 0;
  //virtual bool isLoadableFileFormat( file ) = 0;
};


#endif //__OPENCAESAR3_SCENARIOABSTRACTLOADER_H_INCLUDED__