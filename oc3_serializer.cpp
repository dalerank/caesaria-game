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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "oc3_serializer.hpp"
#include "oc3_exception.hpp"

#include <iostream>

SerialStream::SerialStream()
{
   _stream = NULL;
   _version = 0;

   _mapObjectById[0] = NULL;
   _mapIdByObject[NULL] = 0;
}

void* SerialStream::id2object(const int objectID)
{
   void * res;
   std::map<int, void*>::iterator mapIt;
   mapIt = _mapObjectById.find(objectID);
   if (mapIt == _mapObjectById.end()) THROW("Unknown objectID " << objectID);
   res = mapIt->second;

   return res;
}

bool SerialStream::know_id(const int objectID)
{
   bool res = false;
   std::map<int, void*>::iterator mapIt;
   mapIt = _mapObjectById.find(objectID);
   if (mapIt != _mapObjectById.end())
   {
      // found object
      res = true;
   }
   return res;
}

bool SerialStream::know_object(void *object)
{
   bool res = false;
   std::map<void*, int>::iterator mapIt;
   mapIt = _mapIdByObject.find(object);
   if (mapIt != _mapIdByObject.end())
   {
      // found object
      res = true;
   }
   return res;
}

int SerialStream::object2id(void* object)
{
   int res = 0;

   std::map<void*, int>::iterator mapIt;
   mapIt = _mapIdByObject.find(object);
   if (mapIt == _mapIdByObject.end())
   {
      // object not found in the map
      THROW("Unknown object:" << object);
   }
   else
   {
      res = mapIt->second;
   }

   return res;
}

void SerialStream::link(int objectID, void* object)
{
   _mapIdByObject[object] = objectID;
   _mapObjectById[objectID] = object;
}



InputSerialStream::InputSerialStream()
{
   _maxObjectID = 0;  // 0 is reserved for NULL
}

void InputSerialStream::init(std::fstream &stream, const int version)
{
   _stream = &stream;
   _version = version;

   _stream->seekg(-4, std::ios::end);  // 4 last bytes of file
   _maxObjectID = read_int(4, 1, 1000000);
   _stream->seekg(0, std::ios::beg);  // return to the begin of file
}

void InputSerialStream::read(char* s, std::streamsize n)
{
   _stream->read(s, n);
}

void InputSerialStream::close()
{
   _stream->close();
}

long InputSerialStream::tell()
{
   return _stream->tellg();
}

int InputSerialStream::read_objectID(void** objectRef)
{
   int objectID = read_int(4, 0, _maxObjectID);

   if (objectRef != NULL)
   {
      // object_pointer must be set in reference
      if (objectID == 0)
      {
         // trivial case
         *objectRef = NULL;
      }
      else
      {
         _mapIdByRef[objectRef] = objectID;
      }
   }
   return objectID;

}

std::string InputSerialStream::read_fix_str(const unsigned int size)
{
   char buffer[65536];
   read(buffer, size);
   std::string res(buffer, size);
   return res;
}

std::string InputSerialStream::read_str(const long maxLength)
{
   char buffer[65536];
   unsigned int length = read_int(2, 0, maxLength);
   read(buffer, length);
   std::string res(buffer, length);
   return res;
}

long InputSerialStream::read_int(const unsigned size, const long min, const long max)
{
   long res = 0;
   char res1;
   short res2;
   switch (size)
   {
      // that crap is used to perform sign extension
   case 1:
      read((char*) &res1, size);
      res = res1;
      break;
   case 2:
      read((char*) &res2, size);
      res = res2;
      break;
   case 4:
      read((char*) &res, size);
      break;
   default:
      THROW("Invalid int unserialization size " << size);
   }
   if (res < min || res > max) THROW("Integer overflow, value=" << res);
   return res;
}

void InputSerialStream::set_dangling_pointers(const bool error_on_missing)
{
   for (std::map<void**, int>::iterator mapIt = _mapIdByRef.begin(); mapIt != _mapIdByRef.end(); ++mapIt)
   {
      void** objectRef = mapIt->first;
      int objectID = mapIt->second;

      if (know_id(objectID))
      {
         // objectID is linked
         *objectRef = id2object(objectID);
         _mapIdByRef.erase(mapIt);
      }
      else
      {
         // objectID is still missing...
         if (error_on_missing)
         {
            THROW("Unknown objectID:" << objectID);
         }
      }

   }
}

void InputSerialStream::finalize_read()
{
   set_dangling_pointers();
}


OutputSerialStream::OutputSerialStream()
{
   _nextObjectID = 1;  // 0 is reserved for NULL
   _mapObjectById[0] = NULL;
   _mapIdByObject[NULL] = 0;
}

void OutputSerialStream::init(std::fstream &stream, const int version)
{
   _stream = &stream;
   _version = version;
}

void OutputSerialStream::write(const char* s, std::streamsize n)
{
   _stream->write(s, n);
}

void OutputSerialStream::close()
{
   _stream->close();
}

long OutputSerialStream::tell()
{
   return _stream->tellp();
}

void OutputSerialStream::write_str(const std::string &s, const long maxLength)
{
   int length = s.size();
   write_int(length, 2, 0, maxLength);
   write(s.c_str(), length);
}

void OutputSerialStream::write_int(const long i, const unsigned size, const long min, const long max)
{
   if (i < min || i > max) THROW("Integer overflow, value=" << i);
   write((char*) &i, size);
}

void OutputSerialStream::write_objectID(void *object)
{
   int objectID = 0;
   if (know_object(object))
   {
      objectID = object2id(object);
   }
   else
   {
      objectID = _nextObjectID;
      _nextObjectID++;
      link(objectID, object);
   }

   write_int(objectID, 4, 0, 1000000);
}

void OutputSerialStream::finalize_write()
{
   write_int(_nextObjectID, 4, 0, 1000000);
}

