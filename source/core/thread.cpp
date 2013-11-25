#include "thread.hpp"
#include "requirements.hpp"

#ifdef CAESARIA_PLATFORM_LINUX
extern "C"
{
 int	usleep(useconds_t useconds);
#ifdef NANO_SECOND_SLEEP
 int 	nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
#endif
}

void Thread::msleep( unsigned int milli )
{
#ifdef NANO_SECOND_SLEEP
	struct timespec interval, remainder;
	milli = milli * 1000000;
	interval.tv_sec= 0;
	interval.tv_nsec=milli;
	nanosleep(&interval,&remainder);
#else
	usleep(milli*1000);
#endif	
}
#elif defined(CAESARIA_PLATFORM_WIN)
void Thread::msleep( unsigned int milli )
{
	Sleep( milli );
}
#endif

#include <iostream>
using namespace std;

/**
 * 
 * _THKERNEL
 * thread callback function used by CreateThread
 *
 *
 **/
#ifdef CAESARIA_PLATFORM_WIN
long unsigned int WINAPI _THKERNEL( LPVOID lpvData )
#else
void* _THKERNEL( void* lpvData )
#endif
{
	Thread *pThread = (Thread *)lpvData;
	ThreadType_t lastType;
	/*
	 *
	 * initialization
	 *
	 */


		pThread->m_mutex.lock();
		pThread->m_state = ThreadStateWaiting;
		pThread->m_bRunning = true;
#ifndef CAESARIA_PLATFORM_WIN
		pThread->m_dwId = Thread::getID();
#endif
	pThread->m_mutex.unlock();
	
	while( true )
	{
		lastType = pThread->m_type;

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
		} */

		if( pThread->m_type == ThreadTypeIntervalDriven )
		{
			Thread::msleep(pThread->m_dwIdle);
		}
	}


	pThread->m_mutex.lock();
	pThread->m_state = ThreadStateDown;
	pThread->m_bRunning = false;
	pThread->m_mutex.unlock();


#ifdef CAESARIA_PLATFORM_WIN
	return 0;
#else
	return (void*)0;
#endif
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
	ThreadID id = getID();
	if( ThreadIdsEqual(&id,&m_dwId) ) return true;
	return false;
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
	_CAESARIA_DEBUG_BREAK_IF(lpvData && m_type == ThreadTypeHomogeneous);

	if( m_type != ThreadTypeHomogeneous )
	{
		cerr << "Warning CThread::OnTask:\n\tOnTask(LPVOID) called for a non-homogeneous thread!\n";
		return false;
	}

	((CTask *)lpvData)->setTaskStatus(TaskStatusBeingProcessed);
	bool bReturn = ((CTask *)lpvData)->task();
	((CTask *)lpvData)->setTaskStatus(TaskStatusCompleted);

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
	_CAESARIA_DEBUG_BREAK_IF(m_type == ThreadTypeIntervalDriven);

	if( m_type != ThreadTypeIntervalDriven )
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
bool Thread::Event(CTask *pvTask )
{
	m_mutex.lock();

	_CAESARIA_DEBUG_BREAK_IF(m_type == ThreadTypeHomogeneous ||
													 m_type == ThreadTypeNotDefined );

	try 
	{
		if( FromSameThread() )
		{
			throw "\n\tit is illegal for a thread to place an event on its own event stack!\n";
		}


		// make sure that the thread is running 
		if( !m_bRunning && m_dwObjectCondition == NO_ERRORS )
		{
			m_mutex.unlock();
			PingThread(m_dwIdle*2); // wait two idle cycles for it to start
			m_mutex.lock();
		}
		if( !m_bRunning ) // if it is not running return FALSE;
		{
			m_mutex.unlock();
			return false;
		}


		if( m_dwObjectCondition & ILLEGAL_USE_OF_EVENT )
			m_dwObjectCondition = m_dwObjectCondition ^ ILLEGAL_USE_OF_EVENT;
		if( m_dwObjectCondition & EVENT_AND_TYPE_DONT_MATCH)
			m_dwObjectCondition = m_dwObjectCondition ^ EVENT_AND_TYPE_DONT_MATCH;

		if( m_type != ThreadTypeHomogeneous &&
			m_type != ThreadTypeNotDefined    )
		{
			m_mutex.unlock();
			m_dwObjectCondition |= ILLEGAL_USE_OF_EVENT;
			m_dwObjectCondition |= EVENT_AND_TYPE_DONT_MATCH;
			m_state = ThreadStateFault;
			cerr << "Warning: invalid call to CEvent::Event(CTask *), thread type is not specialized\n";

			return false;
		}

		m_type = ThreadTypeHomogeneous;
		m_mutex.unlock();

		pvTask->setId(&m_dwId);
		if( !push((void*)pvTask) )
			return false;

		pvTask->setTaskStatus(TaskStatusWaitingOnQueue);
		m_event.set();

	}
	catch (char *psz)
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CThread::CEvent",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CThread::CEvent(CTask *pvTask):" << psz;
#endif

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
	m_mutex.lock();
	_CAESARIA_DEBUG_BREAK_IF( m_type == ThreadTypeSpecialized ||
														m_type == ThreadTypeNotDefined );
	try 
	{
		if( FromSameThread() )
		{
			throw "\n\tit is illegal for a thread to place an event on its own event stack!\n";
		}
	}
	catch (char *psz)
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CThread::CEvent",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CThread::CEvent(LPVOID lpvData):" << psz;
#endif

	}

	// make sure that the thread is running 
	if( !m_bRunning && m_dwObjectCondition == NO_ERRORS )
	{
		m_mutex.unlock();
		PingThread(m_dwIdle*2); // wait two idle cycles for it to start
		m_mutex.lock();
	}
	if( !m_bRunning ) // if it is not running return FALSE;
	{
		m_mutex.unlock();
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

	if( m_type != ThreadTypeSpecialized && m_type != ThreadTypeNotDefined )
	{
		m_dwObjectCondition |= ILLEGAL_USE_OF_EVENT;
		m_dwObjectCondition |= EVENT_AND_TYPE_DONT_MATCH;
		cerr << "Warning: invalid call to CEvent::Event(LPVOID), thread type is not specialized\n";
		m_mutex.unlock();
		return false;
	}
	m_type = ThreadTypeSpecialized;

	m_mutex.unlock();
	if( !push(lpvData) )
	{
		return false;
	}

	m_event.set();

	return true;
}


