#ifndef _CAESARIA_MUTEX_H_INCLUDE_
#define _CAESARIA_MUTEX_H_INCLUDE_

#include "platform.hpp"
#include "predefinitions.hpp"

#ifdef CAESARIA_PLATFORM_LINUX
	#include <pthread.h>
#elif defined(CAESARIA_PLATFORM_WIN)
	#include <windows.h>
#endif

class Mutex
{
private:

#ifdef CAESARIA_PLATFORM_WIN
	HANDLE m_mutex;
#elif defined(CAESARIA_PLATFORM_LINUX)
	pthread_mutex_t m_mutex;
#endif

	ThreadID m_owner;
public:
	bool m_bCreated;

	void lock();
	void unlock();

	Mutex(void);
	~Mutex(void);
};

class MutexLocker
{
public:
	MutexLocker(Mutex* mutex)
	{
		_mutex = mutex;
		if( _mutex )
			_mutex->lock();
	}

	~MutexLocker()
	{
		if( _mutex )
			_mutex->unlock();
	}

private:
	Mutex* _mutex;
};

#endif //_CAESARIA_MUTEX_H_INCLUDE_

