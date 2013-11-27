#include "threadevent.hpp"
#include "thread.hpp"

#include <iostream>
#include <string.h>
using namespace std;

CEventClass::CEventClass(void) : m_bCreated(true)
{
	memset(&m_owner,0,sizeof(ThreadID));
#ifdef CAESARIA_PLATFORM_WIN
	m_event = CreateEvent(NULL,FALSE,FALSE,NULL);
	if( !m_event )
	{
		m_bCreated = FALSE;
	}
#else
	pthread_mutexattr_t mattr;
	
	pthread_mutexattr_init(&mattr);
	pthread_mutex_init(&m_lock,&mattr);
	pthread_cond_init(&m_ready,NULL);

#endif	
}

CEventClass::~CEventClass(void)
{
#ifdef CAESARIA_PLATFORM_WIN
	CloseHandle(m_event);
#else
	pthread_cond_destroy(&m_ready);
	pthread_mutex_destroy(&m_lock);
#endif
}


/**
 *
 * Set
 * set an event to signaled
 *
 **/
void
CEventClass::set()
{
#ifdef CAESARIA_PLATFORM_WIN
	SetEvent(m_event);
#else
	pthread_cond_signal(&m_ready);
#endif
}

/**
 *
 * Wait
 * wait for an event -- wait for an event object
 * to be set to signaled.  must be paired with a
 * call to reset within the same thread.
 *
 **/
bool CEventClass::wait()
{
	try
	{
		ThreadID id = Thread::getID();
		if( Thread::ThreadIdsEqual(&id,&m_owner) )
		{
			throw "\n\tinvalid Wait call, Wait can not be called more than once"
				"\n\twithout a corresponding call to Reset!\n";
		}
		ThreadID zero;
		memset(&zero,0,sizeof(ThreadID));

		if( memcmp(&zero,&m_owner,sizeof(ThreadID)) != 0 )
		{
			throw "\n\tanother thread is already waiting on this event!\n";
		}

		m_owner = Thread::getID();
#ifdef CAESARIA_PLATFORM_WIN
		if( WaitForSingleObject(m_event,INFINITE) != WAIT_OBJECT_0 )
		{
			return FALSE;
		}
#else
		pthread_mutex_lock(&m_lock);
		pthread_cond_wait(&m_ready,&m_lock);
		return true;
#endif
	}
	catch( char *psz )
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CEventClass::Wait",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CEventClass::Wait: " << psz;
#endif

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
CEventClass::reset()
{
	try 
	{
		ThreadID id = Thread::getID();
		if( !Thread::ThreadIdsEqual(&id,&m_owner) )
		{
			throw "\n\tunbalanced call to Reset, Reset must be called from\n"
				  "\n\tthe same Wait-Reset pair!\n";
		}

		memset(&m_owner,0,sizeof(ThreadID));

#ifndef CAESARIA_PLATFORM_WIN
		pthread_mutex_unlock(&m_lock);
#endif
	}
	catch( char *psz )
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CEventClass::Reset",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CEventClass::Reset: " << psz;
#endif

	}
}

