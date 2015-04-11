/*
 *   C3Mapper - create minimaps from Caesar 3 scenarios and saved games
 *   Copyright (C) 2007  Bianca van Schaik
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef pkwareinputstream_h
#define pkwareinputstream_h

#include <string>
#include "core/bytearray.hpp"
#include "core/scopedptr.hpp"
//#include <istream>

/**
* Exception class for errors
*/
class PKException {
	public:
		std::string msg;
		PKException(std::string msg) {
			this->msg = msg;
		}
};

/**
* Internal class
*/
class PKDictionary;

/**
* Input class for reading files / blocks of data compressed with the
* PKWare Compression Library.
* All methods (including constructors) may throw a PKException
*/
class PKWareInputStream {
	public:
		/**
		* Constructor
		* @param filename Name of the file to open
		* @param file_length Length of the file to open, or -1 if the
		* constructor should figure it out itself
		*/
		PKWareInputStream(std::string filename, int file_length = -1);
		
		/**
		* Constructor
		* @param i Open input stream to read from
		* @param close_stream Whether to close the stream upon destroying
		* this object. Set to FALSE if you want to continue reading from
		* the stream after decompressing
		* @param file_length Length of the compressed data. If not given, the
		* object tries to figure it out, but may read beyond the end of the
		* compressed block
		*/
		PKWareInputStream(std::istream *i, bool close_stream = true, int file_length = -1);
		~PKWareInputStream();
		
		/**
		* Reads a single byte from the compressed stream
		*/
		unsigned char read();
		
		/**
		* Reads a block of data of maximum length `length' from the stream
		* @param buf Place to put read data
		* @param length Maximum length to read
		* @return int Number of bytes actually read, may be less than
		* `length' if and only if EOF is encountered
		*/
		int read(unsigned char *buf, int length);
		
		/**
		* Reads a byte from the input stream. Same as read()
		*/
		unsigned char readByte();
		
		/**
		* Reads a little-endian short (2 bytes) from the input stream.
		*/
		unsigned short readShort();
		
		/**
		* Reads a little-endian int (4 bytes) from the input stream.
		*/
		unsigned int readInt();
		
		/**
		* Skips over `length' bytes.
		*/
		void skip(int length);
		
		/**
		* Empties the stream, reading until EOF is encountered
		*/
		void empty();
		
	private:
		void init();
		void readHeader();
		int getCopyLength();
		int getCopyOffset(int length);
		int getCopyOffsetHigh();
		int reverse(int number, int length);
		void fillBuffer();
		void advanceByte();
		unsigned char readBit();
		int readBits(int length);
		
		// Class variables (comments is where they're initialised)
		std::istream *input; // ctor
		int dictSize; // readHeader
    ByteArray buffer; // fillBuffer
		int bufOffset; // fillBuffer
		int bufBit; // init
		int dictionary_bits; // readHeader
    ScopedPtr<PKDictionary> dictionary; // readHeader
		
		// For the reading of bytes:
		int read_offset; // init
		int read_length; // init
		bool read_copying; // init
		int file_length; // ctor or init
		
		// For detecting end of stream:
		bool eof_reached; // init, fillBuffer
		int eof_position; // fillBuffer
		bool close_stream; // ctor
		static const int BUFFER_SIZE = 4096;
};

#endif /* pkwareinputstream_h */
