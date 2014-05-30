#ifndef _CAESARIA_SLIDE_ANIMATOR_H_INCLUDE_
#define _CAESARIA_SLIDE_ANIMATOR_H_INCLUDE_

#include "widgetpositionanimator.hpp"

namespace gui
{

//! Names for blind 
const std::string SlideAnimatorTypeNames[] =
{
   "SlideTop",
   "SlideBottom",
   "SlideRight",
   "SlideLeft",
   "SlideTopRight",
   "SlideTopLeft",
   "SlideBottomRight",
   "SlideBottomLeft",
   "SlideCenter",
   ""
};

class SlideAnimator : public PositionAnimator
{
public:
    typedef enum { SlideTop=0, SlideBottom, SlideRight, SlideLeft, 
           SlideTopRight, SlideTopLeft, SlideBottomRight, SlideBottomLeft } Mode;

    SlideAnimator( Widget* node,
                   int flags,
                   Mode mode=SlideTop,
                   int time=1000 );
    ~SlideAnimator(void);

    void setInvert( bool invert );
    
private:
	class Impl;
	Impl* _d;
};

}//end namespace gui

#endif
