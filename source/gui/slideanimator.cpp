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
    anim->setStopPos( parentRect.lefttop() );
    Rect hRect = anim->parent()->parent()
                                ? anim->parent()->parent()->relativeRect()
                                : Rect( Point( 0, 0 ), anim->ui()->rootWidget()->size() );
    switch( mode )
    {
    case SlideAnimator::SlideTop: anim->setStopPos( Point( anim->getStopPos().x(), -parentRect.height() ) ); break;
    case SlideAnimator::SlideBottom: anim->setStopPos( Point( anim->getStopPos().x(), hRect.bottom() ) ); break;
    case SlideAnimator::SlideLeft: anim->setStopPos( Point( - parentRect.width(), anim->getStopPos().y() ) ); break;
    case SlideAnimator::SlideRight: anim->setStopPos( Point( hRect.right(), anim->getStopPos().y() ) ); break;
    case SlideAnimator::SlideTopLeft: anim->setStopPos( Point( -parentRect.height(), -parentRect.width() ) ); break;
    case SlideAnimator::SlideTopRight: anim->setStopPos( Point( -parentRect.height(), hRect.right() ) ); break;
    case SlideAnimator::SlideBottomLeft: anim->setStopPos( Point( -parentRect.width(), hRect.bottom() ) ); break;
    case SlideAnimator::SlideBottomRight: anim->setStopPos( Point( hRect.right(), hRect.bottom() ) ); break;
    }
  }

  if( invert )
  {
    Point tmp = anim->getStartPos();
    anim->setStartPos( anim->getStopPos() );
    anim->setStopPos( tmp );
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
