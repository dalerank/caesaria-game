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

#ifndef __CAESARIA_COLOR_H_INCLUDED__
#define __CAESARIA_COLOR_H_INCLUDED__

#include "core/math.hpp"

 
    
class ColorHelper
{
public:
	//! Creates a 16 bit A1R5G5B5 color
	static inline unsigned short RGBA16(unsigned int r, unsigned int g, unsigned int b, unsigned int a=0xFF)
	{
		return (unsigned short)((a & 0x80) << 8 |
			(r & 0xF8) << 7 |
			(g & 0xF8) << 2 |
			(b & 0xF8) >> 3);
	}


	//! Creates a 16 bit A1R5G5B5 color
	static inline unsigned short RGB16(unsigned int r, unsigned int g, unsigned int b)
	{
		return RGBA16(r,g,b);
	}


	//! Creates a 16bit A1R5G5B5 color, based on 16bit input values
	static inline unsigned short RGB16from16(unsigned short r, unsigned short g, unsigned short b)
	{
		return (0x8000 |
				(r & 0x1F) << 10 |
				(g & 0x1F) << 5  |
				(b & 0x1F));
	}


	//! Converts a 32bit (X8R8G8B8) color to a 16bit A1R5G5B5 color
	static inline unsigned short X8R8G8B8toA1R5G5B5(unsigned int color)
	{
		return (unsigned short)(0x8000 |
			( color & 0x00F80000) >> 9 |
			( color & 0x0000F800) >> 6 |
			( color & 0x000000F8) >> 3);
	}


	//! Converts a 32bit (A8R8G8B8) color to a 16bit A1R5G5B5 color
	static inline unsigned short A8R8G8B8toA1R5G5B5(unsigned int color)
	{
		return (unsigned short)(( color & 0x80000000) >> 16|
			( color & 0x00F80000) >> 9 |
			( color & 0x0000F800) >> 6 |
			( color & 0x000000F8) >> 3);
	}


	//! Converts a 32bit (A8R8G8B8) color to a 16bit R5G6B5 color
	static inline unsigned short A8R8G8B8toR5G6B5(unsigned int color)
	{
		return (unsigned short)(( color & 0x00F80000) >> 8 |
			( color & 0x0000FC00) >> 5 |
			( color & 0x000000F8) >> 3);
	}


