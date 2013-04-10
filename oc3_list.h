// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_LIST_H_INCLUDED__
#define __OPENCAESAR3_LIST_H_INCLUDED__

#include "oc3_math.h"
#include "oc3_allocator.h"

//! Doubly linked list template.
template <class T>
class List
{
private:

	//! List element node with pointer to previous and next element in the list.
	struct SKListNode
	{
		SKListNode(const T& e) : Next(0), Prev(0), Element(e) {}

		SKListNode* Next;
		SKListNode* Prev;
		T Element;
	};

public:
	class const_iterator;

	//! List iterator.
	class iterator
	{
	public:
		iterator() : Current(0) {}

		iterator& operator ++()    { Current = Current->Next; return *this; }
		iterator& operator --()    { Current = Current->Prev; return *this; }
		iterator  operator ++(int) { iterator tmp = *this; Current = Current->Next; return tmp; }
		iterator  operator --(int) { iterator tmp = *this; Current = Current->Prev; return tmp; }

		iterator& operator +=(int num)
		{
			if(num > 0)
			{
				while (num-- && this->Current != 0) ++(*this);
			}
			else
			{
				while(num++ && this->Current != 0) --(*this);
			}
			return *this;
		}

		iterator  operator + (int num) const { iterator tmp = *this; return tmp += num; }
		iterator& operator -=(int num) const { return (*this)+=(-num); }
		iterator  operator - (int num) const { return (*this)+ (-num); }

		bool operator ==(const iterator&      other) const { return Current == other.Current; }
		bool operator !=(const iterator&      other) const { return Current != other.Current; }
		bool operator ==(const const_iterator& other) const { return Current == other.Current; }
		bool operator !=(const const_iterator& other) const { return Current != other.Current; }

		#if defined (_MSC_VER) && (_MSC_VER < 1300)
			#pragma warning(disable:4284) // infix notation problem when using iterator operator ->
		#endif

		T & operator * () { return Current->Element; }
		T * operator ->() { return &Current->Element; }

	private:
		explicit iterator(SKListNode* begin) : Current(begin) {}

		SKListNode* Current;

		friend class List<T>;
		friend class const_iterator;
	};

	//! List iterator for const access.
	class const_iterator
	{
	public:

		const_iterator() : Current(0) {}
		const_iterator(const iterator& iter) : Current(iter.Current)  {}

		const_iterator& operator ++()    { Current = Current->Next; return *this; }
		const_iterator& operator --()    { Current = Current->Prev; return *this; }
		const_iterator  operator ++(int) { const_iterator tmp = *this; Current = Current->Next; return tmp; }
		const_iterator  operator --(int) { const_iterator tmp = *this; Current = Current->Prev; return tmp; }

		const_iterator& operator +=(int num)
		{
			if(num > 0)
			{
				while(num-- && this->Current != 0) ++(*this);
			}
			else
			{
				while(num++ && this->Current != 0) --(*this);
			}
			return *this;
		}

		const_iterator  operator + (int num) const { const_iterator tmp = *this; return tmp += num; }
		const_iterator& operator -=(int num) const { return (*this)+=(-num); }
		const_iterator  operator - (int num) const { return (*this)+ (-num); }

		bool operator ==(const const_iterator& other) const { return Current == other.Current; }
		bool operator !=(const const_iterator& other) const { return Current != other.Current; }
		bool operator ==(const iterator&      other) const { return Current == other.Current; }
		bool operator !=(const iterator&      other) const { return Current != other.Current; }

		const T & operator * () { return Current->Element; }
		const T * operator ->() { return &Current->Element; }

		const_iterator & operator =(const iterator& it) { Current = it.Current; return *this; }

	private:
		explicit const_iterator(SKListNode* begin) : Current(begin) {}

		SKListNode* Current;

		friend class iterator;
		friend class List<T>;
	};

	//! Default constructor for empty list.
	List()
		: _first(0), _last(0), _size(0) {}


	//! Copy constructor.
	List(const List<T>& other) : _first(0), _last(0), _size(0)
	{
		*this = other;
	}


	//! Destructor
	~List()
	{
		clear();
	}


	//! Assignment operator
	void operator=(const List<T>& other)
	{
		if(&other == this)
		{
			return;
		}

		clear();

		SKListNode* node = other._first;
		while(node)
		{
			push_back(node->Element);
			node = node->Next;
		}
	}


	//! Returns amount of elements in list.
	/** \return Amount of elements in the list. */
	unsigned int size() const
	{
		return _size;
	}

