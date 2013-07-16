#ifndef __OPENCAESAR3_FLAGHOLDER_H_INCLUDE_
#define __OPENCAESAR3_FLAGHOLDER_H_INCLUDE_

template< class T >
class FlagHolder 
{
public:
    FlagHolder() : _flags( 0 ) {}

	void setFlag( T flag, bool enabled=true )
	{
        if( enabled )
		    _flags |= flag;
        else
            _flags &= ~flag;
	}

    void resetFlag( T flag )
    {
        setFlag( flag, false );
    }

	bool isFlag( T flag ) const
	{
		return (_flags & flag) > 0;
	}

	void setFlags( int val )
	{
		_flags = val;
	}
private:

	int _flags;
};

#endif