	//! Convert A8R8G8B8 Color from A1R5G5B5 color
	/** build a nicer 32bit Color by extending dest lower bits with source high bits. */
	static inline unsigned int A1R5G5B5toA8R8G8B8(unsigned short color)
	{
		return ( (( -( (int) color & 0x00008000 ) >> (int) 31 ) & 0xFF000000 ) |
				(( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
				(( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
				(( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2)
				);
	}


	//! Returns A8R8G8B8 Color from R5G6B5 color
	static inline unsigned int R5G6B5toA8R8G8B8(unsigned short color)
	{
		return 0xFF000000 |
			((color & 0xF800) << 8)|
			((color & 0x07E0) << 5)|
			((color & 0x001F) << 3);
	}


	//! Returns A1R5G5B5 Color from R5G6B5 color
	static inline unsigned short R5G6B5toA1R5G5B5(unsigned short color)
	{
		return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
	}


	//! Returns R5G6B5 Color from A1R5G5B5 color
	static inline unsigned short A1R5G5B5toR5G6B5(unsigned short color)
	{
		return (((color & 0x7FE0) << 1) | (color & 0x1F));
	}



	//! Returns the alpha component from A1R5G5B5 color
    /** alpha refers to opacity.
	\return The alpha value of the color. 0 is transparent, 1 is opaque. */
	static inline unsigned int getAlpha(unsigned short color)
	{
		return ((color >> 15)&0x1);
	}


	//! Returns the red component from A1R5G5B5 color.
	/** Shift left by 3 to get 8 bit value. */
	static inline unsigned int getRed(unsigned short color)
	{
		return ((color >> 10)&0x1F);
	}


	//! Returns the green component from A1R5G5B5 color
	/** Shift left by 3 to get 8 bit value. */
	static inline unsigned int getGreen(unsigned short color)
	{
		return ((color >> 5)&0x1F);
	}


	//! Returns the blue component from A1R5G5B5 color
	/** Shift left by 3 to get 8 bit value. */
	static inline unsigned int getBlue(unsigned short color)
	{
		return (color & 0x1F);
	}


	//! Returns the average from a 16 bit A1R5G5B5 color
	static inline int getAverage(short color)
	{
		return ((getRed(color)<<3) + (getGreen(color)<<3) + (getBlue(color)<<3)) / 3;
	}
};

//! Class representing a 32 bit ARGB color.
/** The color values for alpha, red, green, and blue are
stored in a single unsigned int. So all four values may be between 0 and 255.
This class must consist of only one unsigned int and must not use virtual functions.
 */

class NColor
{
public:
  enum Format
  {
    //! 16 bit color format used by the software driver.
    /** There are 5 bits for every color component, and a single bit is left
    for alpha information. */
    colorA1R5G5B5 = 0,

    //! Standard 16 bit color format.
    colorR5G6B5,

    //! 24 bit color, no alpha channel, but 8 bit for red, green and blue.
    colorR8G8B8,

    //! Default 32 bit color format. 8 bits are used for every component: red, green, blue and alpha.
    colorA8R8G8B8,

    /** Floating Point formats. The following formats may only be used for render target textures. */

    //! 16 bit floating point format using 16 bits for the red channel.
    colorR16F,

    //! 32 bit floating point format using 16 bits for the red channel and 16 bits for the green channel.
    colorG16R16F,

    //! 64 bit floating point format 16 bits are used for the red, green, blue and alpha channels.
    colorA16B16G16R16F,

    //! 32 bit floating point format using 32 bits for the red channel.
    colorR32F,

    //! 64 bit floating point format using 32 bits for the red channel and 32 bits for the green channel.
    colorG32R32F,

    //! 128 bit floating point format. 32 bits are used for the red, green, blue and alpha channels.
    colorA32B32G32R32F,

    //! Unknown color format:
    colorUnknown
  };

	//! Constructor of the Color. Does nothing.
	/** The color value is not initialized to save time. */
	NColor() {}

	//! Constructs the color from 4 values representing the alpha, red, green and blue component.
	/** Must be values between 0 and 255. */
	NColor (unsigned char a, unsigned char r, unsigned char g, unsigned char b)
		: color(((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)) {}

	//! Constructs the color from a 32 bit value. Could be another color.
	NColor( unsigned int clr)
		: color(clr) {}

	//! Returns the alpha component of the color.
	/** The alpha component defines how opaque a color is.
	\return The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
	unsigned char alpha() const { return color>>24; }

	//! Returns the red component of the color.
	/** \return Value between 0 and 255, specifying how red the color is.
	0 means no red, 255 means full red. */
	unsigned char red() const { return (color>>16) & 0xff; }

	//! Returns the green component of the color.
	/** \return Value between 0 and 255, specifying how green the color is.
	0 means no green, 255 means full green. */
	unsigned char green() const { return (color>>8) & 0xff; }

	//! Returns the blue component of the color.
	/** \return Value between 0 and 255, specifying how blue the color is.
	0 means no blue, 255 means full blue. */
	unsigned char blue() const { return color & 0xff; }

	//! Get lightness of the color in the range [0,255]
	float getLightness() const
	{
     return 0.5f*(  (std::max)((std::max)(red(),green()),blue())+(std::min)((std::min)(red(),green()),blue()));
	}

	//! Get luminance of the color in the range [0,255].
	float getLuminance() const
	{
		return 0.3f*red() + 0.59f*green() + 0.11f*blue();
	}

	//! Get average intensity of the color in the range [0,255].
	unsigned int getAverage() const
	{
		return ( red() + green() + blue() ) / 3;
	}

	//! Sets the alpha component of the Color.
	/** The alpha component defines how transparent a color should be.
	\param a The alpha value of the color. 0 is fully transparent, 255 is fully opaque. */
	void setAlpha(unsigned int a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }

	//! Sets the red component of the Color.
	/** \param r: Has to be a value between 0 and 255.
	0 means no red, 255 means full red. */
	void setRed( unsigned int r) { color = ((r & 0xff)<<16) | (color & 0xff00ffff); }

	//! Sets the green component of the Color.
	/** \param g: Has to be a value between 0 and 255.
	0 means no green, 255 means full green. */
	void setGreen( unsigned int g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }

	//! Sets the blue component of the Color.
	/** \param b: Has to be a value between 0 and 255.
	0 means no blue, 255 means full blue. */
	void setBlue( unsigned int b) { color = (b & 0xff) | (color & 0xffffff00); }

	//! Converts color to OpenGL color format
	/** From ARGB to RGBA in 4 byte components for endian aware
	passing to OpenGL
	\param dest: address where the 4x8 bit OpenGL color is stored. */
	void toOpenGLColor(unsigned char* dest) const
	{
		*dest =   (unsigned char)red();
		*++dest = (unsigned char)green();
		*++dest = (unsigned char)blue();
		*++dest = (unsigned char)alpha();
	}

	//! Sets all four components of the color at once.
	/** Constructs the color from 4 values representing the alpha,
	red, green and blue components of the color. Must be values
	between 0 and 255.
	\param a: Alpha component of the color. The alpha component
	defines how transparent a color should be. Has to be a value
	between 0 and 255. 255 means not transparent (opaque), 0 means
	fully transparent.
	\param r: Sets the red component of the Color. Has to be a
	value between 0 and 255. 0 means no red, 255 means full red.
	\param g: Sets the green component of the Color. Has to be a
	value between 0 and 255. 0 means no green, 255 means full
	green.
	\param b: Sets the blue component of the Color. Has to be a
	value between 0 and 255. 0 means no blue, 255 means full blue. */
	void set( unsigned int a, unsigned int r, unsigned int g, unsigned int b)
	{
		color = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff));
	}
	void set( unsigned int col) { color = col; }

	//! Compares the color to another color.
	/** \return True if the colors are the same, and false if not. */
	bool operator==(const NColor& other) const { return other.color == color; }

	//! Compares the color to another color.
	/** \return True if the colors are different, and false if they are the same. */
	bool operator!=(const NColor& other) const { return other.color != color; }

	//! comparison operator
	/** \return True if this color is smaller than the other one */
	bool operator<(const NColor& other) const { return (color < other.color); }

	//! Adds two colors, result is clamped to 0..255 values
	/** \param other Color to add to this color
	\return Addition of the two colors, clamped to 0..255 values */
	NColor operator+(const NColor& other) const
	{
    return NColor( (std::min)( alpha() + other.alpha(), 255 ),
                                (std::min)(red() + other.red(), 255),
                                (std::min)(green() + other.green(), 255),
                                (std::min)(blue() + other.blue(), 255));
	}

	//! Interpolates the color with a float value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f
	\return Interpolated color. */
	NColor getInterpolated(const NColor &other, float d) const
	{
    d = math::clamp(d, 0.f, 1.f);
		const float inv = 1.0f - d;
    return NColor((unsigned int)floor(other.alpha()*inv + alpha()*d),
                        (unsigned int)floor(other.red()*inv + red()*d),
                        (unsigned int)floor(other.green()*inv + green()*d),
                        (unsigned int)floor(other.blue()*inv + blue()*d));
	}

	int rgba() const
	{
		int a = alpha();
		return ( ((color << 8) & 0xffffff00 ) + a);
	}

	int abgr() const
	{
		int r = red() ;
		int b = (blue() << 16);
		return ( (color & 0xff00ff00 )+ r + b);
	}

	//! color in A8R8G8B8 Format
	unsigned int color;
};


//! Class representing a color with four floats.
/** The color values for red, green, blue
and alpha are each stored in a 32 bit floating point variable.
So all four values may be between 0.0f and 1.0f.
Another, faster way to define colors is using the class Color, which
stores the color values in a single 32 bit integer.
*/
class NColorf
{
public:
	//! Default constructor for Colorf.
	/** Sets red, green and blue to 0.0f and alpha to 1.0f. */
	NColorf() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

	//! Constructs a color from up to four color values: red, green, blue, and alpha.
	/** \param r: Red color component. Should be a value between
	0.0f meaning no red and 1.0f, meaning full red.
	\param g: Green color component. Should be a value between 0.0f
	meaning no green and 1.0f, meaning full green.
	\param b: Blue color component. Should be a value between 0.0f
	meaning no blue and 1.0f, meaning full blue.
	\param a: Alpha color component of the color. The alpha
	component defines how transparent a color should be. Has to be
	a value between 0.0f and 1.0f, 1.0f means not transparent
	(opaque), 0.0f means fully transparent. */
	NColorf(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

	//! Constructs a color from 32 bit Color.
	/** \param c: 32 bit color from which this Colorf class is
	constructed from. */
	NColorf(NColor c)
	{
		const float inv = 1.0f / 255.0f;
		r = c.red() * inv;
		g = c.green() * inv;
		b = c.blue() * inv;
		a = c.alpha() * inv;
	}

	//! Converts this color to a Color without floats.
	NColor toColor() const
	{
      return NColor((unsigned int)floor(a*255.0f), 
                    (unsigned int)floor(r*255.0f), 
                    (unsigned int)floor(g*255.0f), 
                    (unsigned int)floor(b*255.0f));
	}

	//! Sets three color components to new values at once.
	/** \param rr: Red color component. Should be a value between 0.0f meaning
	no red (=black) and 1.0f, meaning full red.
	\param gg: Green color component. Should be a value between 0.0f meaning
	no green (=black) and 1.0f, meaning full green.
	\param bb: Blue color component. Should be a value between 0.0f meaning
	no blue (=black) and 1.0f, meaning full blue. */
	void set(float rr, float gg, float bb) {r = rr; g =gg; b = bb; }

	//! Sets all four color components to new values at once.
	/** \param aa: Alpha component. Should be a value between 0.0f meaning
	fully transparent and 1.0f, meaning opaque.
	\param rr: Red color component. Should be a value between 0.0f meaning
	no red and 1.0f, meaning full red.
	\param gg: Green color component. Should be a value between 0.0f meaning
	no green and 1.0f, meaning full green.
	\param bb: Blue color component. Should be a value between 0.0f meaning
	no blue and 1.0f, meaning full blue. */
	void set(float aa, float rr, float gg, float bb) {a = aa; r = rr; g =gg; b = bb; }

	//! Interpolates the color with a float value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f
	\return Interpolated color. */
	NColorf getInterpolated(const NColorf &other, float d) const
	{
    d = math::clamp(d, 0.f, 1.f);
		const float inv = 1.0f - d;
		return NColorf(other.r*inv + r*d,
		other.g*inv + g*d, other.b*inv + b*d, other.a*inv + a*d);
	}

	//! Sets a color component by index. R=0, G=1, B=2, A=3
	void setColorComponentValue(const int index, float value)
	{
		switch(index)
		{
		case 0: r = value; break;
		case 1: g = value; break;
		case 2: b = value; break;
		case 3: a = value; break;
		}
	}

	//! Returns the alpha component of the color in the range 0.0 (transparent) to 1.0 (opaque)
	float getAlpha() const { return a; }

	//! Returns the red component of the color in the range 0.0 to 1.0
	float getRed() const { return r; }

	//! Returns the green component of the color in the range 0.0 to 1.0
	float getGreen() const { return g; }

	//! Returns the blue component of the color in the range 0.0 to 1.0
	float getBlue() const { return b; }

	//! red color component
	float r;

	//! green color component
	float g;

	//! blue component
	float b;

	//! alpha color component
	float a;
};

namespace DefaultColors
{
#define __REG_COLOR(a,b) const NColor a(b);
__REG_COLOR( red, 0xffff0000 )
__REG_COLOR( caesarRed, 0xffA00000 )
__REG_COLOR( blue, 0xff0000ff )
__REG_COLOR( green, 0xff00ff00 )
__REG_COLOR( show, 0xfffffafa )
__REG_COLOR( ghost, 0xffF8F8FF )
__REG_COLOR( whitesmoke, 0xffF5F5F5 )
__REG_COLOR( floralwhite, 0xffFFFAF0 )
__REG_COLOR( oldlace, 0xffFDF5E6 )
__REG_COLOR( linen, 0xffFAF0E6 )
__REG_COLOR( antiqueWhite, 0xffFAEBD7 )
__REG_COLOR( PapayaWhip, 0xffFFEFD5 )
__REG_COLOR( blanchedAlmond, 0xffFFEBCD )
__REG_COLOR( bisque, 0xffFFE4C4 )
__REG_COLOR( peachPuff, 0xffFFDAB9 )
__REG_COLOR( navajoWhite, 0xffFFDEAD )
__REG_COLOR( moccasin, 0xffFFE4B5 )
__REG_COLOR( cornsilk, 0xffFFF8DC )
__REG_COLOR( ivory, 0xffFFFFF0 )
__REG_COLOR( lemonChiffon, 0xffFFFACD )
__REG_COLOR( seashell, 0xffFFF5EE )
__REG_COLOR( honeydew, 0xffF0FFF0 )
__REG_COLOR( mintCream, 0xffF5FFFA )
__REG_COLOR( azure, 0xffF0FFFF )
__REG_COLOR( aliceBlue, 0xffF0F8FF )
__REG_COLOR( lavender, 0xffE6E6FA )
__REG_COLOR( lavenderBlush, 0xffFFF0F5 )
__REG_COLOR( mistyRose, 0xffFFE4E1 )
__REG_COLOR( white, 0xffFFFFFF )
__REG_COLOR( black, 0xff000000 )
__REG_COLOR( darkSlateGray, 0xff2F4F4F)
__REG_COLOR( dimGrey, 0xff696969)
__REG_COLOR( slateGrey,	0xff708090)
__REG_COLOR( lightSlateGray,	0xff778899)
__REG_COLOR( grey,	0xffBEBEBE)
__REG_COLOR( lightGray,	0xffD3D3D3)
__REG_COLOR( midnightBlue,	0xff191970)
__REG_COLOR( navyBlue,	0xff000080)
__REG_COLOR( cornflowerBlue,	0xff6495ED)
__REG_COLOR( darkSlateBlue,	0xff483D8B)
__REG_COLOR( slateBlue,	0xff6A5ACD)
__REG_COLOR( mediumSlateBlue,	0xff7B68EE)
__REG_COLOR( lightSlateBlue,	0xff8470FF)
__REG_COLOR( mediumBlue,	0xff0000CD)
__REG_COLOR( royalBlue,	0xff4169E1)
__REG_COLOR( skyBlue,	0xff87CEEB)
__REG_COLOR( paleTurquoise,	0xffAFEEEE)
__REG_COLOR( cyan,	0xff00FFFF)
__REG_COLOR( mediumAquamarine,	0xff66CDAA)
__REG_COLOR( darkGreen,	0xff006400)
__REG_COLOR( darkSeaGreen,	0xff8FBC8F)
__REG_COLOR( paleGreen,	0xff98FB98)
__REG_COLOR( limeGreen,	0xff32CD32)
__REG_COLOR( darkKhaki,	0xffBDB76B)
__REG_COLOR( paleGoldenrod,	0xffEEE8AA)
__REG_COLOR( yellow,	0xffFFFF00)
__REG_COLOR( gold,	0xffFFD700)
__REG_COLOR( rosyBrown,	0xffBC8F8F)
__REG_COLOR( indianRed,	0xffCD5C5C)
__REG_COLOR( sienna,	0xffA0522D)
__REG_COLOR( peru,	0xffCD853F)
__REG_COLOR( wheat,	0xffF5DEB3)
__REG_COLOR( sandyBrown,	0xffF4A460)
__REG_COLOR( tan,	0xffD2B48C)
__REG_COLOR( chocolate,	0xffD2691E)
__REG_COLOR( firebrick,	0xffB22222)
__REG_COLOR( brown,	0xffA52A2A)
__REG_COLOR( salmon,	0xffFA8072)
__REG_COLOR( orange,	0xffFFA500)
__REG_COLOR( darkOrange,	0xffFF8C00)
__REG_COLOR( coral,	0xffFF7F50)
__REG_COLOR( tomato,	0xffFF6347)
__REG_COLOR( orangeRed,	0xffFF4500)
__REG_COLOR( hotPink,	0xffFF69B4)
__REG_COLOR( deepPink,	0xffFF1493)
__REG_COLOR( pink,	0xffFFC0CB)
__REG_COLOR( maroon,	0xffB03060 )
__REG_COLOR( magenta,	0xffFF00FF )
__REG_COLOR( violet,	0xffEE82EE )
__REG_COLOR( orchid,	0xffDA70D6 )
__REG_COLOR( purple,	0xffA020F0 )
__REG_COLOR( snow, 0xffFFFAFA )
__REG_COLOR( dodgerBlue, 0xff1E90FF )
__REG_COLOR( steelBlue, 0xff63B8FF )
__REG_COLOR( slateGray, 0xffC6E2FF )
__REG_COLOR( aquamarine, 0xff7FFFD4 )
__REG_COLOR( seaGreen, 0xff54FF9F )
__REG_COLOR( springGreen, 0xff00FF7F )
__REG_COLOR( chartreuse, 0xff7FFF00 )
__REG_COLOR( plum, 0xffFFBBFF )
__REG_COLOR( thistle, 0xffFFE1FF )
}

#endif //__CAESARIA_COLOR_H_INCLUDED__
