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

#include "thread.hpp"
#include "core/requirements.hpp"
#include "core/osystem.hpp"

#include <memory.h>
#include <errno.h>

void Thread::msleep( unsigned int milli )
{
  std::this_thread::sleep_for(std::chrono::milliseconds(milli));
}

#include <iostream>
using namespace std;

/**
 * 
 * _THKERNEL
 * thread callback function used by CreateThread
 *
 *
 **/
void _THKERNEL( void* lpvData )
{
	Thread *pThread = (Thread *)lpvData;
	/*
	 *
	 * initialization
	 *
	 */

  pThread->_mutex.lock();
  pThread->_state = ThreadStateWaiting;
  pThread->_bRunning = true;
  pThread->_dwId = Thread::getID();
  pThread->_mutex.unlock();
	
	while( true )
	{
    ThreadType_t  lastType = pThread->_type;

		if( lastType == ThreadTypeHomogeneous ||
			lastType == ThreadTypeSpecialized ||
			lastType == ThreadTypeNotDefined )
		{
			if( ! pThread->m_event.wait()  )  // wait for a message
					break;
			pThread->m_event.reset(); // message recieved
		}
	
		if( ! pThread->KernelProcess() ) 
				break;


		/*if( lastType == ThreadTypeHomogeneous ||
				lastType == ThreadTypeSpecialized ||
				lastType == ThreadTypeNotDefined )
		{
			pThread->m_event.Reset();
		}*/

    if( pThread->_type == ThreadTypeIntervalDriven )
		{
      Thread::msleep(pThread->_dwIdle);
		}
	}


  pThread->_mutex.lock();
  pThread->_state = ThreadStateDown;
  pThread->_bRunning = false;
  pThread->_mutex.unlock();

  return;
}

/**
 *
 * FromSameThread
 * determines if the calling thread is the same
 * as the thread assoicated with the object
 *
 **/
bool Thread::FromSameThread()
{
  std::thread::id id = getID();
  return threadIdsEqual(&id,&_dwId);
}

/**
 *
 * OnTask
 * called when a thread is tasked using the Event
 * member function
 *
 **/
bool Thread::OnTask( void* lpvData )
{
  _CAESARIA_DEBUG_BREAK_IF(lpvData && _type == ThreadTypeHomogeneous);

  if( _type != ThreadTypeHomogeneous )
	{
		cerr << "Warning CThread::OnTask:\n\tOnTask(LPVOID) called for a non-homogeneous thread!\n";
		return false;
	}

	((ThreadTask *)lpvData)->setTaskStatus(TaskStatusBeingProcessed);
	bool bReturn = ((ThreadTask *)lpvData)->task();
	((ThreadTask *)lpvData)->setTaskStatus(TaskStatusCompleted);

	return bReturn; 
} 

/**
 *
 * OnTask
 * overloaded implementation of OnTask that
 * takes no arguments
 *
 **/
bool Thread::OnTask()
{
  _CAESARIA_DEBUG_BREAK_IF(_type == ThreadTypeIntervalDriven);

  if( _type != ThreadTypeIntervalDriven )
	{
		cerr << "Warning CThread::OnTask:\n\tOnTask() called for a non-event driven thread!\n";
		return false;
	}

	printf("\nthread is alive\n");

	return true;
}

/**
 *
 * CEvent
 * used to place tasks on the threads event queue
 * wakes up thread.
 *
 **/
