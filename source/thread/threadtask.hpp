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

#ifndef _CAESARIA_THREADTASK_H_INCLUDE_
#define _CAESARIA_THREADTASK_H_INCLUDE_

#include "core/platform.hpp"
#include <mutex>
#include <thread>

typedef enum
{
	TaskStatusNotSubmitted,
	TaskStatusWaitingOnQueue,
	TaskStatusBeingProcessed,
	TaskStatusCompleted
} TaskStatus_t;

class ThreadTask
{
private:
	TaskStatus_t m_state;
  std::thread::id m_dwThread;

public:
  std::mutex m_mutex;

	void setTaskStatus(TaskStatus_t state);

  void setId( std::thread::id *pid );

	/**
	 *
	 * Wait
	 * waits for upto timeoutSeconds for a task
	 * to complete
	 *
	 **/
	bool wait(int timeoutSeconds);

	/**
	 *
	 * Status
	 * returns current state of a task
	 *
	 **/
	TaskStatus_t getStatus();

  void assign(std::thread::id *pId);

  ThreadTask();

  virtual ~ThreadTask(){}
	virtual bool task()=0;
};

#endif

