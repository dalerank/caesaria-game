#ifndef _CAESARIA_THREADEVENT_H_INCLUDE_
#define _CAESARIA_THREADEVENT_H_INCLUDE_

#include "predefinitions.hpp"

#ifdef CAESARIA_PLATFORM_WIN
#include "windows.h"
#else
#include <pthread.h>
#endif

class CEventClass
{
private:
	ThreadID m_owner;

#ifdef CAESARIA_PLATFORM_WIN
	HANDLE m_event;
#else
	pthread_cond_t m_ready;
	pthread_mutex_t m_lock;
#endif

public:
	bool m_bCreated;
	void set();
	bool wait();
	void reset();

	CEventClass(void);
	~CEventClass(void);
};

#endif //_CAESARIA_THREADEVENT_H_INCLUDE_

