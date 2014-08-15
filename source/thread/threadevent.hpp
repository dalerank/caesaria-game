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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_THREADEVENT_H_INCLUDE_
#define _CAESARIA_THREADEVENT_H_INCLUDE_

#include "core/predefinitions.hpp"

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

