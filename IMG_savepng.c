/** \file 
 *  This file contains the implementation of the SDL png image save facility. This
 *  code allows any SDL_Surface to be saved as a png to a file, or to be written
 *  as png formatted data via a SDL_RWops.
 *
 *  \author Chris &lt;chris@starforge.co.uk&gt;
 *  \version 0.1
 *  \date 6 Aug 2010  
 *  \todo Support colorkey in true color modes?
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <png.h>
#include "IMG_savepng.h"
#include "IMG_saveend.h"


/* =============================================================================
 *  png writer implementationn
 */

/** Write png data to a RWops data source. This is a custom write callback that 
 *  will be invoked by libpng when it needs to write data from its internal 
 *  buffer.
 *
 *  \param png_ptr A pointer to the png write structure.
 *  \param data    A pointer to the png data buffer.
 *  \param length  The number of bytes to write.
 */
static void sdlrw_write_png(png_structp png_ptr, png_bytep data, png_size_t length)
{
    // Obtain the RWops stored in the write structure, so we have somewhere to write to.
    SDL_RWops *rwops = (SDL_RWops *)png_get_io_ptr(png_ptr);

    // write, and fallover if there's a problem writing the whole buffer
    if(SDL_RWwrite(rwops, data, 1, length) != length) {
        png_error(png_ptr, "Write was not able to write all png data");
    }
}


/* =============================================================================
 *  Pixel format handling code
 */

/** Enumeration of pixel format usabilities. This is used to assist in determining
 *  whether the surface to be savd is usable as-is, or whether it needs to be 
 *  converted to a usable format first.
 */
typedef enum 
{
    PF_UNUSABLE,       //!< Surface format is unusable and must be converted.
    PF_UNUSABLE_ALPHA, //!< Surface format is unusable, and it has an alpha channel.
    PF_PALETTE,        //!< Surface is palettised, and is probably savable as-is
    PF_USABLE,         //!< Surface is in 24 bit, RGB format (network byte order)
    PF_USABLE_ALPHA    //!< Surface is in 32 bit, RGBA format (network byte order)
} usability;


/** Determine whether the specified surface pixel format is suitable for direct use.
 *  This will inspect the surface's pixel format information and work out whether it
 *  can be used as-is (PF_PALETTE, PF_USABLE, or PF_USABLE_ALPHA will be returned)
 *  or whether it is unusable in its present form and needs to be converted to a 
 *  usable format (PF_USABLE or PF_USABLE_ALPHA will be returned).
 *
 *  \param surf The surface to inspect.
 *  \return A usability code indicating the suitability of the surface.
 */
static usability get_format_usability(SDL_Surface *surf)
{
    SDL_PixelFormat *fmt = surf -> format;

    // If the surfaces uses less than 8 bits per pixel, mark it as unusable
    // and let SDL deal with it, as it'll be too much headache otherwise
    if(fmt -> BitsPerPixel < 8) {
        return PF_UNUSABLE;

    // 8 bit image should be palettised..
    } else if((fmt -> BitsPerPixel == 8) && fmt -> palette) {
        return PF_PALETTE;

    // less than 24 bit formats aren't directly usable
    } else if(fmt -> BitsPerPixel < 24) {
        // if Aloss is 8, there's no alpha channel
        return (fmt -> Aloss == 8) ? PF_UNUSABLE : PF_UNUSABLE_ALPHA; 

    // if it's 24 bits per pixel, with usable masks, it's usable
    } else if(fmt -> BitsPerPixel == 24) {
        // RGB format, without alpha channel
        if(fmt -> Rmask == RMASK24 && fmt -> Gmask == GMASK24 && fmt -> Bmask == BMASK24 && fmt -> Aloss == 8) {
            return PF_USABLE;

        // Hmm, masks aren't good, check for alpha channel (this will be the normal case on little-endian systems)
        } else if(fmt -> Aloss == 8){
            return PF_UNUSABLE;

        // Masks don't match, and by some weirdness we have some alpha, so...
        } else {
            return PF_UNUSABLE_ALPHA;
        }

    // Similarly, we can cope with 32 bit, if the masks are sensible
    } else if(fmt -> BitsPerPixel == 32) {
        // RGBA format
        if(fmt -> Rmask == RMASK32 && fmt -> Gmask == GMASK32 && fmt -> Bmask == BMASK32 && fmt -> Amask == AMASK32) {
            return PF_USABLE_ALPHA;
        } else {
            return PF_UNUSABLE_ALPHA;
        }
    }

    // Get here and we don't know what the hell is going on, so let SDL deal 
    return PF_UNUSABLE;
}
        

/** Create a copy of an SDL surface using a bit format that can be fed straight
 *  to libpng. This will create a surface in 24 bit R, G, B or 32bit R, G, B, A
 *  so that libpng can process it directly, avoiding the need for on-the-fly
 *  format conversion shenanigans.
 *
 *  \param surf  The surface to convert.
 *  \param alpha True if the converted surface should have an alpha channel.
 *  \return A pointer to a copy of the surface in the usable format. The caller must
 *          free this surface when done with it.
 */
