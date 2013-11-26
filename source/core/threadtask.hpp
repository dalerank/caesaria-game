#ifndef _CAESARIA_THREADTASK_H_INCLUDE_
#define _CAESARIA_THREADTASK_H_INCLUDE_

#include "platform.hpp"
#include "mutex.hpp"
#include "predefinitions.hpp"

typedef enum
{
	TaskStatusNotSubmitted,
	TaskStatusWaitingOnQueue,
	TaskStatusBeingProcessed,
	TaskStatusCompleted
} TaskStatus_t;

class CTask
{
private:
	TaskStatus_t m_state;
	ThreadID m_dwThread;

public:
	Mutex m_mutex;

	void setTaskStatus(TaskStatus_t state);

	void setId( ThreadID *pid );

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

	void assign(ThreadID *pId);

	CTask();

	~CTask(){}
	virtual bool task()=0;
};

#endif

