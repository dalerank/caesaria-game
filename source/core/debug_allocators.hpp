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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_DEBUG_ALLOCATORS_H_INCLUDED__
#define __CAESARIA_DEBUG_ALLOCATORS_H_INCLUDED__

class debug_new
{
public:
  static void add(unsigned int addr, unsigned int asize,  const char *fname, unsigned int lnum);
  static void rem(unsigned int addr);
  static void print();
};

#ifdef USE_DEBUG_NEW
#define DEBUG_NEW new(__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#define new DEBUG_NEW

#ifdef USE_DEBUG_NEW
inline void * __cdecl operator new(unsigned int size,
                                   const char *file, int line)
{
  void *ptr = (void *)malloc(size);
  debug_new::add((DWORD)ptr, size, file, line);
  return(ptr);
};
inline void __cdecl operator delete(void *p)
{
  debug_new::rem((DWORD)p);
  free(p);
};
#endif //USE_DEBUG_NEW

#endif // __CAESARIA_DEBUG_ALLOCATORS_H_INCLUDED__

