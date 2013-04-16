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


#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <fstream>
#include <string>
#include <map>

class SerialStream
{
public:
   SerialStream();

   void* id2object(const int objectID);  // returns the object of a given ID, or exception if not found
   bool know_id(const int objectID);  // returns true if the objectID is linked
   bool know_object(void* object);  // returns true if the object is linked
   int object2id(void* object);  // returns the ID of a given object
   void link(int objectID, void* object);  // link object and objectID

   std::fstream *_stream;
   int _version;

protected:
   std::map<void*, int> _mapIdByObject;  // key=object, value=objectID
   std::map<int, void*> _mapObjectById;  // key=objectID, value=object
};


class InputSerialStream : public SerialStream
{
public:
   InputSerialStream();
   void init(std::fstream &stream, const int version);
   void read(char *s, std::streamsize n);
   void close();
   long tell();

   // read methods. min&max arguments are used to check for erroneous input
   std::string read_fix_str(const unsigned int size);
   std::string read_str(const long maxLength);
   long read_int(const unsigned int size, const long min, const long max);
   int read_objectID(void** objectRef = NULL);  // read objectID and link to objectReference

   void set_dangling_pointers(const bool error_on_missing=true);  // set dangling pointers
   void finalize_read();  // finish the read

protected:
   int _maxObjectID;  // used for check of objectID
   std::map<void**, int> _mapIdByRef;  // key=reference, value=objectID
};


class OutputSerialStream : public SerialStream
{
public:
   OutputSerialStream();
   void init(std::fstream &stream, const int version);
   void write(const char *s, std::streamsize n);
   void close();
   long tell();

   // write methods. min&max arguments are used to check for erroneous output
   void write_str(const std::string &s, const long maxLength);
   void write_int(const long i, const unsigned int size, const long min, const long max);
   void write_objectID(void *object);

   void finalize_write();  // finish the write

protected:
   int _nextObjectID;  // used for writing objects to stream
};


class Serializable
{
public:
   // read/write self on stream
   virtual void serialize(OutputSerialStream &stream) = 0;
   virtual void unserialize(InputSerialStream &stream) = 0;
};


#endif
