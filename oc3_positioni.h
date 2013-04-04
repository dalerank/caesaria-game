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
#endif // __NRP_POSITION_H_INCLUDED__
