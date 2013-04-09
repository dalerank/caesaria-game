#ifndef __OPENCAESAR3_REFERENCE_COUNTED_H_INCLUDED__
#define __OPENCAESAR3_REFERENCE_COUNTED_H_INCLUDED__

#include <string>
#include "oc3_requirements.h"

class ReferenceCounted
{
public:

	//! Constructor.
	ReferenceCounted()
		: _debugName( "" ), _referenceCounter(1)
	{
	}

	//! Destructor.
	virtual ~ReferenceCounted()
	{
	}

	//! Grabs the object. Increments the reference counter by one.
	void grab() const { ++_referenceCounter; }

	//! Drops the object. Decrements the reference counter by one.
	bool drop() const
	{
		// someone is doing bad reference counting.
		_OC3_DEBUG_BREAK_IF( _referenceCounter <= 0 )

		--_referenceCounter;
		if (!_referenceCounter)
		{
			delete this;
			return true;
		}

		return false;
	}

	//! Get the reference count.
	int getReferenceCount() const
	{
		return _referenceCounter;
	}

	//! Returns the debug name of the object.
    std::string getDebugName() const
	{
		return _debugName;
	}

protected:

	//! Sets the debug name of the object.
	/** The Debugname may only be set and changed by the object
	itself. This method should only be used in Debug mode.
	\param newName: New debug name to set. */
    void setDebugName(const std::string& newName)
	{
		_debugName = newName;
	}

private:

	//! The debug name.
    std::string _debugName;

	//! The reference counter. Mutable to do reference counting on const objects.
	mutable int _referenceCounter;
};

#endif

