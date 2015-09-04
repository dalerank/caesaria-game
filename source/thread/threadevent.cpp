// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "threadevent.hpp"
#include "thread.hpp"
#include "core/osystem.hpp"

#include <iostream>
#include <string.h>
using namespace std;

ThreadEvent::ThreadEvent(void) : m_bCreated(true)
{
  memset(&_owner,0,sizeof(std::thread::id));
}

ThreadEvent::~ThreadEvent(void) {}

/**
 *
 * Set
 * set an event to signaled
 *
 **/
void
ThreadEvent::set()
{
  _ready.notify_all();
}

/**
 *
 * Wait
 * wait for an event -- wait for an event object
 * to be set to signaled.  must be paired with a
 * call to reset within the same thread.
 *
 **/
bool ThreadEvent::wait()
{
	try
	{
    std::thread::id id = Thread::getID();
    if( Thread::threadIdsEqual(&id,&_owner) )
		{
			throw "\n\tinvalid Wait call, Wait can not be called more than once"
				"\n\twithout a corresponding call to Reset!\n";
		}
    std::thread::id zero;
    memset(&zero,0,sizeof(std::thread::id));

    if( memcmp(&zero,&_owner,sizeof(std::thread::id)) != 0 )
		{
			throw "\n\tanother thread is already waiting on this event!\n";
		}

    _owner = Thread::getID();
    std::unique_lock<std::mutex> locker(_lock);
    _ready.wait( locker );
		return true;
	}
	catch( char *psz )
	{
    OSystem::error( "Error","Fatal exception CEventClass::Wait");
	}
	return true;
}

/**
 *
 * Reset
 * reset an event flag to unsignaled
 * wait must be paired with reset within the same thread.
 *
 **/
void
ThreadEvent::reset()
{
	try 
	{
    std::thread::id id = Thread::getID();
    if( !Thread::threadIdsEqual(&id,&_owner) )
		{
			throw "\n\tunbalanced call to Reset, Reset must be called from\n"
				  "\n\tthe same Wait-Reset pair!\n";
		}

    memset(&_owner,0,sizeof(std::thread::id));
    _lock.unlock();
	}
	catch( char *psz )
	{
    OSystem::error( "Error","Fatal exception CEventClass::Reset");
	}
}

