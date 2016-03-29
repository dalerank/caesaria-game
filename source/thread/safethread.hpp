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

#ifndef __CAESARIA_THREADING_SAFETHREAD_H_INLCUDE__
#define __CAESARIA_THREADING_SAFETHREAD_H_INLCUDE__

#include <stdexcept>
#include <string>
#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"
#include "core/delegate.hpp"

namespace threading
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
class SafeThread : public ReferenceCounted
{
  friend void _THKERNEL(SafeThread*);
public:
  typedef Delegate1<bool&> WorkFunction;
  typedef Delegate0<> StopFunction;
  static SmartPtr<SafeThread> create( WorkFunction function);

	// Construct this thread with a callback which will be invoked when the thread is done (failure or not)
  SafeThread( WorkFunction function, StopFunction callbackOnFinish);

	// Returns true if the worker thread threw an exception
  bool failed() const;

  const std::string& errorMessage() const;
  void join();
  void stop();
  void abort();
  void setDelay(int ms);
  bool running() const;

private:
  SafeThread(WorkFunction function);

  class Impl;
  ScopedPtr<Impl> _d;
};

typedef SmartPtr<SafeThread> SafeThreadPtr;

} // threading

#endif //__CAESARIA_THREADING_SAFETHREAD_H_INLCUDE__
