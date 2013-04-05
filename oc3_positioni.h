#ifndef __OPENCAESAR3_POSITION_H_INCLUDED__
#define __OPENCAESAR3_POSITION_H_INCLUDED__

#include "oc3_vector2.h"

class Point : public Vector2<int>
{
public:
    Point( const int x, const int y ) : Vector2<int>( x, y ) {}
    Point() : Vector2<int>( 0, 0 ) {}
};

class PointF : public Vector2<float>
{

};

class TilePos : public Vector2<int>
{
public:
    TilePos( const int i, const int j ) : Vector2<int>( i, j ) {}
    TilePos() : Vector2<int>( 0, 0 ) {}

    int getI() const { return x; }
    int getJ() const { return y; }

    TilePos& operator=(const TilePos& other) { set( other.x, other.y ); return *this; }
    TilePos operator+(const TilePos& other) const { return TilePos( x + other.x, y + other.y ); }
};
#endif // __NRP_POSITION_H_INCLUDED__
