/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef _OPENCAESAR3_SINGLETON_H_INCLUDE_
#define _OPENCAESAR3_SINGLETON_H_INCLUDE_

#include "requirements.hpp"

/** The "classic" Singleton.
 */
template <typename T>
class StaticSingleton
{
public:
	static T& instance()
	{
		static T inst;
		return inst;
	}

protected:

	StaticSingleton() {}

	virtual ~StaticSingleton() {}

private:
	StaticSingleton(const StaticSingleton<T>&) {}
	StaticSingleton<T>& operator=(const StaticSingleton<T>&) {return this;}
};

template <typename T>
class DynamicSingleton
{
public:
	static T* instance()
	{
		_CAESARIA_DEBUG_BREAK_IF( !m_instance );
		return m_instance;
	}

	DynamicSingleton()
	{
		_CAESARIA_DEBUG_BREAK_IF( m_instance );
		m_instance = static_cast<T*>(this);
	}

	virtual ~DynamicSingleton()
	{
		m_instance = 0;
	}

private:
	static T* m_instance;

	DynamicSingleton(const DynamicSingleton<T>&) {}
	DynamicSingleton<T&> operator=(const DynamicSingleton<T>&) {}
};

template <typename T> T* DynamicSingleton<T>::m_instance = 0;

#endif
