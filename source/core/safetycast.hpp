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


#ifndef __CAESARIA_SAFETY_CAST_INCLUDE_
#define __CAESARIA_SAFETY_CAST_INCLUDE_

#include <typeinfo>

template<class T, class B>
inline T safety_cast( B object ) throw()
{
    try
    {
        return dynamic_cast<T>(object);
    }
#if defined(OC3_PLATFORM_WIN) && !defined(OC3_USE_MINGW_COMPILER)
    catch(std::__non_rtti_object )
    {
        //LOGIT(pfEmerg, "cast(0x%p): __non_rtti_object exception has caught: '%s'\n", object, e.what());
        return 0;
    }
#else
    catch(std::bad_cast)
    {
        return 0;
    }
#endif
    catch(...)
    {
        //LOGIT(pfEmerg, "cast(0x%p): an unknown exception has caught.\n", object);
        return 0;
    }
}

#endif //__CAESARIA_SAFETY_CAST_INCLUDE_
