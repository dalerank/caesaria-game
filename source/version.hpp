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

#ifndef __OPENCAESAR3_PROJECT_VERSION_INCLUDE_H_
#define __OPENCAESAR3_PROJECT_VERSION_INCLUDE_H_

#include "core/platform.hpp"

#define OC3_VERSION_MAJOR 0
#define OC3_VERSION_MINOR 2
#define OC3_VERSION_REVSN 856

#define OC3_STR_EXT(__A) #__A
#define OC3_STR_A(__A) OC3_STR_EXT(__A)
#define OC3_VERSION OC3_STR_A(OC3_VERSION_MAJOR)"."OC3_STR_A(OC3_VERSION_MINOR)"."OC3_STR_A(OC3_VERSION_REVSN)"["OC3_PLATFORM_NAME":"OC3_COMPILER_NAME"]"

#endif
