#ifndef _OPENCAESAR3_REQUIREMENTS_INCLUDE_
#define _OPENCAESAR3_REQUIREMENTS_INCLUDE_

#define _USE_ASSERT_4_DEBUG

#if defined(_USE_ASSERT_4_DEBUG)
    #if defined(_MSC_VER) && !defined (_WIN32_WCE)
        #if defined(WIN64) || defined(_WIN64) // using portable common solution for x64 configuration
            #include <crtdbg.h>
            #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
        #else
            #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
        #endif
    #else
        #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
    #endif
#else
    #define _OC3_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#endif
