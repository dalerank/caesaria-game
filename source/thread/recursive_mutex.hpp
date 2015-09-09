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

#ifndef _CAESARIA_RECURSIVEMUTEX_H_INCLUDE_
#define _CAESARIA_RECURSIVEMUTEX_H_INCLUDE_

#include "core/platform.hpp"
#include "core/predefinitions.hpp"

#ifdef CAESARIA_PLATFORM_UNIX
	#include <pthread.h>
#elif defined(CAESARIA_PLATFORM_WIN)
	#include <windows.h>
#endif

class RecursiveMutex
{
public:
  class LockHandle
  {
  public:
      LockHandle(LockHandle&& lh) : m_CriticalSection(lh.m_CriticalSection) { lh.m_CriticalSection = nullptr; }
      ~LockHandle() { if (m_CriticalSection != nullptr) m_CriticalSection->Leave(); }

  private:
      // Not copyable
      LockHandle(const LockHandle&);
      LockHandle& operator=(const LockHandle&);

      friend class RecursiveMutex;
      LockHandle(RecursiveMutex* cs) : m_CriticalSection(cs)
      {
          m_CriticalSection->Enter();
      }

      RecursiveMutex* m_CriticalSection;
  };

  RecursiveMutex() {}
  ~RecursiveMutex() {}

	void Enter() { m_CriticalSection.lock(); }
	void Leave() { m_CriticalSection.unlock(); }
    
	LockHandle scopeLock() { return LockHandle(this); }

private:
    // Not copyable
    RecursiveMutex(const RecursiveMutex&);
    RecursiveMutex& operator=(const RecursiveMutex&);

    std::recursive_mutex m_CriticalSection;
};

#endif //_CAESARIA_RECURSIVEMUTEX_H_INCLUDE_