static SDL_Surface *make_usable_format(SDL_Surface *surf, int alpha)
{
    // Yes, it's probably horrible to just straight up declare these two, but ffs
    // it takes up all of 80 bytes of stack space for these...
    SDL_PixelFormat pf_temp24 = { NULL, 24, 3, 
                                  0, 0, 0, 8, 
                                  RSHIFT24, GSHIFT24, BSHIFT24, 0,
                                  RMASK24, GMASK24, BMASK24, 0,
                                  0, 255 };
    SDL_PixelFormat pf_temp32 = { NULL, 32, 4, 
                                  0, 0, 0, 0, 
                                  RSHIFT32, GSHIFT32, BSHIFT32, ASHIFT32,
                                  RMASK32, GMASK32, BMASK32, AMASK32,
                                  0, 255 };

    // Make a copy of our errant surface
    SDL_Surface *rgb_surf = SDL_ConvertSurface(surf, alpha ? &pf_temp32 : &pf_temp24, SDL_SWSURFACE);

    return rgb_surf;
}


/** Write out the PLTE (and possibly tRNS) chunk to the png. This will create the
 *  palette data to set in the PLTE chunk and set it, and if the colorkey is set
 *  for the surface an appropriate tRNS chunk is generated.
 *
 *  \param png_ptr  A pointer to the png write structure.
 *  \param info_ptr A pointer to the png info structure.
 *  \param surf     The surface that is being written to the png.
 *  \return -1 on error, 0 if the palette chunk was written without problems.
 */
