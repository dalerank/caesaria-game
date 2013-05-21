/** \file 
 *  This file contains several macros to simplify handling endian issues on different
 *  architectures. The defines in this file are used in IMG_savejpg and IMG_savepng 
 *  when checking whether the pixel format of a surface to be saved can be passed to
 *  the appropriate library, and as parameters when creating suitable format copies
 *  if the surface is not directly usable.
 *
 *  \author Chris &lt;chris@starforge.co.uk&gt;
 *  \version 0.1
 *  \date 6 Aug 2010  
 */
/*  
 * Copyright (c) 2010, Chris Page <chris@starforge.co.uk>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this 
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice, this 
 *    list of conditions and the following disclaimer in the documentation and/or 
 *    other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __IMG_SAVEEND_
#define __IMG_SAVEEND_

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL/SDL_byteorder.h> // Needed for endian detection stuff

// Deal with byte order issues. I would like to take this opportunity to ask
// the creators of little-endian architectures to die in a fire.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    // 24 bit RGB masks on big-endian
    #define RMASK24 0xFF0000
    #define GMASK24 0x00FF00
    #define BMASK24 0x0000FF

    // 32 bit RGBA masks on big-endian
    #define RMASK32 0xFF000000
    #define GMASK32 0x00FF0000
    #define BMASK32 0x0000FF00
    #define AMASK32 0x000000FF

    // 24 bit shifts on big-endian
    #define RSHIFT24 16
    #define GSHIFT24 8
    #define BSHIFT24 0

    // 32 bit shifts on big-endian
    #define RSHIFT24 24
    #define GSHIFT24 16
    #define BSHIFT24 8
    #define ASHIFT24 0
#else
    // 24 bit RGB masks on little-endian
    #define RMASK24 0x0000FF
    #define GMASK24 0x00FF00
    #define BMASK24 0xFF0000

    // 32 bit RGBA masks on little-endian
    #define RMASK32 0x000000FF
    #define GMASK32 0x0000FF00
    #define BMASK32 0x00FF0000
    #define AMASK32 0xFF000000

    // 24 bit shifts on little-endian
    #define RSHIFT24 0 
    #define GSHIFT24 8
    #define BSHIFT24 16

    // 32 bit shifts on little-endian
    #define RSHIFT32 0
    #define GSHIFT32 8
    #define BSHIFT32 16
    #define ASHIFT32 24
#endif

#ifdef __cplusplus
}
#endif

#endif // #ifndef __IMG_SAVEEND_
