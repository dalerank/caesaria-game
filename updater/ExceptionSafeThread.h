/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5122 $ (Revision of last commit) 
 $Date: 2011-12-11 23:47:31 +0400 (Вс, 11 дек 2011) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include <stdexcept>
#include <string>
#include "core/delegate.hpp"
#include "core/thread.hpp"

namespace tdm
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
	Delegate1<int> _function;

	// The error message which is filled when exceptions are thrown within the thread
	std::string _errMsg;

	bool _interrupted;

	bool _done;

	int _type;

	Delegate1<int> _onFinish;

public:
	ExceptionSafeThread(const Delegate1<int>& function) :
		_function(function),
		_interrupted(false),
		_done(false)
	{}

	// Construct this thread with a callback which will be invoked when the thread is done (failure or not)
	ExceptionSafeThread(const Delegate1<int>& function, const Delegate1<int>& callbackOnFinish, int type) :
		_function(function),		
		_interrupted(false),
		_done(false),
		_type( type ),
		_onFinish(callbackOnFinish)
	{}

	~ExceptionSafeThread()
	{
		if(!_done)
		{
			Stop();
		}
	}

	bool done() const
	{
		return _done;
	}

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
				_function( _type );
			}
		}
		catch( std::runtime_error& ex )
		{
			_errMsg = ex.what();
		}

		_done = true;

		// Invoke the callback when done
		if( !_onFinish.empty() )
		{
			// This might destroy this object, don't do anything afterwards
			_onFinish( _type );
		}

		return false;
	}
};
typedef SmartPtr<ExceptionSafeThread> ExceptionSafeThreadPtr;

} // namespace