bool Thread::Event(ThreadTask *pvTask )
{
  _mutex.lock();

  _CAESARIA_DEBUG_BREAK_IF(_type == ThreadTypeHomogeneous ||
                           _type == ThreadTypeNotDefined );

	try 
	{
		if( FromSameThread() )
		{
			throw "\n\tit is illegal for a thread to place an event on its own event stack!\n";
		}


		// make sure that the thread is running 
    if( !_bRunning && m_dwObjectCondition == NO_ERRORS )
		{
      _mutex.unlock();
      PingThread(_dwIdle*2); // wait two idle cycles for it to start
      _mutex.lock();
		}
    if( !_bRunning ) // if it is not running return FALSE;
		{
      _mutex.unlock();
			return false;
		}


		if( m_dwObjectCondition & ILLEGAL_USE_OF_EVENT )
			m_dwObjectCondition = m_dwObjectCondition ^ ILLEGAL_USE_OF_EVENT;
		if( m_dwObjectCondition & EVENT_AND_TYPE_DONT_MATCH)
			m_dwObjectCondition = m_dwObjectCondition ^ EVENT_AND_TYPE_DONT_MATCH;

    if( _type != ThreadTypeHomogeneous &&
      _type != ThreadTypeNotDefined    )
		{
      _mutex.unlock();
			m_dwObjectCondition |= ILLEGAL_USE_OF_EVENT;
			m_dwObjectCondition |= EVENT_AND_TYPE_DONT_MATCH;
      _state = ThreadStateFault;
			cerr << "Warning: invalid call to CEvent::Event(CTask *), thread type is not specialized\n";

			return false;
		}

    _type = ThreadTypeHomogeneous;
    _mutex.unlock();

    pvTask->setId(&_dwId);
		if( !push((void*)pvTask) )
			return false;

		pvTask->setTaskStatus(TaskStatusWaitingOnQueue);
		m_event.set();

	}
	catch (char *psz)
	{
    OSystem::error( "Error", "Fatal exception Thread::Event" );
	}
	return true;
}

/**
 *
 * Event
 * used to place tasks on the threads event queue
 * wakes up thread.
 *
 **/
bool Thread::Event(void* lpvData )
{
  _mutex.lock();
  _CAESARIA_DEBUG_BREAK_IF( _type == ThreadTypeSpecialized ||
                            _type == ThreadTypeNotDefined );
	try 
	{
		if( FromSameThread() )
		{
			throw "\n\tit is illegal for a thread to place an event on its own event stack!\n";
		}
	}
	catch (char *psz)
	{
    OSystem::error( "Error",  "Fatal exception Thread::Event(LPVOID lpvData)" );
	}

	// make sure that the thread is running 
  if( !_bRunning && m_dwObjectCondition == NO_ERRORS )
	{
    _mutex.unlock();
    PingThread(_dwIdle*2); // wait two idle cycles for it to start
    _mutex.lock();
	}
  if( !_bRunning ) // if it is not running return FALSE;
	{
    _mutex.unlock();
		return false;
	}

	if( m_dwObjectCondition & ILLEGAL_USE_OF_EVENT )
	{
		m_dwObjectCondition = m_dwObjectCondition ^ ILLEGAL_USE_OF_EVENT;
	}

	if( m_dwObjectCondition & EVENT_AND_TYPE_DONT_MATCH)
	{
		m_dwObjectCondition = m_dwObjectCondition ^ EVENT_AND_TYPE_DONT_MATCH;
	}

  if( _type != ThreadTypeSpecialized && _type != ThreadTypeNotDefined )
	{
		m_dwObjectCondition |= ILLEGAL_USE_OF_EVENT;
		m_dwObjectCondition |= EVENT_AND_TYPE_DONT_MATCH;
		cerr << "Warning: invalid call to CEvent::Event(LPVOID), thread type is not specialized\n";
    _mutex.unlock();
		return false;
	}
  _type = ThreadTypeSpecialized;

  _mutex.unlock();
	if( !push(lpvData) )
	{
		return false;
	}

	m_event.set();

	return true;
}

/**
 *
 * KernelProcess
 * routes thread activity
 *
 **/
