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

#ifndef __CAESARIA_EXCEPTIONSAFETHREAD_H_INLCUDE__
#define __CAESARIA_EXCEPTIONSAFETHREAD_H_INLCUDE__

#include <stdexcept>
#include <string>
#include "core/delegate.hpp"
#include "thread/thread.hpp"
#include "thread/threadtask.hpp"

namespace updater
{

// Thrown to the calling thread if the user hits Ctrl-C
class UserInterruptException : public std::runtime_error
{
public:
	UserInterruptException(const std::string& message) :
		std::runtime_error(message)
	{}
};

/**
 * greebo: A wrapper class which is able to pass exceptions 
 * back to the calling thread. The function passed to boost::thread
 * is decorated with a try/catch block, which is storing the exception's
 * error message locally. After joining the thread the exception is 
 * re-generated such that the calling thread gets a chance to catch it.
 */
class ExceptionSafeThread : public Thread
{
private:
	// The function object to call
	Delegate0<> _function;

	// The error message which is filled when exceptions are thrown within the thread
	std::string _errMsg;

	Delegate0<> _onFinish;

public:
	ExceptionSafeThread(const Delegate0<>& function) :
		_function(function)
	{}

	// Construct this thread with a callback which will be invoked when the thread is done (failure or not)
	ExceptionSafeThread(const Delegate0<>& function, const Delegate0<>& callbackOnFinish) :
		_function(function),		
		_onFinish(callbackOnFinish)
	{}

	// Returns true if the worker thread threw an exception
	bool failed() const
	{
		return !_errMsg.empty();
	}

	const std::string& GetErrorMessage() const
	{
		return _errMsg;
	}

private:
	bool OnTask()
	{
		try
		{
			if( !_function.empty() )
			{
				_function();
			}
		}
		catch( std::runtime_error& ex )
		{
			_errMsg = ex.what();
		}

		return false;
	}
};
typedef SmartPtr<ExceptionSafeThread> ExceptionSafeThreadPtr;

} // namespace

#endif //__CAESARIA_EXCEPTIONSAFETHREAD_H_INLCUDE__
