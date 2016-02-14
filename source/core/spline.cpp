#include <algorithm>
#include <vector>

using namespace std;

#include "spline.hpp"
#include "core/math.hpp"

typedef std::vector< double > vd_t;

Spline::Spline(vd_t &x, vd_t &y)
{
    myX = x;
    myY = y;

    if( ! IsInputSane() )
        return;

    int n = x.size()-1;
    vd_t a;
    a.insert(a.begin(), y.begin(), y.end());
    vd_t b(n);
    vd_t d(n);
    vd_t h;

    for(int i = 0; i < n; ++i)
        h.push_back(x[i+1]-x[i]);

    vd_t alpha;
    for(int i = 0; i < n; ++i)
        alpha.push_back( 3*(a[i+1]-a[i])/h[i] - 3*(a[i]-a[i-1])/h[i-1]  );

    vd_t c(n+1);
    vd_t l(n+1);
    vd_t mu(n+1);
    vd_t z(n+1);
    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;

    for(int i = 1; i < n; ++i)
    {
        l[i] = 2 *(x[i+1]-x[i-1])-h[i-1]*mu[i-1];
        mu[i] = h[i]/l[i];
        z[i] = (alpha[i]-h[i-1]*z[i-1])/l[i];
    }

    l[n] = 1;
    z[n] = 0;
    c[n] = 0;

    for(int j = n-1; j >= 0; --j)
    {
        c[j] = z [j] - mu[j] * c[j+1];
        b[j] = (a[j+1]-a[j])/h[j]-h[j]*(c[j+1]+2*c[j])/3;
        d[j] = (c[j+1]-c[j])/3/h[j];
    }

    mySplineSet.resize(n);
    for(int i = 0; i < n; ++i)
    {
        mySplineSet[i].a = a[i];
        mySplineSet[i].b = b[i];
        mySplineSet[i].c = c[i];
        mySplineSet[i].d = d[i];
        mySplineSet[i].x = x[i];
    }
    return;
}

double Spline::getY( double x)
{
    size_t j;
    for ( j = 0; j < mySplineSet.size(); j++ )
    {
        if( mySplineSet[j].x > x )
        {
            if( j == 0 )
                j++;
            break;
        }
    }
    j--;

    double dx = x - mySplineSet[j].x;
    double y = mySplineSet[j].a + mySplineSet[j].b * dx + mySplineSet[j].c * dx* dx +
               mySplineSet[j].d * dx* dx * dx;

    return y;

}

bool Spline::IsInputSane()
{
    if( ! myX.size() || ! myY.size() )
    {
        myError = no_input;
        return false;
    }
    if( ! std::is_sorted( myX.begin(), myX.end() ))
    {
        myError = x_not_ascending;
        return false;
    }

    bool first = true;
    double xold;
    for( double x : myX )
    {
        if( first )
        {
            xold = x;
            continue;
        }
        first = false;
        if( math::abs( x - xold ) < 1 )
        {
            myError = not_single_valued;
            return false;
        }
        xold = x;
    }

    myError = no_error;
    return true;
}