	//! Clears the list, deletes all elements in the list.
	/** All existing iterators of this list will be invalid. */
	void clear()
	{
		while(_first)
		{
			SKListNode * next = _first->Next;
			_allocator.destruct(_first);
			_allocator.deallocate(_first);
			_first = next;
		}

		//First = 0; handled by loop
		_last = 0;
		_size = 0;
	}


	//! Checks for empty list.
	/** \return True if the list is empty and false if not. */
	bool empty() const
	{
		return (_first == 0);
	}

    //! Adds an element at the end of the list.
    /** \param element Element to add to the list. */
    void append(const T& element)
    {
        push_back( element );
    }

	//! Adds an element at the end of the list.
	/** \param element Element to add to the list. */
	void push_back(const T& element)
	{
		SKListNode* node = _allocator.allocate(1);
		_allocator.construct(node, element);

		++_size;

		if (_first == 0)
			_first = node;

		node->Prev = _last;

		if (_last != 0)
			_last->Next = node;

		_last = node;
	}


	//! Adds an element at the begin of the list.
	/** \param element: Element to add to the list. */
	void push_front(const T& element)
	{
		SKListNode* node = _allocator.allocate(1);
		_allocator.construct(node, element);

		++_size;

		if (_first == 0)
		{
			_last = node;
			_first = node;
		}
		else
		{
			node->Next = _first;
			_first->Prev = node;
			_first = node;
		}
	}


	//! Gets first node.
	/** \return A list iterator pointing to the beginning of the list. */
	iterator begin()
	{
		return iterator(_first);
	}


	//! Gets first node.
	/** \return A const list iterator pointing to the beginning of the list. */
	const_iterator begin() const
	{
		return const_iterator(_first);
	}


	//! Gets end node.
	/** \return List iterator pointing to null. */
	iterator end()
	{
		return iterator(0);
	}


	//! Gets end node.
	/** \return Const list iterator pointing to null. */
	const_iterator end() const
	{
		return const_iterator(0);
	}


	//! Gets last element.
	/** \return List iterator pointing to the last element of the list. */
	iterator getLast()
	{
		return iterator(_last);
	}


	//! Gets last element.
	/** \return Const list iterator pointing to the last element of the list. */
	const_iterator getLast() const
	{
		return const_iterator(_last);
	}


	//! Inserts an element after an element.
	/** \param it iterator pointing to element after which the new element
	should be inserted.
	\param element The new element to be inserted into the list.
	*/
	void insert_after(const iterator& it, const T& element)
	{
		SKListNode* node = _allocator.allocate(1);
		_allocator.construct(node, element);

		node->Next = it.Current->Next;

		if (it.Current->Next)
			it.Current->Next->Prev = node;

		node->Prev = it.Current;
		it.Current->Next = node;
		++_size;

		if (it.Current == _last)
			_last = node;
	}


	//! Inserts an element before an element.
	/** \param it iterator pointing to element before which the new element
	should be inserted.
	\param element The new element to be inserted into the list.
	*/
	void insert_before(const iterator& it, const T& element)
	{
		SKListNode* node = _allocator.allocate(1);
		_allocator.construct(node, element);

		node->Prev = it.Current->Prev;

		if (it.Current->Prev)
			it.Current->Prev->Next = node;

		node->Next = it.Current;
		it.Current->Prev = node;
		++_size;

		if (it.Current == _first)
			_first = node;
	}


	//! Erases an element.
	/** \param it iterator pointing to the element which shall be erased.
	\return iterator pointing to next element. */
	iterator erase(iterator& it)
	{
		// suggest changing this to a const iterator& and
		// working around line: it.Current = 0 (possibly with a mutable, or just let it be garbage?)

		iterator returnIterator(it);
		++returnIterator;

		if(it.Current == _first)
		{
			_first = it.Current->Next;
		}
		else
		{
			it.Current->Prev->Next = it.Current->Next;
		}

		if(it.Current == _last)
		{
			_last = it.Current->Prev;
		}
		else
		{
			it.Current->Next->Prev = it.Current->Prev;
		}

		_allocator.destruct(it.Current);
		_allocator.deallocate(it.Current);
		it.Current = 0;
		--_size;

		return returnIterator;
	}

    //! Swap the content of this list container with the content of another list
	/** Afterwards this object will contain the content of the other object and the other
	object will contain the content of this object. Iterators will afterwards be valid for
	the swapped object.
	\param other Swap content with this object	*/
	void swap(List<T>& other)
	{
                std::swap(_first, other._first);
                std::swap(_last, other._last);
                std::swap(_size, other._size);
                std::swap(_allocator, other._allocator);	// memory is still released by the same allocator used for allocation
	}


private:

	SKListNode* _first;
	SKListNode* _last;
	unsigned int _size;
    Allocator<SKListNode> _allocator;
};

#endif

