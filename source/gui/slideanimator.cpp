#include "slideanimator.hpp"
#include "environment.hpp"

namespace gui
{

class SlideAnimator::Impl
{
public:
	Mode mode;
	bool invert;

	Rect saveStartRect;

	void checkMode( SlideAnimator* anim );
};

SlideAnimator::SlideAnimator(Widget* node,
    int flags,
    Mode mode,
    int time )
    : PositionAnimator( node, flags, Point( 0, 0 ), time ),
	  _d( new Impl )      
{
	_d->mode = mode;
	_d->invert = false;
  _d->checkMode( this );
}

void SlideAnimator::Impl::checkMode( SlideAnimator* anim )
{
  if( anim->parent() )
  {
    Rect parentRect = anim->parent()->relativeRect();
    anim->setDestination( parentRect.lefttop() );
    Rect hRect = anim->parent()->parent()
                                ? anim->parent()->parent()->relativeRect()
                                : Rect( Point( 0, 0 ), anim->ui()->rootWidget()->size() );
    switch( mode )
    {
    case SlideAnimator::SlideTop: anim->setDestination( Point( anim->destination().x(), -parentRect.height() ) ); break;
    case SlideAnimator::SlideBottom: anim->setDestination( Point( anim->destination().x(), hRect.bottom() ) ); break;
    case SlideAnimator::SlideLeft: anim->setDestination( Point( - parentRect.width(), anim->destination().y() ) ); break;
    case SlideAnimator::SlideRight: anim->setDestination( Point( hRect.right(), anim->destination().y() ) ); break;
    case SlideAnimator::SlideTopLeft: anim->setDestination( Point( -parentRect.height(), -parentRect.width() ) ); break;
    case SlideAnimator::SlideTopRight: anim->setDestination( Point( -parentRect.height(), hRect.right() ) ); break;
    case SlideAnimator::SlideBottomLeft: anim->setDestination( Point( -parentRect.width(), hRect.bottom() ) ); break;
    case SlideAnimator::SlideBottomRight: anim->setDestination( Point( hRect.right(), hRect.bottom() ) ); break;
    }
  }

  if( invert )
  {
    Point tmp = anim->getStartPos();
    anim->setStartPos( anim->destination() );
    anim->setDestination( tmp );
  }
}

SlideAnimator::~SlideAnimator(void)
{
}

void SlideAnimator::setInvert( bool invert )
{
	_d->invert = invert;
}

}//end namespace gui
