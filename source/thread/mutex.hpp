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

#ifndef _CAESARIA_MUTEX_H_INCLUDE_
#define _CAESARIA_MUTEX_H_INCLUDE_

#include "core/platform.hpp"
#include "core/predefinitions.hpp"

#ifdef CAESARIA_PLATFORM_UNIX
	#include <pthread.h>
#elif defined(CAESARIA_PLATFORM_WIN)
	#include <windows.h>
#endif

class Mutex
{
private:

#ifdef CAESARIA_PLATFORM_WIN
	HANDLE m_mutex;
#elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
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
	MutexLocker(Mutex* mutex);

	~MutexLocker();

private:
	Mutex* _mutex;
};

#endif //_CAESARIA_MUTEX_H_INCLUDE_