bool Thread::KernelProcess()
{
  _mutex.lock();
  _state = ThreadStateBusy;
  if( !_bRunning )
	{
    _state = ThreadStateShuttingDown;
    _mutex.unlock();
		return false;
	}
  _mutex.unlock();

	if( !empty() )
	{
		while( !empty() )
		{
			pop();
      if( !OnTask(_lpvProcessor) )
			{
        _mutex.lock();
        _lpvProcessor = NULL;
        _state = ThreadStateShuttingDown;
        _mutex.unlock();
				return false;
			}
		}
    _mutex.lock();
    _lpvProcessor = NULL;
    _state = ThreadStateWaiting;
	}
	else
	{
		if( !OnTask() )
		{
      _mutex.lock();
      _state = ThreadStateShuttingDown;
      _mutex.unlock();
			return false;
		}
    _mutex.lock();
    _state = ThreadStateWaiting;
	}

  _mutex.unlock();

	return true;
}

/**
 * 
 * GetEventsPending
 * returns the total number of vents waiting
 * in the event que
 * 
 **/
unsigned int Thread::GetEventsPending()
{
	unsigned int chEventsWaiting;

  _mutex.lock();
  chEventsWaiting = _queuePos;
  _mutex.unlock();

	return chEventsWaiting;
}

/**
 *
 * CThread
 * instanciates thread object and
 * starts thread.
 *
 **/
Thread::Thread(void)
:m_StopTimeout(30)
,_bRunning(false)
,_dwId(0L)
,_lppvQueue(NULL)
,_chQueue(QUEUE_SIZE)
,_queuePos(0)
,_lpvProcessor(NULL)
,_state(ThreadStateDown)
,_dwIdle(100)
,_type(ThreadTypeNotDefined)
,_stackSize(DEFAULT_STACK_SIZE)
{

	m_dwObjectCondition = NO_ERRORS;

  _lppvQueue = new void*[QUEUE_SIZE];

  if( !_lppvQueue )
	{
		m_dwObjectCondition |= MEMORY_FAULT;
    _state = ThreadStateFault;
		return;
	}

	if( !m_event.m_bCreated )
	{
		perror("event creation failed");
		m_dwObjectCondition |= EVENT_CREATION;
    _state = ThreadStateFault;
		return;
	}

	Start();
}


/**
 *
 * PercentCapacity
 * returns a floating point value identifying
 * the current workload of the thread
 *
 **/
float
Thread::PercentCapacity()
{
	float fValue = 0;
  _mutex.lock();
    fValue = (float)_queuePos/_chQueue;
  _mutex.unlock();
	return fValue;
}

/**
 *
 * SetQueueSize
 * changes the threads queue size
 *
 **/
bool Thread::SetQueueSize( unsigned int ch )
{
	void** newQueue = NULL;

  _mutex.lock();
  _CAESARIA_DEBUG_BREAK_IF(ch > _queuePos);

  if( ch <= _queuePos )
	{
		cerr << "Warning CThread::SetQueueSize:\n\tthe new queue size is less than the number of tasks on a non-empty queue! Request ignored.\n";
    _mutex.unlock();
		return false;
	}

	newQueue = new void*[ch];
	if(  !newQueue )
	{
		cerr << "Warning CThread::SetQueueSize:\n\ta low memory, could not reallocate queue!\n";
    _mutex.unlock();
		return false;
	}

  for( unsigned int i=0;i<_queuePos; i++ )
	{
    newQueue[i] = _lppvQueue[i];
	}
  delete [] _lppvQueue;

  _chQueue = ch;
  _lppvQueue = newQueue;

  _mutex.unlock();

	return true;
}



/**
 *
 * Empty
 * returns a value of TRUE if there are no items on the threads que
 * otherwise a value of FALSE is returned.
 *
 **/
bool Thread::empty()
{
  _mutex.lock();
  if( _queuePos <= 0 )
	{
    _mutex.unlock();
		return true;
	}
  _mutex.unlock();
	return false;
}



/**
 *
 * Push
 * place a data object in the threads que
 *
 **/