/**
 *
 * SetPriority
 * sets a threads run priority, see SetThreadPriority
 * Note: only works for Windows family of operating systems
 *
 *
 **/
void Thread::SetPriority(unsigned int dwPriority)
{
#ifdef CAESARIA_PLATFORM_WIN
	SetThreadPriority(m_thread,dwPriority);
#endif
}

	  
/**
 *
 * KernelProcess
 * routes thread activity
 *
 **/
bool Thread::KernelProcess()
{
	m_mutex.lock();
	m_state = ThreadStateBusy;
	if( !m_bRunning )
	{
		m_state = ThreadStateShuttingDown;
		m_mutex.unlock();
		return false;
	}
	m_mutex.unlock();

	if( !empty() )
	{
		while( !empty() )
		{
			pop();
			if( !OnTask(m_lpvProcessor) )
			{
				m_mutex.lock();
				m_lpvProcessor = NULL;
				m_state = ThreadStateShuttingDown;
				m_mutex.unlock();
				return false;
			}
		}
		m_mutex.lock();
		m_lpvProcessor = NULL;
		m_state = ThreadStateWaiting;
	}
	else
	{
		if( !OnTask() )
		{
			m_mutex.lock();
			m_state = ThreadStateShuttingDown;
			m_mutex.unlock();
			return false;
		}
		m_mutex.lock();
		m_state = ThreadStateWaiting;
	}

	m_mutex.unlock();

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

	m_mutex.lock();
	chEventsWaiting = m_queuePos;
	m_mutex.unlock();

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
,m_bRunning(false)
#ifdef CAESARIA_PLATFORM_WIN
,m_thread(NULL)
#endif
,m_dwId(0L)
,m_lppvQueue(NULL)
,m_chQueue(QUEUE_SIZE)
,m_queuePos(0)
,m_lpvProcessor(NULL)
,m_state(ThreadStateDown)
,m_dwIdle(100)
,m_type(ThreadTypeNotDefined)
,m_stackSize(DEFAULT_STACK_SIZE)
{

	m_dwObjectCondition = NO_ERRORS;

	m_lppvQueue = new void*[QUEUE_SIZE];

	if( !m_lppvQueue ) 
	{
		m_dwObjectCondition |= MEMORY_FAULT;
		m_state = ThreadStateFault;
		return;
	}

	if( !m_mutex.m_bCreated )
	{
		perror("mutex creation failed");
		m_dwObjectCondition |= MUTEX_CREATION;
		m_state = ThreadStateFault;
		return;
	}


	if( !m_event.m_bCreated )
	{
		perror("event creation failed");
		m_dwObjectCondition |= EVENT_CREATION;
		m_state = ThreadStateFault;
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
	m_mutex.lock();
		fValue = (float)m_queuePos/m_chQueue;
	m_mutex.unlock();
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

	m_mutex.lock();
	_CAESARIA_DEBUG_BREAK_IF(ch > m_queuePos);

	if( ch <= m_queuePos )
	{
		cerr << "Warning CThread::SetQueueSize:\n\tthe new queue size is less than the number of tasks on a non-empty queue! Request ignored.\n";
		m_mutex.unlock();
		return false;
	}

	newQueue = new void*[ch];
	if(  !newQueue )
	{
		cerr << "Warning CThread::SetQueueSize:\n\ta low memory, could not reallocate queue!\n";
		m_mutex.unlock();
		return false;
	}

	for( unsigned int i=0;i<m_queuePos; i++ )
	{
		newQueue[i] = m_lppvQueue[i];
	}
	delete [] m_lppvQueue;

	m_chQueue = ch;
	m_lppvQueue = newQueue;

	m_mutex.unlock();

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
	m_mutex.lock();
	if( m_queuePos <= 0 )
	{
		m_mutex.unlock();
		return true;
	}
	m_mutex.unlock();
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

	m_mutex.lock();

	if( m_queuePos+1 >= m_chQueue )
	{
		m_dwObjectCondition |= STACK_OVERFLOW;
		m_mutex.unlock();
		return false;
	}

	if( m_dwObjectCondition & STACK_EMPTY )
	{
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_EMPTY;
	}

	if( m_dwObjectCondition & STACK_OVERFLOW ) 
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_OVERFLOW;

	m_lppvQueue[m_queuePos++] = lpv;
	if( m_queuePos+1 >= m_chQueue )
		m_dwObjectCondition |= STACK_FULL;

	m_mutex.unlock();
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
	m_mutex.lock();
	if( m_queuePos-1 < 0 )
	{
		m_queuePos = 0;
		m_dwObjectCondition |= STACK_EMPTY;
		m_mutex.unlock();
		return false;
	}
	if( m_dwObjectCondition & STACK_EMPTY )
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_EMPTY;
	if( m_dwObjectCondition & STACK_OVERFLOW )
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_OVERFLOW;
	if( m_dwObjectCondition & STACK_FULL )
		m_dwObjectCondition = m_dwObjectCondition ^ STACK_FULL;

	m_queuePos--;
	m_lpvProcessor = m_lppvQueue[m_queuePos];
	m_mutex.unlock();
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


		m_mutex.lock();
		m_dwIdle = dwIdle;


		if( m_type == typ ) {
			m_mutex.unlock();
			return;
		}
		if( m_dwObjectCondition & ILLEGAL_USE_OF_EVENT )
			m_dwObjectCondition = m_dwObjectCondition ^ ILLEGAL_USE_OF_EVENT;
		if( m_dwObjectCondition & EVENT_AND_TYPE_DONT_MATCH )
			m_dwObjectCondition = m_dwObjectCondition ^ EVENT_AND_TYPE_DONT_MATCH;

		m_type = typ;


		m_mutex.unlock();
		m_event.set();
	}
	catch (char *psz)
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CThread::SetThreadType",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CThread::SetThreadType(ThreadType_t typ):" << psz;
#endif

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

		m_mutex.lock();
		m_bRunning = false;
		m_mutex.unlock();
		m_event.set();

		int ticks = (m_StopTimeout*1000)/100;

		for( int i=0; i<ticks; i++ )
		{
			msleep(100);

			m_mutex.lock();
			if( m_state == ThreadStateDown )
			{
				m_mutex.unlock();
				return true;
			}
			m_mutex.unlock();

		} 
	}
	catch (char *psz)
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CThread::Stop",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CThread::Stop():" << psz;
#endif

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
	m_mutex.lock();
	m_dwIdle = dwIdle;
	m_mutex.unlock();
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


		m_mutex.lock();
		if( m_bRunning ) 
		{
			m_mutex.unlock();
			return true;
		}

		m_mutex.unlock();


		if( m_dwObjectCondition & THREAD_CREATION )
			m_dwObjectCondition = m_dwObjectCondition ^ THREAD_CREATION;

#ifdef CAESARIA_PLATFORM_WIN
		if( m_thread )
			CloseHandle(m_thread);

		m_thread = CreateThread(NULL,m_stackSize,_THKERNEL,(LPVOID)this,0,&m_dwId);

		if( !m_thread )
		{
			perror("thread creation failed");
			m_dwObjectCondition |= THREAD_CREATION;
			m_state = ThreadStateFault;
			return false;
		}
#elif defined(CAESARIA_PLATFORM_LINUX)
		pthread_attr_t attr;

		pthread_attr_init(&attr);

		if( m_stackSize != 0 )
			pthread_attr_setstacksize(&attr,m_stackSize);

		int error = pthread_create(&m_thread,&attr,_THKERNEL,(void*)this);

		if( error != 0 )
		{
			m_dwObjectCondition |= THREAD_CREATION;
			m_state = ThreadStateFault;

			switch(error)/* show the thread error */
			{
			case EINVAL:
					cerr << "error: attr in an invalid thread attributes object\n";
					break;
			case EAGAIN:
					cerr << "error: the necessary resources to create a thread are not\n";
					cerr << "available.\n";
					break;
			case EPERM:
					cerr << "error: the caller does not have the privileges to create\n";
					cerr << "the thread with the specified attr object.\n";
			break;
			default:
				cerr << "error: an unknown error was encountered attempting to create\n";
				cerr << "the requested thread.\n";
			break;
			}
		}
#else
			cerr << "error: could not create thread, pthread_create failed (" << error << ")!\n";
#endif
		return false;
	}
	catch( char *psz )
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CThread::Start",MB_ICONHAND);
#else
		cerr << "Fatal exception CThread::Start():" << psz;
