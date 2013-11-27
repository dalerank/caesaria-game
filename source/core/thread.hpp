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

#ifndef _CAESARIA_THREAD_H_INCLUDE_
#define _CAESARIA_THREAD_H_INCLUDE_

#include "platform.hpp"
#include "smartptr.hpp"
#include "referencecounted.hpp"

#ifdef CAESARIA_PLATFORM_LINUX
	#include <stdio.h>
	#include <malloc.h>
	#include <memory.h>
	#include <pthread.h>
	#include <stdlib.h>
	#include <time.h>
	#include <errno.h>
#elif defined(CAESARIA_PLATFORM_WIN)
	#include <windows.h>
	#include <stdio.h>
#endif

/*#if defined(AS400) || defined(OS400)
	typedef pthread_id_np_t ThreadId_t;
#elif defined(VMS)
	typedef pthread_t ThreadId_t;
#elif defined(USE_BEGIN_THREAD)
#endif*/

#include "mutex.hpp"
#include "threadevent.hpp"
#include "threadtask.hpp"
#include "predefinitions.hpp"

#define QUEUE_SIZE 100
#define DEFAULT_STACK_SIZE 0

typedef enum
{
	ThreadStateBusy,               // thread is currently handling a task
	ThreadStateWaiting,            // thread is waiting for something to do
	ThreadStateDown,               // thread is not running
	ThreadStateShuttingDown,       // thread is in the process of shutting down
	ThreadStateFault               // an error has occured and the thread could not
																 // be launched
} ThreadState_t;

typedef enum
{
	ThreadTypeHomogeneous,
	ThreadTypeSpecialized,
	ThreadTypeIntervalDriven,
	ThreadTypeNotDefined
} ThreadType_t;

class Thread : public ReferenceCounted
{
private:
	CEventClass   m_event;         // event controller
	int           m_StopTimeout;   // specifies a timeout value for stop
																 // if a thread fails to stop within m_StopTimeout
																 // seconds an exception is thrown
	bool		     m_bRunning;					// set to TRUE if thread is running

#ifdef CAESARIA_PLATFORM_WIN
	HANDLE		  m_thread;		   // thread handle
#else
	pthread_t     m_thread;        // thread handle
#endif
	ThreadID	    m_dwId;          // id of this thread
	void**        m_lppvQueue;    // task que
	unsigned int  m_chQueue;       // que depth
	unsigned int  m_queuePos;      // current que possition
	void*         m_lpvProcessor;  // data which is currently being processed
	ThreadState_t m_state;         // current state of thread see thread state data
																 // structure.
	unsigned int  m_dwIdle;        // used for Sleep periods
	ThreadType_t  m_type;
	unsigned int  m_stackSize;     // thread stack size
#define NO_ERRORS			       0
#define MUTEX_CREATION		       0x01
#define EVENT_CREATION		       0x02
#define THREAD_CREATION		       0x04
#define UNKNOWN					   0x08
#define ILLEGAL_USE_OF_EVENT       0x10
#define MEMORY_FAULT               0x20
#define EVENT_AND_TYPE_DONT_MATCH  0x40
#define STACK_OVERFLOW             0x80
#define STACK_EMPTY                0x100
#define STACK_FULL                 0x200

	unsigned int     m_dwObjectCondition;
	bool		  push(void* lpv);
	bool		  pop();
	bool		  empty();
public:
	/**
	 *
	 * user definable member functions
	 *
	 **/
	Mutex	  m_mutex;         // mutex that protects threads internal data

	virtual bool OnTask(void* lpvData);     // called when an event occurs
	virtual bool OnTask();                   // called when a time interval has elapsed

	Thread(void);
	~Thread(void);

	static void	msleep(unsigned int milli);
#ifdef CAESARIA_PLATFORM_WIN
	friend long unsigned int WINAPI _THKERNEL( LPVOID lpvData );
#else
	friend void* _THKERNEL( void* lpvData );
#endif

	bool FromSameThread();
	float		PercentCapacity();
	void    WaitTillExit();
	bool		KernelProcess();
	bool		Event(void* lpvData=NULL);
	bool    Event(CTask *pvTask);
	void		SetOnStopTimeout(int seconds ) { m_StopTimeout = seconds; }
	bool    SetQueueSize( unsigned int ch );
	bool		Stop();
	bool		Start();
	void		getID(ThreadID *pId);      // returns thread id
	ThreadState_t ThreadState();
	bool		PingThread(unsigned int dwTimeout=0);
	bool    AtCapacity();
#ifdef WINDOWS
	void		SetPriority(DWORD dwPriority=THREAD_PRIORITY_NORMAL);
#else
	void		SetPriority(unsigned int dwPriority=0);
#endif
	unsigned int GetErrorFlags() { return m_dwObjectCondition; } // returns state of object
	void		SetThreadType(ThreadType_t typ=ThreadTypeNotDefined,unsigned int dwIdle=100);
	void		SetIdle(unsigned int dwIdle=100);
	unsigned int GetEventsPending();
	static bool ThreadIdsEqual(ThreadID *p1, ThreadID *p2);

	static ThreadID getID();

};

typedef SmartPtr<Thread> ThreadPtr;
#endif
