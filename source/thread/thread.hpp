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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_THREAD_H_INCLUDE_
#define _CAESARIA_THREAD_H_INCLUDE_

#include <mutex>
#include <thread>
#include "threadevent.hpp"
#include "threadtask.hpp"
#include "core/predefinitions.hpp"
#include "core/platform.hpp"
#include "core/scopedptr.hpp"
#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"

#if defined(CAESARIA_PLATFORM_LINUX)
  #include <stdio.h>
  #include <malloc.h>
  #include <memory.h>
  #include <stdlib.h>
  #include <time.h>
  #include <errno.h>
#elif defined(CAESARIA_PLATFORM_MACOSX)
  #include <stdio.h>
  #include <memory.h>
  #include <stdlib.h>
  #include <time.h>
  #include <errno.h>
#elif defined(CAESARIA_PLATFORM_WIN)
	#include <windows.h>
	#include <stdio.h>
#endif

#define QUEUE_SIZE 100
#define DEFAULT_STACK_SIZE 0

namespace threading
{

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
  TypeHomogeneous,
  TypeSpecialized,
  TypeIntervalDriven,
  TypeNotDefined
} Type_t;

class Thread : public ReferenceCounted
{
private:
  class Impl;
  ScopedPtr<Impl> _d;

  int           m_StopTimeout;   // specifies a timeout value for stop
																 // if a thread fails to stop within m_StopTimeout
																 // seconds an exception is thrown
  bool		     _bRunning;					// set to TRUE if thread is running

  std::thread   _thread;        // thread handle
  std::thread::id	    _dwId;          // id of this thread
  void**        _lppvQueue;    // task que
  unsigned int  _chQueue;       // que depth
  int  _queuePos;      // current que possition
  void*         _lpvProcessor;  // data which is currently being processed
  ThreadState_t _state;         // current state of thread see thread state data
																 // structure.
  unsigned int  _dwIdle;        // used for Sleep periods
  Type_t  _type;
  unsigned int  _stackSize;     // thread stack size
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

  bool wait();
  void resetEvent();

	virtual bool OnTask(void* lpvData);     // called when an event occurs
	virtual bool OnTask();                   // called when a time interval has elapsed

	Thread(void);
  virtual ~Thread(void);

	static void	msleep(unsigned int milli);
  friend void _THKERNEL( void* lpvData );

	bool FromSameThread();
	float		PercentCapacity();
	void    WaitTillExit();
	bool		KernelProcess();
	bool		Event(void* lpvData=NULL);
  bool    Event(ThreadTask *pvTask);
	void		SetOnStopTimeout(int seconds ) { m_StopTimeout = seconds; }
	bool    SetQueueSize( unsigned int ch );
	bool		Stop();
	bool		Start();
  void		getID(std::thread::id *pId);      // returns thread id
	ThreadState_t ThreadState();
	bool		PingThread(unsigned int dwTimeout=0);
	bool    AtCapacity();
	unsigned int GetErrorFlags() { return m_dwObjectCondition; } // returns state of object
  void		SetThreadType(Type_t typ=TypeNotDefined,unsigned int dwIdle=100);
	void		SetIdle(unsigned int dwIdle=100);
	unsigned int GetEventsPending();
  static bool threadIdsEqual(std::thread::id *p1, std::thread::id *p2);

  static std::thread::id getID();
};

typedef SmartPtr<Thread> ThreadPtr;

}//end namespace threading
#endif
