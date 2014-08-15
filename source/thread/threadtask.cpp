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
	timeoutSeconds *= 1000;

	while( getStatus() != TaskStatusCompleted && timeoutSeconds > 0 )
	{
		Thread::msleep( 100 );
		timeoutSeconds -= 100;
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