static int write_palette_chunk(png_structp png_ptr, png_infop info_ptr, SDL_Surface *surf)
{
    png_colorp palette;
    Uint8     *alphas;
    int        slot;

    SDL_PixelFormat *fmt = surf -> format;
    SDL_Color *sourcepal = fmt -> palette -> colors;

    // Write the image header first...
    png_set_IHDR(png_ptr, info_ptr, surf -> w, surf -> h, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // now sort out the palette
    if(!(palette = (png_colorp)malloc(fmt -> palette -> ncolors * sizeof(png_color)))) {
        SDL_SetError("Unable to create memory for palette storage");
        return -1;
    }

    // Copy the palette over. Can't just use a straight 
    // memcpy as sdl palettes have pad bytes.
    for(slot = 0; slot < fmt -> palette -> ncolors; ++slot) {
        memcpy(&palette[slot], &sourcepal[slot], 3);
    }
    
    // Set it...
    png_set_PLTE(png_ptr, info_ptr, palette, fmt -> palette -> ncolors);

    // Done with the palette now
    free(palette);

    // If we have a colour key, we need to set up the alphas for each palette colour
    if(surf -> flags & SDL_SRCCOLORKEY) {
        // According the the PNG spec (section 4.2.1.1) we only need enough entries
        // to store transparencies up to the transparent pixel.
        if(!(alphas = (Uint8 *)malloc((fmt -> colorkey + 1) * sizeof(Uint8)))) {
            SDL_SetError("Unable to create memory for transparency storage");
            return -1;
        }

        // Set all of the alpha values to full
        memset(alphas, 255, (fmt -> colorkey + 1) * sizeof(Uint8));

        // And handle the transparent pixel
        alphas[fmt -> colorkey] = 0;

        // Write the chunk, and then we're done with the transparencies
        png_set_tRNS(png_ptr, info_ptr, alphas, fmt -> colorkey + 1, NULL);
        free(alphas);
    }

    return 0;
}


/* =============================================================================
 *  exposed code
 */

/** Write the specified surface to a file at the requested compression.
 *
 *  \param filename    The name of the file to save to.
 *  \param surf        The surface to write as a png.
 *  \param compression The compression level to write the png at. Set to -1 to use
 *                     zlib's default compression, othewise this should be in the
 *                     range 0 (no compression) to Z_BEST_COMPRESSION (usually 9).
 *  \return -1 on error, 0 if the png was saved successfully.
 */
int IMG_SavePNG(const char *filename, SDL_Surface *surf, int compression)
{
    SDL_RWops *out;
    
    // Open a RWops so we can write to a file using IMG_SavePNG_RW
    if(!(out = SDL_RWFromFile(filename, "wb"))) {
        return (-1);
    }

    // Write the png out...
    int result = IMG_SavePNG_RW(out, surf, compression);

    // and we're done
    SDL_RWclose(out);
    return result;
}


/** Write the specified surface to a SDL RWops data source at the requested 
 *  compression. This function writes the specified surface pixels to the RWops
 *  as png data, where the RWops goes to shouldn't actually matter to this.
 *
 *  \param dest        The SDL_RWops to write the surface to.
 *  \param surf        The surface to write as a png.
 *  \param compression The compression level to write the png at. Set to -1 to use
 *                     zlib's default compression, othewise this should be in the
 *                     range 0 (no compression) to Z_BEST_COMPRESSION (usually 9).
 *  \return -1 on error, 0 if the png was saved successfully.
 */
int IMG_SavePNG_RW(SDL_RWops *dest, SDL_Surface *surf, int compression) 
{
	png_structp png_ptr;
	png_infop info_ptr;
    SDL_Surface *outsurf = surf;
    png_byte *line;
    int y;
    int start;         // rw position on invoking this function, for error handling

    // Clamp the compression
    if(compression < -1) compression = -1;
    if(compression > Z_BEST_COMPRESSION) compression = Z_BEST_COMPRESSION;

    // Do nothing if we have no destination or surface
    if(!dest) {
        SDL_SetError("No destination RWops specified.");
        return -1;
    }

    if(!surf) { 
        SDL_SetError("No surface specified.");
        return -1; 
    }

    // Determine whether the surface is in a usable format, and if it is not
    // attempt to create a usable copy of it. +
    usability isUsable = get_format_usability(surf);
    if(isUsable == PF_UNUSABLE || isUsable == PF_UNUSABLE_ALPHA) {
        if(!(outsurf = make_usable_format(surf, isUsable == PF_UNUSABLE_ALPHA))) {
            SDL_SetError("Unable to create temporary surface.");
            return -1;
        }
    }

    // Create the png write structure we need to generate the png output
	if(!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,NULL,NULL))) {
		SDL_SetError("Unable to allocate png write structure");
        return -1;
	}

    // And the corresponding info structure...
    if(!(info_ptr = png_create_info_struct(png_ptr))) {
		SDL_SetError("Unable to allocate png info structure");
        png_destroy_write_struct(&png_ptr, NULL);
        return -1;
	}

    // We need to use a custom writer, so that we can output to the RWops
    png_set_write_fn(png_ptr, (void *)dest, sdlrw_write_png, NULL);

    // Determine the current position in the RW so we can restore it on errors
    start = SDL_RWtell(dest);

    // Mark the location we want to come out if there is a fatal error
	if(setjmp(png_jmpbuf(png_ptr))) {
        // Something bad happend in libpng or the write callback, clean up and give up.
        png_destroy_write_struct(&png_ptr, &info_ptr);

        // kill the temporary surface if we created one.
        if(isUsable == PF_UNUSABLE || isUsable == PF_UNUSABLE_ALPHA) SDL_FreeSurface(outsurf);

        // Restore the position of the RWops to the location it was at when we started
        SDL_RWseek(dest, start, RW_SEEK_SET);
        SDL_SetError("PNG saving error, giving up.");
        return -1;
    }

    // handle compression
    if(compression == Z_NO_COMPRESSION) {
        png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
        png_set_compression_level(png_ptr, Z_NO_COMPRESSION);
    } else {
        png_set_compression_level(png_ptr, compression);
    }  

    // sort out the header, which might include a palette...
    if(isUsable == PF_PALETTE) {
        if(write_palette_chunk(png_ptr, info_ptr, outsurf) == -1) {
            png_destroy_write_struct(&png_ptr, &info_ptr);

            // kill the temporary surface if we created one.
            if(isUsable == PF_UNUSABLE || isUsable == PF_UNUSABLE_ALPHA) SDL_FreeSurface(outsurf);
            
            // Restore the position of the RWops to the location it was at when we started
            SDL_RWseek(dest, start, RW_SEEK_SET);
            return -1;
        }

    // not palettised, and with no alpha
    } else if(isUsable == PF_USABLE || isUsable == PF_UNUSABLE) {
        png_set_IHDR(png_ptr, info_ptr, outsurf -> w, outsurf -> h, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // not palettised, with alpha
    } else {
        png_set_IHDR(png_ptr, info_ptr, outsurf -> w, outsurf -> h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    }

    // info setup done, so write it out and we can get to image data.
	png_write_info(png_ptr, info_ptr);

    // SDL does this lock/write/unlock when writing BMP so I'm fairly sure it's safe
    if(SDL_MUSTLOCK(outsurf)) SDL_LockSurface(outsurf);

    // Write out the png...
    for(y = 0, line = (png_byte *)outsurf -> pixels; y < outsurf -> h; y++, line += outsurf -> pitch) {
        png_write_row(png_ptr, line);
    }

    if(SDL_MUSTLOCK(outsurf)) SDL_UnlockSurface(outsurf);
    
    // kill the temporary surface if we created one.
    if(isUsable == PF_UNUSABLE || isUsable == PF_UNUSABLE_ALPHA) SDL_FreeSurface(outsurf);

    // All done by this point...
	png_write_end(png_ptr, NULL);
	png_destroy_write_struct(&png_ptr,&info_ptr);

    return 0;
}

#ifdef __cplusplus
}
#endif
