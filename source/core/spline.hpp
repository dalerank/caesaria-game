#include <vector>

/**  A 2D cubic spline : draws a set of smooth curves through points */
class Spline
{

public:

    /**  Constructor: Calculates the spline curve coefficients

    @param[in] x  The x points
    @param[in] y  The y points

    */
    Spline( std::vector< double >& x,
            std::vector< double >& y );

    /** Check if input is insane

    @return true if all OK
     */
    bool IsSane()      { return (bool) !myError; }

    /// error numbers
    enum e_error
    {
        no_error,
        x_not_ascending,
        no_input,
        not_single_valued,
    } myError;

    /** Check for error

    @return 0 if all OK, otherwise an error number

    */
    e_error IsError()   { return myError; }

    /**  Draw the spline curves

    @param[in]  the drawing function
    @param[in]  resolution The number of times to call the drawing function, default 100.

    This method will iterate between the minimum and maximum x values
    calling the drawing function 100 times with evenly spaced values of x
    and the y value of the spline curve for that x.

    Example of usage
    <pre>
    double xold = x0;
    double yold = spline.getY( x0 );
    spline.Draw( [&] ( double x, double y )
                {
                    dc.DrawLine( xold, yold, x, y );
                    xold = x;
                    yold = y;
                });

    </pre>
    */

    /** Get the Y value of the spline curves for a particular X
    @param[in] x
    @return the y value
    */
    double getY( double x);

private:

    // The fitted points
    std::vector< double > myX;
    std::vector< double > myY;

    /// The coefficients of the spline curve between two points
    struct SplineSet
    {
        double a;   // constant
        double b;   // 1st order coefficient
        double c;   // 2nd order coefficient
        double d;   // 3rd order coefficient
        double x;   // starting x value
    };

    /// The coefficients of the spline curves between all points
    std::vector< SplineSet > mySplineSet;

    bool IsInputSane();
};