#endif
		exit(-1);
	}

	return true;
}

void Thread::getID(ThreadID* pId)
{
  memcpy(pId,&m_dwId,sizeof(ThreadID));
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
	m_mutex.lock();
		if( ((m_dwObjectCondition & STACK_OVERFLOW ||
			  m_dwObjectCondition & STACK_FULL ) &&
			  m_state == ThreadStateBusy) || !m_bRunning)
		{
			m_mutex.unlock();
			return true;
		}
	m_mutex.unlock();
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
	m_mutex.lock();
		currentState = m_state;
	m_mutex.unlock();
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
	if( m_bRunning ) // gracefull termination
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
#ifdef CAESARIA_PLATFORM_WIN
		    MessageBoxA(NULL,&psz[2],"Fatal exception CThread::Stop",MB_ICONHAND);
		    exit(-1);
#else
			cerr << "Fatal exception CThread::Stop: " << psz;
#endif
		}
	}
#ifdef CAESARIA_PLATFORM_WIN
	CloseHandle(m_thread);
#endif

	delete [] m_lppvQueue;
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
		m_mutex.lock();
		if( m_bRunning )
		{
			m_mutex.unlock();
			return true;
		}
		dwTotal += m_dwIdle;
		m_mutex.unlock();
		msleep(m_dwIdle);
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




		if( !m_bRunning ) return;


#ifdef CAESARIA_PLATFORM_WIN
		WaitForSingleObject(m_thread,INFINITE);
#else
		void* lpv;

		pthread_join(m_thread,&lpv);
#endif
	}
	catch( char *psz )
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CThread::WaitTillExit",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CThread::WaitTillExit: " << psz;
#endif

	}
}


bool Thread::ThreadIdsEqual(ThreadID* p1, ThreadID* p2)

{
/*#if defined(AS400)||defined(OS400)
	return(( memcmp(p1,p2,sizeof(ThreadId_t))==0)?TRUE:FALSE);
#elif defined(VMS)
	return (( pthread_equal(*p1,*p2) )?TRUE:FALSE );
#else */
	return ((*p1 == *p2)?true:false);
	//#endif
}

ThreadID Thread::getID()
{
	ThreadID thisThreadsId;
#ifdef CAESARIA_PLATFORM_WIN
	thisThreadsId = (ThreadID)GetCurrentThreadId();
#else
	thisThreadsId = pthread_self();
#endif
	return thisThreadsId;
}