bool Thread::push(void* lpv )
{
	if( !lpv ) return true;

  _mutex.lock();

  if( _queuePos+1 >= _chQueue )
	{
		m_dwObjectCondition |= STACK_OVERFLOW;
    _mutex.unlock();
		return false;
	}

	if( m_dwObjectCondition & STACK_EMPTY )
	{
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_EMPTY;
	}

	if( m_dwObjectCondition & STACK_OVERFLOW ) 
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_OVERFLOW;

  _lppvQueue[_queuePos++] = lpv;
  if( _queuePos+1 >= _chQueue )
		m_dwObjectCondition |= STACK_FULL;

  _mutex.unlock();
	return true;
}


/**
 *
 * Pop
 * move an object from the input que to the processor
 *
 **/
bool Thread::pop()
{
  _mutex.lock();
  if( _queuePos-1 < 0 )
	{
    _queuePos = 0;
		m_dwObjectCondition |= STACK_EMPTY;
    _mutex.unlock();
		return false;
	}
	if( m_dwObjectCondition & STACK_EMPTY )
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_EMPTY;
	if( m_dwObjectCondition & STACK_OVERFLOW )
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_OVERFLOW;
	if( m_dwObjectCondition & STACK_FULL )
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_FULL;

  _queuePos--;
  _lpvProcessor = _lppvQueue[_queuePos];
  _mutex.unlock();
	return true;
}


/**
 *
 * SetThreadType
 * specifies the type of threading that is to be performed.
 *
 * ThreadTypeEventDriven (default): an event must be physically sent
 *									to the thread using the Event member
 *									function.
 *
 * ThreadTypeIntervalDriven       : an event occurs automatically every 
 *                                  dwIdle milli seconds.
 *
 **/
void
Thread::SetThreadType(ThreadType_t typ, unsigned int dwIdle)
{
	try 
	{
		if( FromSameThread() )
		{
			throw "\n\tit is illegal for a thread to change its own type!\n";
		}


    _mutex.lock();
    _dwIdle = dwIdle;


    if( _type == typ ) {
      _mutex.unlock();
			return;
		}
		if( m_dwObjectCondition & ILLEGAL_USE_OF_EVENT )
			m_dwObjectCondition = m_dwObjectCondition ^ ILLEGAL_USE_OF_EVENT;
		if( m_dwObjectCondition & EVENT_AND_TYPE_DONT_MATCH )
			m_dwObjectCondition = m_dwObjectCondition ^ EVENT_AND_TYPE_DONT_MATCH;

    _type = typ;


    _mutex.unlock();
		m_event.set();
	}
	catch (char *psz)
	{
    OSystem::error( "Error", "Fatal exception CThread::SetThreadType(ThreadType_t typ):" );
	}
}


/**
 *
 * Stop
 * stop thread 
 *
 **/
bool Thread::Stop()
{
	try 
	{
		if( FromSameThread() )
		{
			throw "\n\tit is illegal for a thread to attempt to signal itself to stop!\n";
		}

    _mutex.lock();
    _bRunning = false;
    _mutex.unlock();
		m_event.set();

		int ticks = (m_StopTimeout*1000)/100;

		for( int i=0; i<ticks; i++ )
		{
			msleep(100);

      _mutex.lock();
      if( _state == ThreadStateDown )
			{
        _mutex.unlock();
				return true;
			}
      _mutex.unlock();
		} 
	}
	catch (char *psz)
	{
    OSystem::error( "Error", "Fatal exception CThread::Stop():" );
	}
	return false;
}


/**
 *
 * SetIdle
 * changes the threads idle interval
 *
 **/
void Thread::SetIdle(unsigned int dwIdle)
{
  _mutex.lock();
  _dwIdle = dwIdle;
  _mutex.unlock();
}

/**
 *
 * Start
 * start thread
 *
 **/
