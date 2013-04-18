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


#ifndef __OPENCAESAR3_SIZE_H_INCLUDED__
#define __OPENCAESAR3_SIZE_H_INCLUDED__

#include "oc3_vector2.hpp"

class Size : public Vector2<int>
{
public:
    Size( const int w, const int h ) : Vector2<int>( w, h ) {}
    Size() : Vector2<int>( 0, 0 ) {}
    Size( const int s ) : Vector2<int>( s, s ) {}

    Size operator+(const Size& other) const { return Size( x + other.x, y + other.y ); }

    int getWidth() const { return x; }
    int getHeight() const { return y; }

    void setWidth( int w ) { x = w; }
    void setHeight( int h ) { y = h; }
};

#endif

