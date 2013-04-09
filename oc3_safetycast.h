#ifndef __OPENCAESAR3_SAFETY_CAST_INCLUDE_
#define __OPENCAESAR3_SAFETY_CAST_INCLUDE_

#include <typeinfo>

template<class T, class B>
inline T safety_cast( B object ) throw()
{
    try
    {
        return dynamic_cast<T>(object);
    }
#if defined _WIN32 && !USE_MINGW_COMPILER
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

#endif
