#ifndef _OPENCAESAR3_EVENTCONVERTER_H_INCLUDE_
#define _OPENCAESAR3_EVENTCONVERTER_H_INCLUDE_

#include "oc3_event.h"
#include <memory>
#include <SDL_events.h>

class EventConverter
{
public:
    static EventConverter& instance();
	~EventConverter();

	NEvent get( const SDL_Event& event );

private:
    class Impl;
    std::auto_ptr< Impl > _d;
    
    EventConverter();
};

#endif
