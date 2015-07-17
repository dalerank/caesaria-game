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

#ifndef __CAESARIA_STACKTRACE_H_INCLUDED__
#define __CAESARIA_STACKTRACE_H_INCLUDED__

//#include <cdk.h>
#include "platform.hpp"

namespace crashhandler
{
void install();
void remove();
void printstack(bool showMessage = true, unsigned int starting_frame = 0, unsigned int max_frames = 63 );
}

#endif //__CAESARIA_STACKTRACE_H_INCLUDED__