bool Thread::Start()
{
	try 
	{
		if( FromSameThread() )
		{
			throw "\n\tit is illegal for a thread to attempt to start itself!\n";
		}


    _mutex.lock();
    if( _bRunning )
		{
      _mutex.unlock();
			return true;
		}

    _mutex.unlock();


		if( m_dwObjectCondition & THREAD_CREATION )
			m_dwObjectCondition = m_dwObjectCondition ^ THREAD_CREATION;

    _thread = std::thread(_THKERNEL,(void*)this);

    if( !_thread.joinable() )
		{
			m_dwObjectCondition |= THREAD_CREATION;
      _state = ThreadStateFault;

      /*switch(error)// show the thread error
			{
			case EINVAL: cerr << "error: attr in an invalid thread attributes object\n";	break;
			case EAGAIN: cerr << "error: the necessary resources to create a thread are not available.\n";	break;
			case EPERM:	cerr << "error: the caller does not have the privileges to create the thread with the specified attr object.\n"; break;
			default: cerr << "error: an unknown error was encountered attempting to create the requested thread.\n"; break;
      }*/

			return false;
		}

    _thread.join();
	}
	catch( char *psz )
	{
    OSystem::error( "Error", "Fatal exception Thread::Start():" );
	}

	return true;
}

void Thread::getID(std::thread::id* pId)
{
  memcpy(pId,&_dwId,sizeof(std::thread::id));
}

/**
 *
 * AtCapacity
 * returns TRUE if the threads queue is full, and the thread
 * is busy processing an event or the thread is not running
 *
 **/
bool Thread::AtCapacity()
{
  _mutex.lock();
  if( ((m_dwObjectCondition & STACK_OVERFLOW ||
      m_dwObjectCondition & STACK_FULL ) &&
      _state == ThreadStateBusy) || !_bRunning)
  {
    _mutex.unlock();
    return true;
  }
  _mutex.unlock();
	return false;
}

/**
 *
 * ThreadState
 * return the current state of the thread
 *
 **/
ThreadState_t 
Thread::ThreadState()
{
	ThreadState_t currentState;
  _mutex.lock();
    currentState = _state;
  _mutex.unlock();
	return currentState;
}

/**
 *
 * ~CThread
 * destructor.  Stop should be called prior to destruction to
 * allow for gracefull thread termination.
 *
 **/
Thread::~Thread(void)
{
  if( _bRunning ) // gracefull termination
	{
		try 
		{
			if( !Stop() )
			{
				throw "\n\tthread failed to stop in a timely manner!\n";
			}
		}
		catch( char *psz )
		{
      OSystem::error( "Error", "Fatal exception Thread::Stop: " );
		}
	}
  delete [] _lppvQueue;
}


/**
 *
 * PingThread
 * used to determine if a thread is running
 *
 **/
bool Thread::PingThread(unsigned int milli )
{
	unsigned int dwTotal = 0;

	while( true )
	{
		if( dwTotal > milli && milli > 0 )
			return false;
    _mutex.lock();
    if( _bRunning )
		{
      _mutex.unlock();
			return true;
		}
    dwTotal += _dwIdle;
    _mutex.unlock();
    msleep(_dwIdle);
	}

	return false;
}

/**
 *
 * WaitTillExit
 * blocks caller until thread exits
 *
 **/
void
Thread::WaitTillExit()
{
	/*
	 *
	 * prevent users from calling this function from within the same thread
	 * of execution
	 *
	 */
	try 
	{
		if( FromSameThread() )
			throw "\n\tthis function can not be called from within the same thread!\n";

    if( !_bRunning )
      return;

    _thread.join();
	}
	catch( char *psz )
	{
    OSystem::error( "Error", "Fatal exception CThread::WaitTillExit: " );
	}
}

bool Thread::threadIdsEqual(std::thread::id* p1, std::thread::id* p2)

{
	return ((*p1 == *p2)?true:false);
}

std::thread::id Thread::getID()
{
  std::thread::id thisThreadsId = std::this_thread::get_id();
	return thisThreadsId;
}
