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

#ifndef __CAESARIA_PROJECT_VERSION_INCLUDE_H_
#define __CAESARIA_PROJECT_VERSION_INCLUDE_H_

#include "core/platform.hpp"

#define CAESARIA_VERSION_MAJOR 0
#define CAESARIA_VERSION_MINOR 4
#define CAESARIA_VERSION_REVSN @rev_number@

#define CAESARIA_STR_EXT(__A) #__A
#define CAESARIA_STR_A(__A) CAESARIA_STR_EXT(__A)
#define CAESARIA_VERSION CAESARIA_STR_A(CAESARIA_VERSION_MAJOR)"."CAESARIA_STR_A(CAESARIA_VERSION_MINOR)"."CAESARIA_STR_A(CAESARIA_VERSION_REVSN)"["CAESARIA_PLATFORM_NAME":"CAESARIA_COMPILER_NAME"]"

#endif
