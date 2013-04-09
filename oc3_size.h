#ifndef __OPENCAESAR3_SIZE_H_INCLUDED__
#define __OPENCAESAR3_SIZE_H_INCLUDED__

#include "oc3_vector2.h"

class Size : public Vector2<int>
{
public:
    Size( const int w, const int h ) : Vector2<int>( w, h ) {}
    Size() : Vector2<int>( 0, 0 ) {}

    Size operator+(const Size& other) const { return Size( x + other.x, y + other.y ); }

    int getWidth() const { return x; }
    int getHeight() const { return y; }

    void setWidth( int w ) { x = w; }
    void setHeight( int h ) { y = h; }
};

#endif

