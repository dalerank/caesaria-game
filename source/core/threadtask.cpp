#include "threadtask.hpp"
#include "thread.hpp"

#include <string.h>

void CTask::setTaskStatus(TaskStatus_t state)
{
	m_mutex.lock();
	m_state=state;
	m_mutex.unlock();
}

void CTask::setId(ThreadID* pid)
{
	memcpy(&m_dwThread,pid,sizeof(ThreadID));
}

bool CTask::wait(int timeoutSeconds)
{
	timeoutSeconds = timeoutSeconds * 1000;

	if( getStatus() != TaskStatusCompleted && timeoutSeconds > 0 )
	{
		Thread::msleep( 100 );
		timeoutSeconds = timeoutSeconds - 100;
	}

	if( getStatus() == TaskStatusCompleted ) return true;

	return false;
}

TaskStatus_t CTask::getStatus()
{
	TaskStatus_t state ;

	m_mutex.lock();
	state = m_state;
	m_mutex.unlock();

	return state;
}

void CTask::assign(ThreadID* pId)
{
	memcpy(pId,&m_dwThread,sizeof(ThreadID));
}

CTask::CTask()
{
	m_state=TaskStatusNotSubmitted;
	memset(&m_dwThread,0,sizeof(ThreadID));
}
